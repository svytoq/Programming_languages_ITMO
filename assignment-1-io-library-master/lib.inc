%define EXIT 60
%define READ 0
%define WRITE 1


section .text
 
 
; Принимает код возврата и завершает текущий процесс
exit: 
    mov rax, EXIT
    syscall

; Принимает указатель на нуль-терминированную строку, возвращает её длину
;rdi - принимает строку
string_length:
    xor rax, rax
.loop:
    cmp byte [rdi+rax], 0
    je .end
    inc rax
    jmp .loop
.end:
    ret

; Принимает указатель на нуль-терминированную строку, выводит её в stdout

print_string:
    push rdi    
    call string_length
    pop rsi
    mov rdx, rax
    mov rax, WRITE
    mov rdi, 1
    syscall
    ret

; Принимает код символа и выводит его в stdout
;rdi - принимает код символа
print_char:
    push rdi
    mov rsi, rsp
    mov rdx, 1
    mov rax, WRITE
    mov rdi, 1
    syscall
    pop rdi
    ret

; Переводит строку (выводит символ с кодом 0xA)
print_newline:
    mov rdi, 0xA
    jmp print_char

; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
; rdi - принимает число
print_uint:
    mov rax, rdi
    mov r8, 10
    push 0x00
    .loop:
        xor rdx, rdx
        div r8
        add rdx, 0x30
        push rdx
        cmp rax, r8
        jae .loop
    add rax, 0x30
    cmp rax, 0x30
    je .circle
    push rax
    .circle:
        pop rdi
        cmp rdi, 0x00
        je .eof
        call print_char
        jmp .circle
    .eof:
        ret

; Выводит знаковое 8-байтовое число в десятичном формате
; rdi - принимает число 
print_int:
    test rdi, rdi
    jns .call
    push rdi
    mov rdi, '-'
    call print_char
    pop rdi
    neg rdi
    .call:
        jmp print_uint

; Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
; rdi - первый указатель
; rsi - второй указатель
string_equals:
    xor rcx, rcx
    .loop:
        mov dl, byte[rdi+rcx]
        mov al, byte[rsi+rcx]
        inc rcx
        cmp dl, al
        jne .negative
        cmp al, 0x00
        je .positive        
        jmp .loop
    .negative:
        xor rax, rax
        ret
    .positive:
        mov rax, 1
        ret

; Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
read_char:
    push 0
    mov rax, READ
    mov rdi, 0
    mov rsi, rsp
    mov rdx, 1
    syscall
    pop rax
    ret 

; Принимает: адрес начала буфера, размер буфера
; Читает в буфер слово из stdin, пропуская пробельные символы в начале, .
; Пробельные символы это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
; Останавливается и возвращает 0 если слово слишком большое для буфера
; При успехе возвращает адрес буфера в rax, длину слова в rdx.
; При неудаче возвращает 0 в rax
; Эта функция должна дописывать к слову нуль-терминатор
; rdi - адресс начала буфера
; rsi - размер буфера
read_word:
    xor rcx, rcx
    .loop:
        push rdi
        push rsi
        push rcx
        call read_char
        pop rcx
        pop rsi
        pop rdi
        cmp rax, 0
        je .sukces
        cmp rcx, 0
        jne .one_letter_more
        cmp rax, 0x20
        je .skip
        cmp rax, 0x9
        je .skip
        cmp rax, 0xA
        je .skip
        .one_letter_more:
            cmp rax, 0x20
            je .sukces
            cmp rax, 0x9
            je .sukces
            cmp rax, 0xA
            je .sukces
            mov [rdi+rcx], rax
            inc rcx
    .skip:
        cmp rcx, rsi
        je .fail
        jmp .loop
    .fail:
        xor rax, rax
        ret
    .sukces:
        mov byte[rdi+rcx], 0
        mov rax, rdi
        mov rdx, rcx
        ret
 

; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
parse_uint:
    mov r10, 10
    xor rcx, rcx
    xor rax, rax
    xor r8, r8
    .loop:
        mov r8b, byte[rdi+rcx]
        cmp r8, 0x30
        jb .end
        cmp r8, 0x39
        ja .end
        sub r8, 0x30
        mul r10
        add rax, r8
        inc rcx
        jmp .loop
    .end:
        mov rdx, rcx
    ret




; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax: число, rdx : его длину в символах (включая знак, если он был) 
; rdx = 0 если число прочитать не удалось
parse_int:
    cmp byte[rdi], '-'
    jne parse_uint
    inc rdi
    call parse_uint
    neg rax
    inc rdx
    ret 

; Принимает указатель на строку, указатель на буфер и длину буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
; rdi - указатель на строку
; rsi - указатель на буфер
; rdx - длина буфера
string_copy:
    push rdi
    push rdx
    call string_length
    pop rdx
    pop rdi
    cmp rdx, rax
    jle .fail
    xor rcx, rcx
    xor r8, r8
    inc rax
    .loop:
        mov r8b, byte[rdi+rcx]
        mov byte[rsi+rcx], r8b
        inc rcx
        cmp rax, rcx
        je .sukces
        jmp .loop
    .fail:
        xor rax, rax      
        ret
    .sukces:
        dec rax
        mov rcx, rax
        ret
