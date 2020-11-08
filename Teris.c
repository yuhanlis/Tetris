#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/time.h>
#include<sys/stat.h>
#include<sys/select.h>
#include<sys/time.h>
#include<errno.h>
#include<pthread.h>
#include<linux/fb.h>
#include<sys/mman.h>
#include<string.h>
#include<sys/shm.h>
#include<termios.h>
#include <sys/ipc.h>

#include "game.h"

#define KEY	8888
#define SIZE	100
#define NEW_DATA	1
#define OLD_DATA	0


#define RED		63488
#define GREEN	2016
#define YELLOW	65504
#define BLUE	31
#define BLACK	0
#define WHITE	65535

#define LCD_WIDTH 800
#define LCD_HEIGHT 480
#define LCD_PIXCLOCK 2

#define LCD_RIGHT_MARGIN 67
#define LCD_LEFT_MARGIN 40
#define LCD_HSYNC_LEN 31

#define LCD_UPPER_MARGIN 25
#define LCD_LOWER_MARGIN 5
#define LCD_VSYNC_LEN 1

#define LCD_XSIZE  LCD_WIDTH
#define LCD_YSIZE  LCD_HEIGHT
#define SCR_XSIZE  LCD_WIDTH
#define SCR_YSIZE  LCD_HEIGHT

#define LCD_BLANK	20
#define C_UP		( LCD_XSIZE - LCD_BLANK*2 )
#define C_RIGHT		( LCD_XSIZE - LCD_BLANK*2 )
#define V_BLACK		( ( LCD_YSIZE - LCD_BLANK*4 ) / 6 )

extern unsigned int SHAPES[7][4][4][4];
//共享内存全局变量

int id;//A,B共享内存文件句柄
char* sharedMem = NULL;//A共享内存指针
//LCD相关函数
//所需全局变量
struct fb_var_screeninfo fb_var;//当前缓冲区的可变参数，用于LCD的基地址显示
struct fb_fix_screeninfo fb_fix;//固定参数，同上
char* fb_base_addr = NULL;//映射的LCD基地址
int display_fd;//lcd设备的文件句柄
long int screensize = 0;//屏幕大小

//按键相关全局变量
int buttons_fd;
char buttons[2]={'0','0'};
char first_flag=0;

unsigned int directionA,directionB;
//共享内存相关操作
void init_sharedmemory(void)
{
    id=shmget(KEY,SIZE,IPC_CREAT|0666);
    sharedMem = (char*)shmat(id,NULL,0);
    memset(sharedMem,0x00,SIZE);
    
}
void getB() ///通过共享内存获取B运动方向
{
    if(sharedMem[0]==NEW_DATA)
    {
        char dir = sharedMem[1];
        switch (dir)
        {
        case 'w':
            directionB=0;
            break;
        case 's':
            directionB=1;
            break;
        case 'a':
            directionB=2;
            break;
        case 'd':
            directionB=3;
            break;
        
        default:
            break;
        }

    }
}

//LCD相关操作
void TFT_LCD_Test(void);//LCD测试程序
//初始化屏幕
void init_screen()
{
    display_fd=open("/dev/fb0",O_RDWR);//打开设备
    if(display_fd<0)
    {
        perror("open device buttons failed!");
        exit(1);
    }
    //printf("lcd is open\n");
    if(ioctl(display_fd,FBIOGET_FSCREENINFO,&fb_fix))
    {
        ////printf("Error reading fb fixed information.\n");
        exit(1);
    }
    //printf("Get fixed screen information OK\n");
    if(ioctl(display_fd,FBIOGET_VSCREENINFO,&fb_var))
    {
        ////printf("Error reading fb variable information.\n");
        exit(1);
    }
    screensize=fb_var.xres*fb_var.yres*fb_var.bits_per_pixel/8;
    //printf("fb_var.xres:%d\n", fb_var.xres);
	//printf("fb_var.xres:%d\n", fb_var.yres);
    fb_base_addr = (char*)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, display_fd, 0);
	//printf("buffer is ok\n");
	if (-1 == (int)fb_base_addr)
	{
		////printf("errno\n");
		exit(1);
	}
	memset(fb_base_addr, 0xff, screensize);
    //TFT_LCD_Test();
    ////printf("Screen inited\n");
    return;
    
}

//绘制点
void draw_point(int x, int y, int color)//画点
{
	*((unsigned short*)(fb_base_addr + y * fb_var.xres * 2 + x * 2)) = color;
}
//使用某种颜色清屏
void Lcd_ClearScr(int color)//清屏
{
	unsigned int x, y;

	for (y = 0; y < SCR_YSIZE; y++)
	{
		for (x = 0; x < SCR_XSIZE; x++)
		{
			draw_point(x, y, color);
		}
	}
}
//绘制直线
void Glib_Line(int x1, int y1, int x2, int y2, int color)//画线
{
	int dx, dy, e;
	dx = x2 - x1;
	dy = y2 - y1;
	if (dx >= 0)
	{
		if (dy >= 0) // dy>=0
		{
			if (dx >= dy) // 1/8 octant
			{
				e = dy - dx / 2;
				while (x1 <= x2)
				{
					draw_point(x1, y1, color);
					if (e > 0) { y1 += 1; e -= dx; }
					x1 += 1;
					e += dy;
				}
			}
			else		// 2/8 octant
			{
				e = dx - dy / 2;
				while (y1 <= y2)
				{
					draw_point(x1, y1, color);
					if (e > 0) { x1 += 1; e -= dy; }
					y1 += 1;
					e += dx;
				}
			}
		}
		else		   // dy<0
		{
			dy = -dy;   // dy=abs(dy)

			if (dx >= dy) // 8/8 octant
			{
				e = dy - dx / 2;
				while (x1 <= x2)
				{
					draw_point(x1, y1, color);
					if (e > 0) { y1 -= 1; e -= dx; }
					x1 += 1;
					e += dy;
				}
			}
			else		// 7/8 octant
			{
				e = dx - dy / 2;
				while (y1 >= y2)
				{
					draw_point(x1, y1, color);
					if (e > 0) { x1 += 1; e -= dy; }
					y1 -= 1;
					e += dx;
				}
			}
		}
	}
	else //dx<0
	{
		dx = -dx;		//dx=abs(dx)
		if (dy >= 0) // dy>=0
		{
			if (dx >= dy) // 4/8 octant
			{
				e = dy - dx / 2;
				while (x1 >= x2)
				{
					draw_point(x1, y1, color);
					if (e > 0) { y1 += 1; e -= dx; }
					x1 -= 1;
					e += dy;
				}
			}
			else		// 3/8 octant
			{
				e = dx - dy / 2;
				while (y1 <= y2)
				{
					draw_point(x1, y1, color);
					if (e > 0) { x1 -= 1; e -= dy; }
					y1 += 1;
					e += dx;
				}
			}
		}
		else		   // dy<0
		{
			dy = -dy;   // dy=abs(dy)

			if (dx >= dy) // 5/8 octant
			{
				e = dy - dx / 2;
				while (x1 >= x2)
				{
					draw_point(x1, y1, color);
					if (e > 0) { y1 -= 1; e -= dx; }
					x1 -= 1;
					e += dy;
				}
			}
			else		// 6/8 octant
			{
				e = dx - dy / 2;
				while (y1 >= y2)
				{
					draw_point(x1, y1, color);
					if (e > 0) { x1 -= 1; e -= dy; }
					y1 -= 1;
					e += dx;
				}
			}
		}
	}
}

//绘制实心带框矩形
void Glib_FilledRectangle(int x1, int y1, int x2, int y2, int color)//填充矩形
{
	int i;
	for (i = y1+1; i <= y2-1; i++)
		Glib_Line(x1+1, i, x2-1, i, color);
    Glib_Line(x1,y1,x2,y1,BLACK);
    Glib_Line(x1,y2,x2,y2,BLACK);
    Glib_Line(x1,y1,x1,y2,BLACK);
    Glib_Line(x2,y1,x2,y2,BLACK);
}


///绘制空心矩形
void Glib_EmptyRectangle(int x1,int y1,int x2,int y2)
{
    int i;
    Glib_Line(x1,y1,x2,y1,BLACK);
    Glib_Line(x1,y2,x2,y2,BLACK);
    Glib_Line(x1,y1,x1,y2,BLACK);
    Glib_Line(x2,y1,x2,y2,BLACK);
}
//LCD测试程序
void TFT_LCD_Test(void)//LCD测试程序
{
	Lcd_ClearScr(BLACK);		//清空屏幕（黑）
	Lcd_ClearScr(WHITE);		//清空屏幕（白）
	Lcd_ClearScr(0xffff);

	Glib_FilledRectangle(LCD_BLANK, LCD_BLANK, (LCD_XSIZE - LCD_BLANK), (LCD_YSIZE - LCD_BLANK), 0x0000);						//fill a Rectangle with some color
	Glib_FilledRectangle((LCD_BLANK * 2), (LCD_BLANK * 2 + V_BLACK * 0), (C_RIGHT), (LCD_BLANK * 2 + V_BLACK * 1), 0x001f);		//fill a Rectangle with some color
	Glib_FilledRectangle((LCD_BLANK * 2), (LCD_BLANK * 2 + V_BLACK * 1), (C_RIGHT), (LCD_BLANK * 2 + V_BLACK * 2), 0x07e0);		//fill a Rectangle with some color
	Glib_FilledRectangle((LCD_BLANK * 2), (LCD_BLANK * 2 + V_BLACK * 2), (C_RIGHT), (LCD_BLANK * 2 + V_BLACK * 3), 0xf800);		//fill a Rectangle with some color
	Glib_FilledRectangle((LCD_BLANK * 2), (LCD_BLANK * 2 + V_BLACK * 3), (C_RIGHT), (LCD_BLANK * 2 + V_BLACK * 4), 0xffe0);		//fill a Rectangle with some color
	Glib_FilledRectangle((LCD_BLANK * 2), (LCD_BLANK * 2 + V_BLACK * 4), (C_RIGHT), (LCD_BLANK * 2 + V_BLACK * 5), 0xf81f);		//fill a Rectangle with some color
	Glib_FilledRectangle((LCD_BLANK * 2), (LCD_BLANK * 2 + V_BLACK * 5), (C_RIGHT), (LCD_BLANK * 2 + V_BLACK * 6), 0x07ff);		//fill a Rectangle with some color
}

//-----------按键相关函数--------------------



void initbuttons()
{
    buttons_fd=open("/dev/dial_key",0);
    if(buttons_fd<0)
    {
        perror("open device buttons failed!");
        exit(1);
    }
    char current_buttons[2];
    char realchar;
    
    

}


void delay(long time)
{
	long tmp = 25;
	while( time-- )
	{
		while(tmp--); 
		tmp = 25;
	}
}





//游戏相关变量
MAP gameA,gameB;

//画出游戏方块池
void Glib_Game(MAP m)
{
    unsigned int row = 4,column= 0;
    for(row;row<HEIGH;row++)
	{
		for(column;column<WIDTH;column++)
		{
			if(m->stage[row][column])
			{
				Glib_FilledRectangle(column*LCD_BLANK,(row-4)*LCD_BLANK,(column+1)*LCD_BLANK,(row-3)*LCD_BLANK,BLUE);
			}
			else
			{
				Glib_EmptyRectangle(column*LCD_BLANK,(row-4)*LCD_BLANK,(column+1)*LCD_BLANK,(row-3)*LCD_BLANK);
			}
		}
	}
}


/**
 * 
 * 画出方块
 * 
 * */
void Glib_Blk(MAP m)
{
	unsigned int x = m->blk->addr[0]-4;
	unsigned int y = m->blk->addr[1];
	int color;
	switch(m->blk->color)
	{
		case 0:
			color = RED;
			break;
		case 1:
			color = BLUE;
			break;
		case 2:
			color = YELLOW;
			break;
		case 3:
			color = GREEN;
			break;
		default:
			color= BLACK;
			break;	
	}
	unsigned int row = 0;
	unsigned int column=0;
	if(x>=4)
	{
		for(row;row<4;row++)
			{
				for(column;column<4;column++)
					{
						if(SHAPES[m->blk->shape][m->blk->shape][row][column])
						{
							Glib_FilledRectangle((x+column)*LCD_BLANK,(y+row)*LCD_BLANK,(x+column+1)*LCD_BLANK,(y+row+1)*LCD_BLANK,color);
						}
						else
						{
							Glib_EmptyRectangle((x+column)*LCD_BLANK,(y+row)*LCD_BLANK,(x+column+1)*LCD_BLANK,(y+row+1)*LCD_BLANK);
						}
					}
			}
	}
}


/**
 * 
 * 捕捉A按键，进行相关操作
 * 
 * */
void getA(MAP m)
{
	char current_buttons[2];
    char realchar;
    if(read(buttons_fd,current_buttons,sizeof(current_buttons))!=sizeof(current_buttons))
    {
        perror("read buttons:");
        exit(1);
    }
    if(buttons[0]!=current_buttons[0]||buttons[1]!=current_buttons[1])
        {
                buttons[0]=current_buttons[0];
                buttons[1]=current_buttons[1];
                switch (buttons[0])
                {   
                case 51:
					switch(buttons[1])
					{
						case 51:realchar='1';break;
						case 50:realchar='2';break;
						case 49:realchar='3';break;
						case 48:realchar='A';break;
					}break;
				case 50:
					switch(buttons[1])
					{
						case 51:realchar='4';break;
						case 50:realchar='5';break;
						case 49:realchar='6';break;
						case 48:realchar='B';break;
					}break;
				case 49:
					switch(buttons[1])
					{
						case 51:realchar='7';break;
						case 50:realchar='8';break;
						case 49:realchar='9';break;
						case 48:realchar='C';break;
					}break;
				case 48:
					switch(buttons[1])
					{
						case 51:realchar='*';break;
						case 50:realchar='0';break;
						case 49:realchar='#';break;
						case 48:realchar='D';break;
					}break;
                }
            }
            switch (realchar)
            {
            case '2':
                m->change(m);
                break;
            case '5':
                m->fall(m);
                break;
            case '4':
                m->move(m,LEFT);
                break;
            case '6':
				m->move(m,RIGHT);
                break;
            
            default:
                break;
    }
}


/**
 * 
 * 游戏整体逻辑
 * 
 * */
void game(MAP m)
{
	init_map(m);
	m->creatblk(m);
	while(1)
	{
		if(!m->movtognd(m))
		{
			m->fall(m);
		}
		if(m->movtognd(m))
		{
			m->drawblk(m);
			m->clearfulllines(m);
			if(m->isover(m))
			{
				break;
			}
			else
			{
				m->creatblk(m);
			}
			
		}
		Glib_Game(m);
		Glib_Blk(m);
	}
}


int main(void)
{
	MAP m;
	game(m);
	return 0;
}