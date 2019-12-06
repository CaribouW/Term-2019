
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

	for (p = proc_table + NR_TASKS; p < proc_table + NR_TASKS + NR_PROCS; p++)
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
                           sys_disp_str
 *======================================================================*/
PUBLIC int sys_disp_str(char *str)
{
	disp_str(str);
	return 0;
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