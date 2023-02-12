#include "./tests.h"


#define HEAP_SIZE 10000
#define BLOCK_SIZE 1000



static void free_heap(void *heap) {
    munmap(heap, size_from_capacity((block_capacity){.bytes = HEAP_SIZE}).bytes);
}


//simple memory allocation
bool test1() {
    printf("Test 1: simple allocation\n");

    void *heap = heap_init(HEAP_SIZE);

    void *block = _malloc(BLOCK_SIZE);

    debug_heap(stdout, heap);

    if (!block) {
        fprintf(stderr, "Test 1: simple allocation. FAILED\n");
        return false;
    };

    _free(block);

    debug_heap(stdout, heap);

    free_heap(heap);

    printf("Test1: simple allocation. PASSED\n");
    return true;

}

// free one block
bool test2() {
    printf("Test 2: free one block\n");

    void *heap = heap_init(HEAP_SIZE);

    void *block1 = _malloc(BLOCK_SIZE);
    void *block2 = _malloc(BLOCK_SIZE);
    void *block3 = _malloc(BLOCK_SIZE);
    void *block4 = _malloc(BLOCK_SIZE);
    void *block5 = _malloc(BLOCK_SIZE);

    debug_heap(stdout, heap);

    _free(block3);

    if (!block1 || !block2 || !block4 || !block5) {
        _free(block1);
        _free(block2);
        _free(block4);
        _free(block5);
        free_heap(heap);
        fprintf(stderr, "Test 2: free one block. FAILED\n");
        return false;
    }
    debug_heap(stdout, heap);

    _free(block1);
    _free(block2);
    _free(block4);
    _free(block5);

    free_heap(heap);

    printf("Test 2: free one block. PASSED\n");
    return true;

}

//free two blocks
bool test3() {
    printf("Test 3: free two blocks\n");

    void *heap = heap_init(HEAP_SIZE);

    void *block1 = _malloc(BLOCK_SIZE);
    void *block2 = _malloc(BLOCK_SIZE);
    void *block3 = _malloc(BLOCK_SIZE);
    void *block4 = _malloc(BLOCK_SIZE);
    void *block5 = _malloc(BLOCK_SIZE);
    void *block6 = _malloc(BLOCK_SIZE);
    void *block7 = _malloc(BLOCK_SIZE);

    debug_heap(stdout, heap);

    _free(block3);
    _free(block4);

    if (!block1 || !block2 || !block5 || !block6 || !block7) {
        _free(block1);
        _free(block2);
        _free(block5);
        _free(block6);
        _free(block7);
        free_heap(heap);
        fprintf(stderr, "Test 3: free two block. FAILED\n");
        return false;
    }
    debug_heap(stdout, heap);

    _free(block1);
    _free(block2);
    _free(block5);
    _free(block6);
    _free(block7);

    printf("Test 3: free two block. PASSED\n");
    return true;
}

// extend heap
bool test4() {
    printf("Test 4: extend heap\n");

    void *heap = heap_init(HEAP_SIZE);

    void *block1 = _malloc(BLOCK_SIZE);
    void *block2 = _malloc(BLOCK_SIZE);
    void *block3 = _malloc(BLOCK_SIZE);
    void *block4 = _malloc(BLOCK_SIZE);
    void *block5 = _malloc(BLOCK_SIZE);
    void *block6 = _malloc(BLOCK_SIZE);
    void *block7 = _malloc(BLOCK_SIZE);
    void *block8 = _malloc(BLOCK_SIZE);
    void *block9 = _malloc(BLOCK_SIZE);
    void *block10 = _malloc(BLOCK_SIZE);

    debug_heap(stdout, heap);

    void *block11 = _malloc(BLOCK_SIZE);



    if (!block11) {
        _free(block1);
        _free(block2);
        _free(block3);
        _free(block4);
        _free(block5);
        _free(block6);
        _free(block7);
        _free(block8);
        _free(block9);
        _free(block10);
        _free(block11);
        free_heap(heap);
        fprintf(stderr, "Test 4: extend heap. FAILED\n");
        return false;
    }

    struct block_header* heap_extended = (struct block_header*) heap;
    size_t heap_extended_size = size_from_capacity(heap_extended->capacity).bytes;

    if (heap_extended_size <= HEAP_SIZE) {
        _free(block1);
        _free(block2);
        _free(block3);
        _free(block4);
        _free(block5);
        _free(block6);
        _free(block7);
        _free(block8);
        _free(block9);
        _free(block10);
        _free(block11);
        free_heap(heap);
        fprintf(stderr, "Test 4: extend heap. FAILED\n");
        return false;
    }

    debug_heap(stdout, heap);

    _free(block1);
    _free(block2);
    _free(block3);
    _free(block4);
    _free(block5);
    _free(block6);
    _free(block7);
    _free(block8);
    _free(block9);
    _free(block10);
    _free(block11);
    free_heap(heap);
    free_heap(heap_extended);
    printf("Test 4: extend heap. PASSED");
    return true;
}

//allocate in another place
bool test5() {
    printf("Test 5: allocate in another place\n");

    void *heap = heap_init(HEAP_SIZE);

    void *block1 = _malloc(BLOCK_SIZE);
    void *block2 = _malloc(BLOCK_SIZE);
    void *block3 = _malloc(BLOCK_SIZE);
    void *block4 = _malloc(BLOCK_SIZE);
    void *block5 = _malloc(BLOCK_SIZE);

    debug_heap(stdout, heap);

    _free(block4);

    void *block6 = _malloc(BLOCK_SIZE);

    struct block_header* heap_extended = (struct block_header*) heap;
    size_t heap_extended_size = size_from_capacity(heap_extended->capacity).bytes;

    if (heap_extended_size > HEAP_SIZE) {
        fprintf(stderr, "Test 5: allocate in another place. FAILED\n");
        _free(block1);
        _free(block2);
        _free(block3);
        _free(block5);
        _free(block6);
        free_heap(heap);
        return false;
    }

    debug_heap(stdout, heap);

    _free(block1);
    _free(block2);
    _free(block3);
    _free(block5);
    _free(block6);
    free_heap(heap);
    printf("Test 5: allocate in another place. PASSED\n");
    return true;
}
