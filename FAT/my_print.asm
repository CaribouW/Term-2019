        global  mPrint
        section .text
mPrint:
        mov rdx, rsi         ;length
        mov rsi, rdi        ;string input
        mov rax, 0x2000004
        mov rdi, 1
        syscall
        ret


