%include "lib.inc"
%include "words.inc"
%include "colon.inc"
global _start

%define BUFF_SIZE 256
%define PTR_SIZE 8

extern find_word

section .rodata
	msg_to_long: db "String is too long", 0xA 0
	msg_not_found: db "Key not found", 0xA, 0
	
section .bss
input_buffer: resb BUFF_SIZE

section .text
_start:
    mov rdi, input_buffer
    mov rsi, BUFF_SIZE
    call read_word
    cmp rax, 0
    jz .err_to_long


    mov rdi, input_buffer
    mov rsi, NEXT_KEY
    call find_word
    cmp rax, 0
    jz .err_not_found

    mov rdi, rax
    add rdi, 8
    push rdi
    call string_length
    pop rdi
    add rax, rdi
    inc rax
    mov rdi, rax
    call print_string
    xor rdi, rdi
    call exit

    .err_to_long:
        mov rdi, msg_to_long
        call print_err
        mov rdi, 1
        call exit
    .err_not_found:
        mov rdi, msg_not_found
        call print_err
        mov rdi, 1
        call exit
