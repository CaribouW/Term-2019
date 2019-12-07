
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
#define READER_LIMIT 1
#define WRITER
PRIVATE SEMAPHORE wrmutex = {1, 0}, count_mutex = {1, 0}, print_mutex = {1, 0};
PRIVATE int reader_count = 0, total = 0;
PRIVATE char buffer[10];

PRIVATE void countR();
#ifdef WRITER
PRIVATE SEMAPHORE writer_first = {1, 0};
#endif

#ifdef READER_LIMIT
PRIVATE SEMAPHORE reader_mutex = {READER_LIMIT, 0};
#endif
/*======================================================================*
                              schedule
 *======================================================================*/
PUBLIC void schedule()
{
	PROCESS *p;
	int greatest_ticks = 0;

	for (p = proc_table; p < proc_table + NR_TASKS + NR_PROCS; p++)
	{
		if (p->sleep_ticks > 0)
		{
			p->sleep_ticks--;
		}
	}

	while (!greatest_ticks)
	{
		for (p = proc_table; p < proc_table + NR_TASKS + NR_PROCS; p++)
		{
			//If the process is not ready , go on is_waiting or sleeping
			if (p->is_wait || p->sleep_ticks)
			{
				continue;
			}

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
				//If the process is not ready , go on is_waiting or sleeping
				if (p->is_wait || p->sleep_ticks)
				{
					continue;
				}
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

///========================================================

PUBLIC void summary()
{
	while (1)
	{
		countR();
		milli_delay(10000);
	}
}

PRIVATE void countR()
{
	printf("============Summary============\n");
	printf("We have ");
	itoa(buffer, reader_count);
	printf(buffer);
	printf(" readers reading\n");
}

/*======================================================================*
                           reader
 *======================================================================*/
PUBLIC void reader(char *name, int len)
{
	sys_P(&count_mutex);
	{
		if (0>reader_count) reader_count = 0;
		if (0 == reader_count)
		{
			sys_P(&wrmutex); //block writer;
		}
		++reader_count;
	}
	sys_V(&count_mutex);

	//======read begin===========
	printf(name);
	printf(" begins reading\n");
	milli_delay(10000 * len);
	//stop read
	printf(name);
	printf(" stops reading\n");

	//============================

	sys_P(&count_mutex);
	{
		//reduce counter
		--reader_count;
		if (0 == reader_count)
			sys_V(&wrmutex); //release block writer
	}
	sys_V(&count_mutex);
	milli_delay(10000);
}

/*======================================================================*
                           writer
 *======================================================================*/
PUBLIC void writer(char *name, int len)
{
	sys_P(&wrmutex);
	//Begin read
	printf(name);
	printf(" begins writing\n");
	milli_delay(10000 * len);
	//stop read
	printf(name);
	printf(" stops writing\n");
	sys_V(&wrmutex);
	milli_delay(10000);
}