#include <stdio.h>
#include "game.h"
#define HEIGH 27
#define WIDTH 15
#define CLK   1000  //降落速度

#define SHAPE_L1 2
#define SHAPE_I 6
#define SHAPE_O 4
#define SHAPE_S 0
#define SHAPE_T 5
#define SHAPE_Z 1
#define SHAPE_L2 3

#define DIR_UP 0;
#define DIR_DO 1;
#define DIR_LE 2;
#define DIR_RE 3;

#define LEFT 0;
#define RIGHT 1;
#define DOWN 2;
#define CHANG 3;


unsigned int mapA [HEIGH][WIDTH];
unsigned int mapB [HEIGH][WIDTH];
unsigned int SHAPES [7][4][4][4];

typedef struct block* BLOCK;
typedef struct map*  MAP;

typedef    void (*pfunc_fall) (MAP);         //下落   
typedef    void (*pfunc_change)(MAP);       //旋转
typedef    void (*pfunc_move)(struct map*,unsigned int );       //移动
typedef    unsigned int (*pfunc_iscollision)(unsigned int,MAP);   //判断移动后是否会碰撞
typedef    unsigned int (*pfunc_movtognd)(MAP);  //判断是否落到底
typedef    void (*pfunc_clear)(MAP);        //消除行
typedef    void (*pfunc_creatblk)(MAP);     //生成方块
typedef    unsigned int (*pfunc_gameover)(MAP); //判断游戏是否结束
typedef    unsigned int (*pfunc_isfull)(MAP);   //判断一行是否填满
typedef    unsigned int (*pfunc_drawblk)(MAP);  //显示游戏中唯一的方块









void delay_ms(unsigned int time_ms);    //延时函数(毫秒)

SHAPES = 
{
    {
    { { 0,1,1,0 },{ 1,1,0,0 },{ 0,0,0,0 },{ 0,0,0,0 } },
    { { 1,0,0,0 },{ 1,1,0,0 },{ 0,1,0,0 },{ 0,0,0,0 } },
    { { 0,1,1,0 },{ 1,1,0,0 },{ 0,0,0,0 },{ 0,0,0,0 } },
    { { 1,0,0,0 },{ 1,1,0,0 },{ 0,1,0,0 },{ 0,0,0,0 } }
    }
    ,
    {
    { { 1,1,0,0 },{ 0,1,1,0 },{ 0,0,0,0 },{ 0,0,0,0 } },
    { { 0,1,0,0 },{ 1,1,0,0 },{ 1,0,0,0 },{ 0,0,0,0 } },
    { { 1,1,0,0 },{ 0,1,1,0 },{ 0,0,0,0 },{ 0,0,0,0 } },
    { { 0,1,0,0 },{ 1,1,0,0 },{ 1,0,0,0 },{ 0,0,0,0 } }
    }
    ,
    {
    { { 1,1,1,0 },{ 1,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 } },
    { { 1,0,0,0 },{ 1,0,0,0 },{ 1,1,0,0 },{ 0,0,0,0 } },
    { { 0,0,1,0 },{ 1,1,1,0 },{ 0,0,0,0 },{ 0,0,0,0 } },
    { { 1,1,0,0 },{ 0,1,0,0 },{ 0,1,0,0 },{ 0,0,0,0 } }
    }
    ,
    {
    { { 1,1,1,0 },{ 0,0,1,0 },{ 0,0,0,0 },{ 0,0,0,0 } },
    { { 1,1,0,0 },{ 1,0,0,0 },{ 1,0,0,0 },{ 0,0,0,0 } },
    { { 1,0,0,0 },{ 1,1,1,0 },{ 0,0,0,0 },{ 0,0,0,0 } },
    { { 0,1,0,0 },{ 0,1,0,0 },{ 1,1,0,0 },{ 0,0,0,0 } }
    }
    ,
    {
    { { 1,1,0,0 },{ 1,1,0,0 },{ 0,0,0,0 },{ 0,0,0,0 } },
    { { 1,1,0,0 },{ 1,1,0,0 },{ 0,0,0,0 },{ 0,0,0,0 } },
    { { 1,1,0,0 },{ 1,1,0,0 },{ 0,0,0,0 },{ 0,0,0,0 } },
    { { 1,1,0,0 },{ 1,1,0,0 },{ 0,0,0,0 },{ 0,0,0,0 } }
    }
    ,
    {
    { { 0,1,0,0 },{ 1,1,1,0 },{ 0,0,0,0 },{ 0,0,0,0 } },
    { { 0,1,0,0 },{ 1,1,0,0 },{ 0,1,0,0 },{ 0,0,0,0 } },
    { { 1,1,1,0 },{ 0,1,0,0 },{ 0,0,0,0 },{ 0,0,0,0 } },
    { { 1,0,0,0 },{ 1,1,0,0 },{ 1,0,0,0 },{ 0,0,0,0 } }
    }
    ,
    {
    { { 1,1,1,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 } },
    { { 1,0,0,0 },{ 1,0,0,0 },{ 1,0,0,0 },{ 1,0,0,0 } },
    { { 1,1,1,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 } },
    { { 1,0,0,0 },{ 1,0,0,0 },{ 1,0,0,0 },{ 1,0,0,0 } }
    }
    };


struct map
{
    unsigned int stage[HEIGH][WIDTH];
    BLOCK blk;
    pfunc_change change;
    pfunc_fall fall;
    pfunc_iscollision iscollision;
    pfunc_move  move;
    pfunc_movtognd movtognd;
};

struct block
{
    unsigned int shape;
    unsigned int status;
    unsigned int direction;
    unsigned int addr[2];
    
};



void my_fall(MAP m)
{
    if(!m->iscollision(2,m))
    {
        m->blk->addr[1]+=1;
    }
}



void my_change(MAP m)
{
    if(!m->iscollision(3,m))
    {
        m->blk->status=(m->blk->status+1)%4;
    }
}


void my_move(MAP m,unsigned int dir)
{
    if(!m->iscollision(dir,m))
    {
        if(dir==0)
        {
            m->blk->addr[0]-=1;
        }
        else if(dir==1)
        {
            m->blk->addr[0]+=1;
        }
    }
}


unsigned int my_iscollection(MAP m,unsigned int act)
{
    unsigned int x= m->blk->addr[0];
    unsigned int y= m->blk->addr[1];
    unsigned int ** shape =SHAPES[m->blk->shape][m->blk->status];
    unsigned int res = 0;
    switch(act)
    {
        case 0:
            x-=1;
            break;
        case 1:
            x+=1;
            break;
        case 3:
            shape =SHAPES[m->blk->shape][(m->blk->status+1)%4];
            break;
        default:
            break;
    }
    if (x<0)
    {
        res = 0;
    }
    unsigned int i,j;
    for(i=0;i<4;i++)
    {
        for(j=0;j<4;j++)
        {
            if(m->stage[x+i][y+j]==1&&shape[i][j]==1)
            {
                res = 1;
                return res;
            }
            if(shape[i][j]==1&&x+i>=WIDTH)
            {
                res =1;
                return res;
            }
        }
    }
    
    return res;
}


unsigned int my_movtognd(MAP m)
{
    unsigned int x= m->blk->addr[0];
    unsigned int y= m->blk->addr[1];
    unsigned int ** shape =SHAPES[m->blk->shape][m->blk->status];
    unsigned int res = 0;
    unsigned int i,j;
    for(i=0;i<4;i++)
    {
        if((m->stage[x+i][y+3]==1&&shape[i][3]==1))
        {
            res = 1;
            return res;
        }
        if(shape[i][3]==1&&y+3>=HEIGH)
        {
            res=1;
            return res;
        }
    }
    return res;
}




