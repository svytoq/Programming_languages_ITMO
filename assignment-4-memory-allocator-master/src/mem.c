#define _DEFAULT_SOURCE

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mem_internals.h"
#include "mem.h"
#include "util.h"

void debug_block(struct block_header* b, const char* fmt, ... );
void debug(const char* fmt, ... );

extern inline block_size size_from_capacity( block_capacity cap );
extern inline block_capacity capacity_from_size( block_size sz );

static bool            block_is_big_enough( size_t query, struct block_header* block ) { return block->capacity.bytes >= query; }
static size_t          pages_count   ( size_t mem )                      { return mem / getpagesize() + ((mem % getpagesize()) > 0); }
static size_t          round_pages   ( size_t mem )                      { return getpagesize() * pages_count( mem ) ; }

static void block_init( void* restrict addr, block_size block_sz, void* restrict next ) {
  *((struct block_header*)addr) = (struct block_header) {
    .next = next,
    .capacity = capacity_from_size(block_sz),
    .is_free = true
  };
}

static size_t region_actual_size( size_t query ) { return size_max( round_pages( query ), REGION_MIN_SIZE ); }

extern inline bool region_is_invalid( const struct region* r );



static void* map_pages(void const* addr, size_t length, int additional_flags) {
  return mmap( (void*) addr, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | additional_flags , -1, 0 );
}

struct region new_region (void const * addr, void* reg_addr, size_t actual_size) { 
  return (struct region) {
    .addr = reg_addr,
    .size = actual_size,
    .extends = (addr==reg_addr)
    };
}


/*  аллоцировать регион памяти и инициализировать его блоком */
static struct region alloc_region  ( void const * addr, size_t query ) {

  if (!addr) {
    return REGION_INVALID;
  }

  block_size actual_size = (block_size) {region_actual_size(size_from_capacity((block_capacity) {query}).bytes)};

  void* malloc_addr = map_pages(addr, actual_size.bytes, MAP_FIXED_NOREPLACE);

  if (malloc_addr == MAP_FAILED) {
    malloc_addr = map_pages(addr, actual_size.bytes, 0);

    if (malloc_addr == MAP_FAILED) {
      return REGION_INVALID;
    }
  }
  block_init(malloc_addr, (block_size) {.bytes = actual_size.bytes}, NULL);

  return new_region(addr, malloc_addr, actual_size.bytes);
}

static void* block_after( struct block_header const* block )         ;

void* heap_init( size_t initial ) {
  const struct region region = alloc_region( HEAP_START, initial );
  if ( region_is_invalid(&region) ) return NULL;

  return region.addr;
}

#define BLOCK_MIN_CAPACITY 24

/*  --- Разделение блоков (если найденный свободный блок слишком большой )--- */

static bool block_splittable( struct block_header* restrict block, size_t query) {
  return block-> is_free && query + offsetof( struct block_header, contents ) + BLOCK_MIN_CAPACITY <= block->capacity.bytes;
}

static bool split_if_too_big( struct block_header* block, size_t query ) {

    if (!block_splittable(block, query)) return false;

    size_t current_block_capacity_in_bytes = block->capacity.bytes;

    void *new_block = block->contents + query;
    block_size new_block_size = {current_block_capacity_in_bytes - query};

    block_init(new_block, new_block_size, block->next);

    block->capacity.bytes = query;
    block->next = new_block;

    return true;
}


/*  --- Слияние соседних свободных блоков --- */

static void* block_after( struct block_header const* block )              {
  return  (void*) (block->contents + block->capacity.bytes);
}
static bool blocks_continuous (
                               struct block_header const* fst,
                               struct block_header const* snd ) {
  return (void*)snd == block_after(fst);
}

static bool mergeable(struct block_header const* restrict fst, struct block_header const* restrict snd) {
  return fst->is_free && snd->is_free && blocks_continuous( fst, snd ) ;
}

static bool try_merge_with_next( struct block_header* block ) {
  
  if (!(block->next) || !(mergeable(block, block->next))) {
    return false;
  }

  block->capacity.bytes += size_from_capacity(block->next->capacity).bytes;
  block->next = block->next->next;

  return true;
}


/*  --- ... ecли размера кучи хватает --- */

struct block_search_result {
  enum {BSR_FOUND_GOOD_BLOCK, BSR_REACHED_END_NOT_FOUND, BSR_CORRUPTED} type;
  struct block_header* block;
};

struct block_search_result found_block (struct block_header* block) { 
  return (struct block_search_result) {
      .type = BSR_FOUND_GOOD_BLOCK,
      .block = block
    };
}

struct block_search_result not_found_block (struct block_header* block) { 
  return (struct block_search_result) {
      .type = BSR_REACHED_END_NOT_FOUND,
      .block = block
    };
}


static struct block_search_result find_good_or_last  ( struct block_header* restrict block, size_t sz )    {
  
  if (!block) {
      return (struct block_search_result){.type = BSR_CORRUPTED};
  }

  while (block) {
      while (try_merge_with_next(block));

      if(block_is_big_enough(sz, block) && block->is_free) {
        return found_block(block);
      }

    if (!block->next) {
      break;
    }

    block = block->next;
  }

  return not_found_block(block);
}

/*  Попробовать выделить память в куче начиная с блока `block` не пытаясь расширить кучу
 Можно переиспользовать как только кучу расширили. */
static struct block_search_result try_memalloc_existing ( size_t query, struct block_header* block ) {

    query = size_max(query, BLOCK_MIN_CAPACITY);

    struct block_search_result block_search_result = find_good_or_last(block, query);

    if (block_search_result.type == BSR_FOUND_GOOD_BLOCK) {

        split_if_too_big(block_search_result.block, query);
        block_search_result.block->is_free = false;

    }

    return block_search_result;
}



static struct block_header* grow_heap( struct block_header* restrict last, size_t query ) {
  
  if (!last) {
    return NULL;
  }

  struct region new_region = alloc_region(block_after(last), size_max(query, BLOCK_MIN_CAPACITY));

  if (region_is_invalid(&new_region)) {
    return NULL;
  }

  block_size size = (block_size) {.bytes = new_region.size};
  block_init(new_region.addr, size, NULL);


  last->next = new_region.addr;

  if (last->is_free && try_merge_with_next(last)) {
    return last;
  }

  return last->next;
}

/*  Реализует основную логику malloc и возвращает заголовок выделенного блока */
static struct block_header* memalloc( size_t query, struct block_header* heap_start) {

    query = size_max(query, BLOCK_MIN_CAPACITY);

  struct block_search_result block_result = try_memalloc_existing(query, heap_start);

  if (block_result.type == BSR_FOUND_GOOD_BLOCK) {
    return block_result.block;
  }
  
  if (block_result.type == BSR_REACHED_END_NOT_FOUND) {
    heap_start = grow_heap(block_result.block, query);
    block_result = try_memalloc_existing(query, heap_start);
    return block_result.block;
  } 

  return NULL;

}

void* _malloc( size_t query ) {
 struct block_header* const addr = memalloc( query, (struct block_header*) HEAP_START );
  if (addr) return addr->contents;
  else return NULL;

}

static struct block_header* block_get_header(void* contents) {
  return (struct block_header*) (((uint8_t*)contents)-offsetof(struct block_header, contents));
}

void _free( void* mem ) {
  if (!mem) return ;
  struct block_header* header = block_get_header( mem );
  header->is_free = true;
  
  while(try_merge_with_next(header));
}
