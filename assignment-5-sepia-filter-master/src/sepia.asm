;; Аргумент 1 - номер группы пикселей. Всего три группы будут
%macro do_group 1
    pxor xmm0, xmm0
    pxor xmm1, xmm1
    pxor xmm2, xmm2

    ;; Достаём из памяти
    pinsrb xmm0, [rdi+%1*3+0], 0
    pinsrb xmm1, [rdi+%1*3+1], 0
    pinsrb xmm2, [rdi+%1*3+2], 0
    pinsrb xmm0, [rdi+%1*3+3], 4
    pinsrb xmm1, [rdi+%1*3+4], 4
    pinsrb xmm2, [rdi+%1*3+5], 4

    ;; Перевести всё в float
    cvtdq2ps xmm0, xmm0
    cvtdq2ps xmm1, xmm1
    cvtdq2ps xmm2, xmm2

    %if %1 = 0
        ;; p1, p1, p1, p2
        %define pattern 0b01000000
    %elif %1 = 1
        ;; p2, p2, p3, p3
        %define pattern 0b01010000
    %elif %1 = 2
        ;; p3, p4, p4, p4
        %define pattern 0b01010100
    %endif

    shufps xmm0, xmm0, pattern
    shufps xmm1, xmm1, pattern
    shufps xmm2, xmm2, pattern

    ;; Делаем матричные умножения
    mulps xmm0, xmm3
    mulps xmm1, xmm4
    mulps xmm2, xmm5

    addps xmm0, xmm1
    addps xmm0, xmm2

    cvtps2dq xmm0, xmm0

    ;; Максимальное значение - 255
    pminud xmm0, [max]

    ;; Вытаскиваем результат из xmm регистров
    pextrb [rsi+%1*4+0], xmm0, 0
    pextrb [rsi+%1*4+1], xmm0, 4
    pextrb [rsi+%1*4+2], xmm0, 8
    pextrb [rsi+%1*4+3], xmm0, 12

    ;; Волшебным числом прокручиваем матрицу
    shufps xmm3, xmm3, 0b01001001
    shufps xmm4, xmm4, 0b01001001
    shufps xmm5, xmm5, 0b01001001    
%endmacro

section .rodata
    align 16
    m_row_1: dd 0.272, 0.349, 0.393, 0.272
    m_row_2: dd 0.534, 0.686, 0.769, 0.534
    m_row_3: dd 0.131, 0.168, 0.189, 0.131
    max: dd 0xFF, 0xFF, 0xFF, 0xFF

section .text
global sepia_asm_helper
sepia_asm_helper:
    push rbp

    ;; Подгружаем матрицу, которую будет
    ;; потом крутить для следующий групп
    ;; Группа 0: b, g, r, b
    ;; Группа 1: g, r, b, g
    ;; Группа 2: r, b, g, r
    movaps xmm3, [m_row_1]
    movaps xmm4, [m_row_2]
    movaps xmm5, [m_row_3]

    do_group 0
    do_group 1
    do_group 2

    pop rbp
    ret