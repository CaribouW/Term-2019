
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define GLOBAL_VARIABLES_HERE

#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "proc.h"
#include "global.h"
#include "proto.h"

PUBLIC PROCESS proc_table[NR_TASKS + NR_PROCS];

PUBLIC TASK task_table[NR_TASKS] = {
	{task_tty, STACK_SIZE_TTY, "tty"},
	{F, STACK_SIZE_F, "F"},

};

PUBLIC TASK user_proc_table[NR_PROCS] = {
	{ReaderA, STACK_SIZE_READER_A, "ReaderA"},
	{ReaderB, STACK_SIZE_READER_B, "ReaderB"},
	{ReaderC, STACK_SIZE_READER_C, "ReaderC"},
	{WriterD, STACK_SIZE_Writer_D, "WriterD"},
	{WriterE, STACK_SIZE_Writer_E, "WriterE"},
};

PUBLIC char task_stack[STACK_SIZE_TOTAL];

PUBLIC TTY tty_table[NR_CONSOLES];
PUBLIC CONSOLE console_table[NR_CONSOLES];

PUBLIC irq_handler irq_table[NR_IRQ];

PUBLIC system_call sys_call_table[NR_SYS_CALL] = {sys_get_ticks, sys_process_sleep, sys_disp_str};

PUBLIC char colors[NR_COLOR] = {WHITE_COLOR, RED_COLOR, GREEN_COLOR, BLUE_COLOR, 0x03, 0x04, 0x06};
