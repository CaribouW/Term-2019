
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"

/*======================================================================*
                              schedule
 *======================================================================*/
PUBLIC void schedule()
{
	PROCESS *p;
	int greatest_ticks = 0;

	while (!greatest_ticks)
	{
		for (p = proc_table; p < proc_table + NR_TASKS + NR_PROCS; p++)
		{
			if (p->ticks > greatest_ticks)
			{
				greatest_ticks = p->ticks;
				p_proc_ready = p;
			}
		}

		if (!greatest_ticks)
		{
			for (p = proc_table; p < proc_table + NR_TASKS + NR_PROCS; p++)
			{
				p->ticks = p->priority;
			}
		}
	}
}

/*======================================================================*
                           sys_get_ticks
 *======================================================================*/
PUBLIC int sys_get_ticks()
{
	return ticks;
}
/*======================================================================*
                           sys_process_sleep
 *======================================================================*/
PUBLIC int sys_process_sleep(int milli_sec)
{
	p_proc_ready->sleep_ticks = milli_sec / 10;
	schedule();
	return 0;
}
/*======================================================================*
                           sys_P
 *======================================================================*/
PUBLIC int sys_P(SEMAPHORE *s)
{
	s->value--;
	if (s->value < 0)
	{
		PROCESS *p;

		//first we insert current process to wait queue
		p_proc_ready->is_wait = 1;
		if (s->queue == 0)
		{
			s->queue = p_proc_ready; //let it to wait
		}
		else
		{
			PROCESS *rear = s->queue;
			while (rear->next != 0)
			{
				rear = rear->next;
			}
			rear->next = p_proc_ready;
		}
		schedule();
	}
	return 0;
}

/*======================================================================*
                           sys_V
 *======================================================================*/
PUBLIC int sys_V(SEMAPHORE *s)
{
	s->value++;
	if (s->value <= 0)
	{
		//wake up process
		p_proc_ready = s->queue;
		s->queue = s->queue->next;
		p_proc_ready->next = 0;
		p_proc_ready->is_wait = 0;
	}
	return 0;
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
	char *p;
	char tmp[256];
	va_list p_next_arg = args;

	for (p = buf; *fmt; fmt++)
	{
		if (*fmt != '%')
		{
			*p++ = *fmt;
			continue;
		}

		fmt++;

		switch (*fmt)
		{
		case 'x':
			itoa(tmp, *((int *)p_next_arg));
			strcpy(p, tmp);
			p_next_arg += 4;
			p += strlen(tmp);
			break;
		case 's':
			break;
		default:
			break;
		}
	}

	return (p - buf);
}

PUBLIC int printf(const char *fmt, ...)
{
	int i;
	char buf[256];

	va_list arg = (va_list)((char *)(&fmt) + 4); /*4是参数fmt所占堆栈中的大小*/
	i = vsprintf(buf, fmt, arg);
	write(buf, i);

	return i;
}