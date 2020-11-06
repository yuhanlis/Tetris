#include <stdio.h>
#include "game.h"
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
    if (x<0||x>=WIDTH)
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
        }
    }
    
    return res;
}


unsigned int my_movtognd(MAP m)
{
    
}

