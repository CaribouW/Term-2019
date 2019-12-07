
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                               syscall.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                                                     Forrest Yu, 2005
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

%include "sconst.inc"

INT_VECTOR_SYS_CALL equ 0x90
_NR_get_ticks       equ 0
_NR_write	    equ 1
_NR_system_process_sleep	equ 2 ;
_NR_system_P 		equ 3 ;
_NR_system_V		equ 4 ;
; 导出符号
global	get_ticks
global	write
global	system_process_sleep
global	system_P
global  system_V
bits 32
[section .text]

; ====================================================================
;                              get_ticks
; ====================================================================
get_ticks:
	mov	eax, _NR_get_ticks
	int	INT_VECTOR_SYS_CALL
	ret

; ====================================================================================
;                          void write(char* buf, int len);
; ====================================================================================
write:
        mov     eax, _NR_write
        mov     ebx, [esp + 4]
        mov     ecx, [esp + 8]
        int     INT_VECTOR_SYS_CALL
        ret
; ====================================================================
;                              system_process_sleep
; ====================================================================
system_process_sleep:
	push ebx
	mov ebx, [esp + 8]
	mov	eax, _NR_system_process_sleep
	int	INT_VECTOR_SYS_CALL
	pop ebx
	ret

	; ====================================================================
;                              system_P
; ====================================================================
system_P:
	push ebx
	mov ebx, [esp + 8]
	mov	eax, _NR_system_P
	int	INT_VECTOR_SYS_CALL
	pop ebx
	ret

; ====================================================================
;                              system_V
; ====================================================================
system_V:
	push ebx
	mov ebx, [esp + 8]
	mov	eax, _NR_system_V
	int	INT_VECTOR_SYS_CALL
	pop ebx
	ret