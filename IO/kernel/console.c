
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			      console.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
	回车键: 把光标移到第一列
	换行键: 把光标前进到下一行
*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"

PRIVATE void set_cursor(unsigned int position);
PRIVATE void set_video_start_addr(u32 addr);
PRIVATE void flush(CONSOLE *p_con);
PRIVATE void fill_symbol(u8 *pos, const u8 ch, const u8 color);

//The location set to the begin of the esc begin position
PRIVATE u8 *esc_begin_v_mem;
//Help to compute the length of the esc pattern string
PRIVATE int esc_begin;

//out char table and functions
typedef void (*out_char_f)(CONSOLE *p_con, char ch);
PRIVATE void normal_out_char(CONSOLE *p_con, char ch);
PRIVATE void esc_out_char(CONSOLE *p_con, char ch);
PRIVATE void enter_out_char(CONSOLE *p_con, char ch);
//Helper to put the chars
PRIVATE void put_ch(CONSOLE *p_con, const char ch); //ch could be '\t' or '\b'
//reset arr

PRIVATE MODIFY modifies[MODIFY_LIM];
PRIVATE int modify_index;
/*======================================================================*
			   init_screen
 *======================================================================*/
PUBLIC void init_screen(TTY *p_tty)
{
	int nr_tty = p_tty - tty_table;
	p_tty->p_console = console_table + nr_tty;

	int v_mem_size = V_MEM_SIZE >> 1; /* 显存总大小 (in WORD) */

	int con_v_mem_size = v_mem_size / NR_CONSOLES;
	p_tty->p_console->original_addr = nr_tty * con_v_mem_size;
	p_tty->p_console->v_mem_limit = con_v_mem_size;
	p_tty->p_console->current_start_addr = p_tty->p_console->original_addr;

	/* 默认光标位置在最开始处 */
	p_tty->p_console->cursor = p_tty->p_console->original_addr;

	if (nr_tty == 0)
	{
		/* 第一个控制台沿用原来的光标位置 */
		p_tty->p_console->cursor = disp_pos / 2;
		disp_pos = 0;
	}
	else
	{
		out_char(p_tty->p_console, nr_tty + '0');
		out_char(p_tty->p_console, '#');
	}

	esc_begin = modify_index = 0;

	set_cursor(p_tty->p_console->cursor);
}

/*======================================================================*
			   is_current_console
*======================================================================*/
PUBLIC int is_current_console(CONSOLE *p_con)
{
	return (p_con == &console_table[nr_current_console]);
}

/*======================================================================*
			   out_char
 *======================================================================*/
PUBLIC void out_char(CONSOLE *p_con, char ch)
{
	out_char_f out[3] = {normal_out_char, esc_out_char, enter_out_char};
	//func table
	//change among three modes
	out[ESC_MODE](p_con, ch);

	while (p_con->cursor >= p_con->current_start_addr + SCREEN_SIZE)
	{
		scroll_screen(p_con, SCR_DN);
	}

	flush(p_con);
}

/*======================================================================*
                           flush
*======================================================================*/
PRIVATE void flush(CONSOLE *p_con)
{
	set_cursor(p_con->cursor);
	set_video_start_addr(p_con->current_start_addr);
}

/*======================================================================*
			    set_cursor
 *======================================================================*/
PRIVATE void set_cursor(unsigned int position)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, CURSOR_H);
	out_byte(CRTC_DATA_REG, (position >> 8) & 0xFF);
	out_byte(CRTC_ADDR_REG, CURSOR_L);
	out_byte(CRTC_DATA_REG, position & 0xFF);
	enable_int();
}

/*======================================================================*
			  set_video_start_addr
 *======================================================================*/
PRIVATE void set_video_start_addr(u32 addr)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, START_ADDR_H);
	out_byte(CRTC_DATA_REG, (addr >> 8) & 0xFF);
	out_byte(CRTC_ADDR_REG, START_ADDR_L);
	out_byte(CRTC_DATA_REG, addr & 0xFF);
	enable_int();
}

/*======================================================================*
			   select_console
 *======================================================================*/
PUBLIC void select_console(int nr_console) /* 0 ~ (NR_CONSOLES - 1) */
{
	if ((nr_console < 0) || (nr_console >= NR_CONSOLES))
	{
		return;
	}

	nr_current_console = nr_console;

	set_cursor(console_table[nr_console].cursor);
	set_video_start_addr(console_table[nr_console].current_start_addr);
}

/*======================================================================*
			   scroll_screen
 *----------------------------------------------------------------------*
 滚屏.
 *----------------------------------------------------------------------*
 direction:
	SCR_UP	: 向上滚屏
	SCR_DN	: 向下滚屏
	其它	: 不做处理
 *======================================================================*/
PUBLIC void scroll_screen(CONSOLE *p_con, int direction)
{
	if (direction == SCR_UP)
	{
		if (p_con->current_start_addr > p_con->original_addr)
		{
			p_con->current_start_addr -= SCREEN_WIDTH;
		}
	}
	else if (direction == SCR_DN)
	{
		if (p_con->current_start_addr + SCREEN_SIZE <
			p_con->original_addr + p_con->v_mem_limit)
		{
			p_con->current_start_addr += SCREEN_WIDTH;
		}
	}
	else
	{
	}

	set_video_start_addr(p_con->current_start_addr);
	set_cursor(p_con->cursor);
}

/*====================================================
empty console contents
===========================*/
PUBLIC void empty(CONSOLE *p_con)
{
	p_con->cursor = p_con->original_addr;
	u8 *p_vmem = (u8 *)(V_MEM_BASE + p_con->cursor * 2);
	while (p_con->cursor < p_con->original_addr + p_con->v_mem_limit - 1)
	{
		fill_symbol(p_vmem, 0x0, DEFAULT_CHAR_COLOR);
		p_vmem += 2;
		p_con->cursor++;
	}
	p_con->cursor = p_con->original_addr;
	p_con->current_start_addr = p_con->original_addr;
	flush(p_con);
}

/*===============================
set single
===================================*/
PRIVATE void fill_symbol(u8 *pos, const u8 ch, const u8 color)
{
	pos[0] = ch;
	pos[1] = color;
}
//
PRIVATE void normal_out_char(CONSOLE *p_con, char ch)
{
	u8 *p_vmem = (u8 *)(V_MEM_BASE + p_con->cursor * 2); //cur
	MODIFY m;
	switch (ch)
	{
	case '\n':
		//ESC
		fill_symbol(p_vmem, 0x0A, SPECIAL_CHAR_COLOR);
		m.ch = 0x0A;
		m.cursor = p_con->cursor;
		m.is_add = 1;

		if (p_con->cursor < p_con->original_addr +
								p_con->v_mem_limit - SCREEN_WIDTH)
		{
			p_con->cursor = p_con->original_addr + SCREEN_WIDTH *
													   ((p_con->cursor - p_con->original_addr) /
															SCREEN_WIDTH +
														1);
		}
		modifies[modify_index++ % MODIFY_LIM] = m;

		break;
	case '\e':
		//enter into ESC mode
		esc_begin = p_con->cursor;
		esc_begin_v_mem = (u8 *)(V_MEM_BASE + p_con->cursor * 2);
		ESC_MODE = 1;
		break;
	case '\z': //reset pre'
		if (modify_index <= 0)
		{
			modify_index = 0;
		}
		else
		{
			m = modifies[--modify_index];
			//if append , we remove from [pre + 1] to end
			if (m.is_add)
			{
				int pre = m.cursor;
				int end = p_con->cursor;
				do
				{
					u8 *tmp_cursor = (u8 *)(V_MEM_BASE + pre * 2);
					pre += 2;
					fill_symbol(tmp_cursor, 0x0, DEFAULT_CHAR_COLOR);
				} while (pre <= end);
				p_con->cursor = m.cursor;
			}
			//if remove something , we add it back
			else
			{
				put_ch(p_con, m.ch);
				// u8 *tmp_cursor = (u8 *)(V_MEM_BASE + p_con->cursor * 2);
				// if (!(m.ch == 0x0 || m.ch == 0x09))
				// {
				// 	fill_symbol(tmp_cursor, m.ch, DEFAULT_CHAR_COLOR);
				// 	p_con->cursor++;
				// }
			}
		}
		break;
	case '\b':
		if (p_con->cursor > p_con->original_addr)
		{
			m.is_add = 0;
			m.cursor = p_con->cursor;
			u8 *p_vmem = (u8 *)(V_MEM_BASE + (p_con->cursor - 1) * 2); //cur
			m.ch = *p_vmem;
			modifies[modify_index++ % MODIFY_LIM] = m;
			put_ch(p_con, ch);
		}
		break;
	case '\t':
		m.ch = '\t';
		m.cursor = p_con->cursor;
		m.is_add = 1;
		put_ch(p_con, ch);
		modifies[modify_index++ % MODIFY_LIM] = m;
		break;
	default:
		if (p_con->cursor <
			p_con->original_addr + p_con->v_mem_limit - 1)
		{
			m.ch = ch;
			m.cursor = p_con->cursor;
			m.is_add = 1;
			fill_symbol(p_vmem, ch, DEFAULT_CHAR_COLOR);
			modifies[modify_index++ % MODIFY_LIM] = m;
			p_con->cursor++;
		}
		break;
	}
	//clean up
	if (modify_index >= MODIFY_LIM)
		modify_index = 0;
}
//ESC mode == 1
PRIVATE void esc_out_char(CONSOLE *p_con, char ch)
{
	int found = 0;
	int len = p_con->cursor - esc_begin;
	u8 *p_vmem = (u8 *)(V_MEM_BASE + p_con->cursor * 2); //cur
	switch (ch)
	{
		//\t print out normally
	case '\n':
		//Find pattern from beginning
		for (int p_cursor = p_con->current_start_addr;
			 p_cursor < esc_begin;
			 p_cursor++)
		{
			u8 *temp_p_vmem = (u8 *)(V_MEM_BASE + p_cursor * 2);
			for (int i = 0; i < len * 2; i += 2)
			{

				if (temp_p_vmem[i] != esc_begin_v_mem[i])
				{
					found = 0;
					break;
				}
				else
				{
					found = 1;
				}
			}
			if (found)
			{
				for (int i = 0; i < len; i++)
				{
					fill_symbol(temp_p_vmem, temp_p_vmem[0], ESC_CHAR_COLOR);
					temp_p_vmem += 2;
				}
			}
		}
		ESC_MODE = 2;
		break;

	case '\b':
		// pattern becomes null
		if (p_vmem <= esc_begin_v_mem)
			break;
	case '\t':
	case '\e':
		put_ch(p_con, ch);
		break;
	default:
		if (p_con->cursor <
			p_con->original_addr + p_con->v_mem_limit - 1)
		{
			fill_symbol(p_vmem, ch, ESC_CHAR_COLOR);
			p_con->cursor++;
		}
		break;
	}
}

//ESC mode == 2
PRIVATE void enter_out_char(CONSOLE *p_con, char ch)
{
	switch (ch)
	{
	case '\e':
		put_ch(p_con, ch);
		break;
	default:
		break;
	}
}

PRIVATE void put_ch(CONSOLE *p_con, const char ch)
{
	u8 *p_vmem = (u8 *)(V_MEM_BASE + p_con->cursor * 2); //cur
	switch (ch)
	{
	case '\t':
		if (p_con->cursor + 4 < p_con->original_addr +
									p_con->v_mem_limit)
		{
			int begin = p_con->original_addr + ((p_con->cursor - p_con->original_addr) / SCREEN_WIDTH) * SCREEN_WIDTH;
			int count = 4 - (p_con->cursor - begin) % 4;
			for (int i = 0; i < count; i++)
			{
				fill_symbol(p_vmem, 0x09, SPECIAL_CHAR_COLOR);
				p_vmem += 2;
				p_con->cursor++;
			}
		}
		break;
	case '\b':
		if (p_con->cursor > p_con->original_addr)
		{
			//Find the cursor before
			u8 *tmp_cursor = (u8 *)(V_MEM_BASE + (p_con->cursor - 1) * 2);
			if ('\0' == *tmp_cursor)
			{
				//return back to the char until ch != '\0'
				do
				{
					p_con->cursor--;
					tmp_cursor -= 2;
				} while (*tmp_cursor == '\0');
				--p_con->cursor;
				tmp_cursor = (u8 *)(V_MEM_BASE + (p_con->cursor) * 2);
				fill_symbol(tmp_cursor, BLANK, DEFAULT_CHAR_COLOR);
			}
			else if (0x09 == *tmp_cursor)
			{
				int count = 4;
				do
				{
					/* code */
					--p_con->cursor;
					--count;
					fill_symbol(tmp_cursor, BLANK, DEFAULT_CHAR_COLOR);
					tmp_cursor -= 2;
				} while (*tmp_cursor == 0x09 && count > 0);
			}
			else
			{
				p_con->cursor--;
				fill_symbol(tmp_cursor, 0x0, DEFAULT_CHAR_COLOR);
			}
		}
		break;
	case '\e':
		while (p_vmem > esc_begin_v_mem)
		{
			p_vmem = p_vmem - 2;
			fill_symbol(p_vmem, ' ', DEFAULT_CHAR_COLOR); //clean up pattern
			p_con->cursor--;
		}

		for (int p_cursor = p_con->current_start_addr; p_cursor <= p_con->cursor; p_cursor++)
		{
			u8 *temp_p_vmem = (u8 *)(V_MEM_BASE + p_cursor * 2);
			temp_p_vmem[0] == 0x09 || temp_p_vmem[0] == 0x0A ? fill_symbol(temp_p_vmem, temp_p_vmem[0], SPECIAL_CHAR_COLOR)
															 : fill_symbol(temp_p_vmem, temp_p_vmem[0], DEFAULT_CHAR_COLOR);
		}
		esc_begin_v_mem = esc_begin = ESC_MODE = 0;
		break;
	default:
		if (p_con->cursor <
			p_con->original_addr + p_con->v_mem_limit - 1)
		{
			fill_symbol(p_vmem, ch, DEFAULT_CHAR_COLOR);
			p_con->cursor++;
		}
		break;
	}
}