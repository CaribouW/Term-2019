            section .data
new_line            dw      0AH
blank               dw      ' '
neg_symbol          dw      '-'
opr_size            dw      10      ;操作数长度
opr_1_sign          dw      0       ;第一个操作数符号 0 为正
opr_2_sign          dw      0       ;第二个操作数符号
result_sign         dw      0       ; 结果的符号s （减法时最高位产生借位, 或者乘法符号不同就是负号）
opr_1_size          dw      0       ;第一个操作数的长度
opr_2_size          dw      0       ;第二个操作数的长度
add_data_size       dw      20
sub_data_size       dw      20
mul_data_size       dw      40

; debug
input_prompt        dw      "Please input x and y:",10
input_prompt_len    equ     $ - input_prompt
reach_blank         dw      "You have reach the blank!", 10
reach_blank_len     equ     $ - reach_blank
reach_new_line      dw      "You have reach new line!", 10
reach_new_line_len  equ     $ - reach_new_line
err_txt             dw      "Error!",10
err_txt_len         equ     $ - err_txt

global _main
extern println
default rel
            section .bss
in_char:			resw 20      ; For input char
counter             resw 1
opr_1               resw 20      ; 第一个操作数, 指针小者为低位
opr_2               resw 20      ; 第二个操作数, 指针小者为低位
opr_sub_result      resw 20      ; 差
opr_mul_result      resw 40      ; 乘积
opr_sum_result      resw 40      ; 和

            section .text

_main:
    xor r8, r8                  ; 输入长度计数器
    mov rax, 0x2000004
    mov rdi, 1
    mov rsi, input_prompt
    mov rdx, input_prompt_len
    syscall
    jmp _read_console

; 读取console输入  N1 N2
; 将输入全部压入stack
_read_console:
    mov rax, 0x2000003
    mov rdi, 0
    mov rsi, in_char            ; store the input in the in_char
    mov rdx, 1
    syscall

    mov r10, [rsi]              ; 取值, 放到r10               
    push r10                    ; push result
    inc r8                      ; 计数器加一
    
    cmp byte [rsi], 0AH
    jne _read_console           ; 循环
    jmp _input_analyse          

; 把计数器包含的字符串全部取出
_input_analyse:
    xor rax, rax
    pop r10                     ; 弹出\n
    dec r8
    mov rcx, r8                 ; rcx 作为暂存的总大小
    
    xor r10, r10
    mov r11, 0x1
    xor rdx, rdx                ; rdx作为暂时的计数器

    jmp pop_opr_2

    ; 把操作数逐位进行写入, 从低位到高位写入
    pop_opr_2:
        pop r10                     ; r10 作为获取单个值
        ;进行下一步的整数获取
        cmp r10, 0x2D               ;如果是 '-' 
        je opr_2_neg
        jmp opr_2_blank
        
        opr_2_neg:
            mov word[opr_2_sign], 1 ; 符号位置 1
            mov word[opr_2_size], dx    ; 此时已经知道第二个操作数长度
            dec r8                      ; 来到 下一个
            cmp r8, 0
            jne pop_opr_2
            ; 异常,没有出现空格
            call print_err
            jmp _sys_exit
        
        opr_2_blank:
            cmp r10, 32                 ; 是否是空格
            ; 如果不是空格
            jne push_opr_2
            ;判定为空格
            dec r8
            ;此时 rcx存放总长度(包含空格), rdx作为第二个操作数长度
            ; 那么第一个操作数长度 rcx - rdx -1 = r8
            ; test
            mov rax, r8
            mov r12, rdx

            mov word[opr_1_size], ax       ; 此处可能会多算入一个减号
            mov word[opr_2_size], dx

            xor rdx, rdx

            jmp pop_opr_1
        
        ; 存储第二个操作数
        push_opr_2:
            sub r10, 0x30               ; 获取数字的真值
            lea rsi, [opr_2]
            and r10, 0xFFFF
            mov [rsi + 2*rdx], r10
            inc rdx                     ; 长度递增
            dec r8                      ; 计数器递减
            cmp r8, 0
            jne pop_opr_2
            ; 异常,没有出现空格
            call print_err
            jmp _sys_exit
        
        ;遇到空格
        pop_opr_1:
            pop r10
            cmp r10, 0x2D               ; 如果是 '-'
            je opr_1_neg

            sub r10, 0x30               ; 获取数字的真值
            lea rsi, [opr_1]
            and r10, 0xFFFF
            mov [rsi + 2*rdx],r10
            inc rdx
        opr_1_ret:
            dec r8
            cmp r8, 0
            jne pop_opr_1
            ;解析完毕
            mov ax, word[opr_1_size]
            mov bx, word[opr_1_sign]
            sub rax, rbx
            mov word[opr_1_size],ax     ; 如果bx == 1 , 那么就让opr_1 长度减一
            lea rax, [opr_1]
            lea rbx, [opr_2]
            jmp push_opr_1
        
        opr_1_neg:
            mov word[opr_1_sign], 1
            jmp opr_1_ret

        push_opr_1:
            jmp compute_result

; 计算结果, 此处是输出ASCII字符
compute_result:
    push rax                        ; 临时寄存
    push rdx                        ; 作为计数器
    push rcx                        ; 作为阈值

    mov dx, word[opr_1_sign]
    mov ax, word[opr_2_sign]
    xor rdx, rax                  ; 符号异或. 同号做加, 异号做减

    mov word[result_sign], dx
    cmp rdx, 1
    je opr_sub
    add rdx, rax                    ; 确定加法的结果符号
    jmp opr_add

; 减法计算
; 此时无法确定符号位, 需要考察最高位是否存在借位来判定
opr_sub:
    xor rax, rax                    ;寄存器清零
    xor rdx, rdx                    ;计数器清零
    xor rcx, rcx                ; 借位标志
    xor rbx, rbx
    ; 减法逐位相减, 一直到最后一位
    jmp sub_loop
    
    sub_loop: 
        xor rax, rax
        xor rbx, rbx
        lea rsi, [opr_1]
        mov ax, word[rsi + 2*rdx]       ; opr_1[rdx] - > r10
        lea rsi, [opr_2]
        mov bx, word[rsi + 2*rdx]      ; opr_2[rdx] - > r11
        sub ax, cx                  ; 借位
        sub ax, bx                ; ’单‘位减法, 注意这时需要截断
        ; and r10, 0xFFFF
        cmp ax, 0                 ; 相减, 结果如果大于等于10, 表示是一个负数,不够减
        jge normal_sub
        jmp carry_sub

        carry_sub:
            ;不够减
            ; neg ax                     ; 取反
            add ax, 10
            ; mov bx, 10
            ; xor rbx, rbx
            ; sub bx, ax
            ; mov ax, bx
            mov rcx, 1                  ; 设置借位
            lea rsi, [opr_sub_result]
            mov word[rsi + 2 * rdx], ax  ; rdx 给原有数
            inc rdx
            cmp dx, word [sub_data_size]        ; 一直加到20位
            jl sub_loop                 ; if less than 20, then go on loop
            jmp sub_post_handle
        
        normal_sub:
            mov rcx, 0
            lea rsi, [opr_sub_result]
            mov word[rsi + 2 * rdx], ax  ; rax 给原有数

            inc rdx
            cmp dx, word [sub_data_size]            ; 一直加到20位
            jl sub_loop                 ; if less than 20, then go on loop
            jmp sub_post_handle

sub_post_handle:                        ; 后处理
    xor rdx, rdx
    xor rax, rax
    mov rdx, 19
    lea rsi, [opr_sub_result]
    
    mov cx, word[rsi + 2*rdx]
    and rcx, 0xFFFF
    dec rdx
    cmp rcx, 9                  ; 如果最高位是9,表示不够减
    jne set_sub_sign            ; 如果不是9, 表示可以直接输出

    ;需要确定结果的符号
    mov ax, word[opr_1_sign]    ; 得到第一个操作数符号
    ; 结果的符号和ax相反
    cmp ax, 1
    je set_sign_0
    jmp set_sign_1

    set_sub_sign:
        mov ax, word[opr_1_sign]
        mov word[result_sign],ax
        jmp print_sub_result

    set_sign_0:
        mov ax, 0
        mov word[result_sign],  ax
        jmp sub_exchange
    set_sign_1: 
        mov ax, 1
        mov word[result_sign], ax
        jmp sub_exchange

    sub_exchange:                   ; 前操作数不够减,绝对值小于第二个操作数绝对值, 采取把两个操作数交换位置
        mov ax, word[opr_1_size]
        mov bx, word[opr_2_size]
        mov word[opr_1_size], bx
        mov word[opr_2_size], ax
        mov ax, word[opr_1_sign]
        mov bx, word[opr_2_sign]
        mov word[opr_1_sign], bx
        mov word[opr_2_sign], ax
        
        xor rdx, rdx
        jmp sub_exchange_loop
        sub_exchange_loop:
            lea rsi, [opr_1]
            mov ax, word[rsi + 2*rdx]
            lea rsi, [opr_2]
            mov bx, word[rsi + 2*rdx]
            mov word[rsi + 2*rdx], ax
            lea rsi, [opr_1]
            mov word[rsi + 2*rdx], bx
            lea rsi, [opr_sub_result]
            mov word[rsi + 2*rdx], 0
            inc rdx
            cmp dx, word[sub_data_size]
            jl sub_exchange_loop
            jmp opr_sub

print_sub_result:
    
    xor rdx, rdx
    mov dx, word[result_sign]
    mov ax, word[opr_1_sign]
    
    ; 接下来的输出
    cmp rdx, 1
    je print_sub_neg
    jmp print_sub_true_val
    print_sub_neg:
        call print_neg
        jmp print_sub_true_val
    print_sub_true_val:
        mov rdx, 0              ; 指针
        lea rsi, [opr_sub_result]
        mov ax, word[sub_data_size]
        mov word[counter],ax
        call fetch_opr_size
        mov r15, [counter]
        and r15, 0xFFFF
        xor rax, rax
        jmp print_sub_push

    print_sub_push:
        lea rsi, [opr_sub_result]
        mov ax, word [rsi + 2*rdx]
        add rax, 30H
        push rax
        inc rdx
        cmp rdx, r15
        jl print_sub_push
        jmp print_sub_pop

    ; 此时rdx的值就是里面push的个数
    print_sub_pop:
        pop rax
        mov word[counter],ax
        lea rsi, [counter]
        call print_num

        dec rdx
        cmp rdx, 0
        jne print_sub_pop
        call println
        jmp opr_mul

; 执行加法运算
opr_add:
    xor rax, rax                    ;寄存器清零
    xor rdx, rdx                    ; 计数器
    ; 加法累加, 一直加到最后一位
    jmp add_loop
    
    add_loop: 
        lea rsi, [opr_1]
        mov r10, [rsi + 2*rdx]       ; opr_1[rdx] - > r10
        and r10, 0xFFFF               ; 获取低 16 位
        lea rsi, [opr_2]
        mov r11, [rsi + 2*rdx]      ; opr_2[rdx] - > r11
        and r11, 0xFFFF
        add r10, r11                ; ’单‘位加法
        xor r11, r11                ; 清空r11
        ;存储到 opr_sum_result
        lea rsi, [opr_sum_result]
        and r10, 0xFFFF
        mov [rsi + 2*rdx], r10

        inc rdx
        cmp dx, word[add_data_size]                 ; 一直加到21位
        jl add_loop                 ; if less than 21, then go on loop
        ; 加法运算成功, 接下来进行进位处理
        xor rdx, rdx                ; 清空计数器
        jmp add_carry_loop
    
    add_carry_loop:
        xor rax, rax
        lea rsi, [opr_sum_result]
        mov r10, [rsi + 2*rdx]      ; r10 作为当前位 , r11作为carry
        and r10, 0xFFFF
        add r10, r11                ; 添加前一位的carry
        mov rcx, 10                 ; 除数

        mov r15, rdx                    ;缓存rdx

        jmp resolve_add_carry
        resolve_add_carry:
            cmp r10, 10
            jl add_less_10              ; r10小于10就不运算
            ; put to param
            xor rdx, rdx
            mov rax, r10
            mov rcx, 10        

            div rcx                      ; 除以10  商 rax , 余数 rdx (有个坑 : 余数必须清零)
            lea rsi, [opr_sum_result]
            mov word[rsi + 2 * r15], dx  ; rdx 给原有数
            mov r11, rax                ; 商进位, 余数归还
            mov rdx, r15                ; 返还rdx
            inc rdx                     ; 来到下一位
            cmp dx, word[add_data_size]
            jl add_carry_loop
            jmp print_add_result
        
        add_less_10:
            lea rsi, [opr_sum_result]
            mov rax, r10
            mov word[rsi + 2 * r15], ax
            mov rdx, r15
            inc rdx                     ; 直接递增计数器
            xor r11, r11                ; carry = 0
            xor r10, r10
            cmp dx, word[add_data_size]
            jl add_carry_loop
            jmp print_add_result


print_add_result:
    xor rdx, rdx
    mov dx, word[result_sign]
    ; 接下来的输出
    cmp rdx, 1
    je print_add_neg
    jmp print_add_true_val
    print_add_neg:
        call print_neg
        jmp print_add_true_val
    print_add_true_val:
        mov rdx, 0              ; 指针
        lea rsi, [opr_sum_result]
        mov ax, word[add_data_size]
        mov word[counter],ax
        call fetch_opr_size
        mov r15, [counter]
        and r15, 0xFFFF
        xor rax, rax
        jmp print_add_push

    print_add_push:
        lea rsi, [opr_sum_result]
        mov ax, word [rsi + 2*rdx]
        add rax, 30H
        push rax
        inc rdx
        cmp rdx, r15
        jl print_add_push
        jmp print_add_pop

    ; 此时rdx的值就是里面push的个数
    print_add_pop:
        pop rax
        mov word[counter],ax
        lea rsi, [counter]
        call print_num

        dec rdx
        cmp rdx, 0
        jne print_add_pop
        call println
        jmp opr_mul


; 执行乘法运算
; 使用两个指针r8 r9进行运算
opr_mul:
    mov bx, word [opr_1_size]       ; 第一个操作数长度
    mov cx, word [opr_2_size]       ; 第二个操作数长度
    xor r8, r8                      ; 第一个操作数指针  
    xor r9, r9                      ; 第二个操作数指针
            
    jmp opr_mul_loop_1
    opr_mul_loop_1:
        xor r9, r9                  ; 第二个操作数从头开始
        mov bx, word [opr_1_size]
        cmp r8, rbx
        
        jl opr_mul_loop_2          ; if r8 < opr_1_size
        xor r11, r11                ; 清空r11
        jmp mul_carry_loop         ; 处理进位
        opr_mul_loop_2:
            mov rax, r8
            lea rsi, [opr_1]
            mov r10, [rsi + 2 * rax]    ; 获取第一个操作数
            and r10, 0xFFFF

            mov rax, r9
            lea rsi, [opr_2]
            mov r11, [rsi + 2 * rax]    ; 获取第二个操作数
            and r11, 0xFFFF

            mov rax, r10
            imul r10, r11               ; r10 = x*y
            
            mov rax, r8
            add rax, r9                ; rax 作为指针 = r8 + r9

            lea rsi, [opr_mul_result]
            mov r12, [rsi + 2*rax]      ; r12 = 获取第 i + j 个数
            and r12, 0xFFFF

            add r10, r12                ;  追加到r10
            mov rbx, r10
            mov word [rsi + 2*rax], bx  ;追加

            inc r9                        
            mov cx, word [opr_2_size]
            cmp r9, rcx
            jl opr_mul_loop_2           ; if r9 < opr_2_size
            inc r8
            jmp opr_mul_loop_1          ; break

    mul_carry_loop:
        xor rax, rax
        lea rsi, [opr_mul_result]
        mov r10, [rsi + 2*rdx]      ; r10 作为当前位 , r11作为carry
        and r10, 0xFFFF
        add r10, r11                ; 添加前一位的carry
        mov rcx, 10                 ; 除数

        mov r15, rdx                    ;缓存rdx

        jmp resolve_mul_carry
        resolve_mul_carry:
            cmp r10, 10
            jl mul_less_10              ; r10小于10就不运算
            ; put to param
            xor rdx, rdx
            mov rax, r10
            mov rcx, 10        

            div rcx                      ; 除以10  商 rax , 余数 rdx (有个坑 : 余数必须清零)
            lea rsi, [opr_mul_result]
            mov word[rsi + 2 * r15], dx  ; rdx 给原有数
            mov r11, rax                ; 商进位, 余数归还
            mov rdx, r15                ; 返还rdx
            inc rdx                     ; 来到下一位
            cmp dx, word[mul_data_size]
            jl mul_carry_loop
            jmp _sys_exit
        
        mul_less_10:
            lea rsi, [opr_mul_result]
            mov rax, r10
            mov word[rsi + 2 * r15], ax
            mov rdx, r15
            inc rdx                     ; 直接递增计数器
            xor r11, r11                ; carry = 0
            xor r10, r10
            cmp dx, word[mul_data_size]
            jl mul_carry_loop
            xor rdx, rdx
            xor rax, rax
            mov dx, word[opr_1_sign]
            mov ax, word[opr_2_sign]
            xor rdx, rax                  ; 符号异或
            mov word[result_sign], dx
            jmp print_mul_result

print_mul_result:
    ; 首先输出符号位
    xor rdx, rdx
    mov dx, word[result_sign]
    ; 接下来的输出
    cmp rdx, 1
    je print_mul_neg

    jmp print_mul_true_val
    print_mul_neg:
        call print_neg
        jmp print_mul_true_val
    print_mul_true_val:
        mov rdx, 0              ; 指针
        lea rsi, [opr_mul_result]
        mov ax, word[mul_data_size]
        mov word[counter],ax
        call fetch_opr_size
        mov r15, [counter]
        and r15, 0xFFFF
        xor rax, rax
        jmp print_mul_push

    print_mul_push:
        lea rsi, [opr_mul_result]
        mov ax, word [rsi + 2*rdx]
        add rax, 30H
        push rax
        inc rdx
        cmp rdx, r15
        jl print_mul_push
        
        jmp print_mul_pop

    ; 此时rdx的值就是里面push的个数
    print_mul_pop:
        pop rax
        mov word[rel counter],ax
        lea rsi, [rel counter]
        call print_num

        dec rdx
        cmp rdx, 0
        jne print_mul_pop
        call println
        jmp _sys_exit

_sys_exit:
    mov rax, 0x02000001         ; exit系统调用
    xor rdi, rdi                ; exit code 0
    syscall
    ret

; 获取最终结果的大小,参数是counter, 结果放在counter
fetch_opr_size:
    push rbx
    push rcx
    push rsi
    xor rbx, rbx
    xor rcx, rcx
    mov bx, word[rel counter]   ; 获取参数,作为基准长度
    dec rbx                      ; n = N - 1
    jmp fetch_loop
    fetch_loop:
        cmp rbx, 0
        je fetch_ret
        
        mov cx, word[rsi + 2*rbx]    ; 获取数
        and rcx, 0xFFFF
        dec rbx                      ; --n
        cmp rcx, 0                   ; 从后往前比较, 是否为0
        je fetch_loop
        add rbx, 2
        jmp fetch_ret
        
        fetch_ret:
            mov word[counter], bx
            pop rsi
            pop rcx
            pop rbx
            ret
        
; 输出换行
println:
    push rsi
    push rax
    push rdi
    push rdx

    mov rsi, new_line
    mov rax, 0x2000004
    mov rdi, 1
    mov rdx, 1
    syscall

    pop rdx
    pop rdi
    pop rax
    pop rsi
    ret

print_num:
    push rax
    push rdi
    push rdx
    ; lea rsi, [rel counter]
    mov rax, 0x2000004
    mov rdi, 1
    mov rdx, 1
    syscall

    pop rdx
    pop rdi
    pop rax
    ret
print_neg:
    push rax
    push rdi
    push rdx

    lea rsi, [neg_symbol]
    mov rax, 0x2000004
    mov rdi, 1
    mov rdx, 1
    syscall

    pop rdx
    pop rdi
    pop rax
    ret

print_err:
    push rax
    push rdi
    push rdx
    lea rsi, [err_txt]
    mov rax, 0x2000004
    mov rdi, 1
    mov rdx, err_txt_len
    syscall

    pop rdx
    pop rdi
    pop rax
    ret