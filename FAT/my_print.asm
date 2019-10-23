global add

section .data
section .text
add:
    mov   rax, rsi
    add   rax, rdi
    ret