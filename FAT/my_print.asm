        global  print
        extern  printf

        section .text
print:
        ; 我们需要调用 printf 函数, 但是我们也同时在使用 rax,rbx 和 rcx 这三个寄存器。
        ; 调用 printf 函数会破坏 rax 和 rcx 这两个寄存器的值, 所以我们要在调用前保存
        ; 并且在调用后恢复这两个寄存器中的数据。
;rdi，rsi，rdx，rcx，r8和r9
        push    rax                     ; 调用者保存寄存器
        push    rcx                     ; 调用者保存寄存器
        push    rdi
        push    rsi
        push    rdx
        push    rcx
        push    r8
        push    r10
        push    r9
        mov     rax, rdi
        call    printf

        pop     r9
        pop     r10
        pop     r8
        pop     rcx
        pop     rdx
        pop     rsi
        pop     rdi
        pop     rcx
        pop     rax                     ; 返回前恢复 rbx 的值
        ret
