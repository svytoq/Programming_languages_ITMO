%include "lib.inc"

global find_word

find_word:
    .loop:
        push rdi
        push rsi
        add rsi, 8
        call string_equals
        pop rsi
        pop rdi
        cmp rax, 1
        je .found
        mov rsi, [rsi]
        cmp rsi, 0
        je .fault
        jmp .loop
    .found:
        mov rax, rsi
        ret
    .fault:
        xor rax, rax
        ret
