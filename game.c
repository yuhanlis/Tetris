#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
extern unsigned int SHAPES[7][4][4][4];
/**
 * 
 *方块下落函数 
 * 
 * */
void my_fall(MAP m)
{
    if(!m->movtognd(m))
    {
        m->blk->addr[1]+=1;
    }
}

/**
 * 
 * 改变方块形状
 * */
void my_change(MAP m)
{
    if(!m->iscollision(m,3))
    {
        m->blk->status=(m->blk->status+1)%4;
    }
}

/**
 * 
 * 左右移动方块
 * */
void my_move(MAP m,unsigned int dir)
{
    if(!m->iscollision(m,dir))
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

/**
 * 
 * 判断执行某操作后方块是否碰撞或越界
 * 
 * */
unsigned int my_iscollection(MAP m,unsigned int act)
{
    unsigned int x= m->blk->addr[0];
    unsigned int y= m->blk->addr[1];
    unsigned int ** shape =(unsigned int **)SHAPES[m->blk->shape][m->blk->status];
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
            shape =(unsigned int **)SHAPES[m->blk->shape][(m->blk->status+1)%4];
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

/**
 * 判断方块是否落到底
 * */
unsigned int my_movtognd(MAP m)
{
    unsigned int x= m->blk->addr[0];
    unsigned int y= m->blk->addr[1];
    unsigned int ** shape =(unsigned int **)SHAPES[m->blk->shape][m->blk->status];
    unsigned int res = 0;
    unsigned int i,j;
    for(i=0;i<4;i++)
    {
        if((m->stage[x+i][y+3]==1&&shape[i][3]==1))
        {
            res = 1;
            free(m->blk);
            return res;
        }
        if(shape[i][3]==1&&y+3>=HEIGH)
        {
            res=1;
            free(m->blk);
            return res;
        }
    }
    return res;
}

/**
 * 
 * 随机生成方块
 * 
 * */
void my_creatblk(MAP m)
{
    unsigned int shape_random,status_random,color_random;    //随机数
    BLOCK block_temp = (BLOCK)malloc(sizeof(struct block));
    srand((unsigned int)time(NULL));
    shape_random=rand()%7;
    status_random=rand()%4;
    color_random = rand()%5;
    block_temp->shape=shape_random;
    block_temp->status=status_random;
    block_temp->addr[0]=0;
    block_temp->addr[1]=WIDTH/2;
    block_temp->color=color_random;
    m->blk=block_temp;
}

/**
 * 消除某一满行
 * */
void my_clearline(unsigned int  line,MAP m)
{
    unsigned int i = line;
    unsigned int j=0;
    for(i;i-1>0&&m->linestatus(i,m)!=0;i--)
    {
        for(j=0;j<WIDTH;j++)
        {
            m->stage[i][j]=m->stage[i-1][j];
        }
    }
    m->scores+=1;
}

/**
 * 
 * 返回一行的状态
 * 0空行
 * 1满行
 * 2不满不空
 * 3行越界
 * 
 * */
unsigned int  my_linestatus(unsigned int line,MAP m)
{
    unsigned int j =0 ;
    unsigned int res = m->stage[line][0]==1;
    if(line<0||line>=HEIGH)
    return 3;
    for(j=1;j<WIDTH;j++)
    {
        if(m->stage[line][j]!=res)
        {
            res = 2;
            return res;
        }
    }
    return res;
}


/**
 * 
 * 消除满行
 * */
void my_cleanfulllines(MAP m)
{
    unsigned int i = HEIGH-1;
    for(i;i>0;i--)
    {
        switch (m->linestatus(i,m))
        {
        case 0:             //空行直接结束
            return;
            break;
        case 1:
            m->clearline(i,m);          //满行消除行
            i++;
            break;
        default:
            break;
        }
    }
}

/**
 * 
 * 将已经落到底的方块复制到舞台中
 * */
void my_drawblk(MAP m)
{
    
    unsigned int i=0,j=0;
    for(i;i<4;i++)
    {
        for(j;j<4;j++)
        {
            m->stage[m->blk->addr[0]+i][m->blk->addr[1]+j]=SHAPES[m->blk->shape][m->blk->status][i][j];
        }
    }
}

/**
 * 判断游戏是否结束
 * */
unsigned int my_gameover(MAP m)
{
    if(m->linestatus(4,m)!=0)
    {
        return 1;
    }
    return 0;
}


unsigned int init_map(MAP m)
{
    m=(MAP)malloc(sizeof(struct map));
    unsigned int i,j;
    for(i=0;i<HEIGH;i++)
    {
        for(j;j<WIDTH;j++)
        {
            m->stage[i][j]=0;
        }
    }
    m->change=my_change;
    m->fall = my_fall;
    m->iscollision=my_iscollection;
    m->move = my_move;
    m->movtognd=my_movtognd;
    m->creatblk=my_creatblk;
    m->clearline=my_clearline;
    m->linestatus=my_linestatus;
    m->clearfulllines=my_cleanfulllines;
    m->drawblk = my_drawblk;
    m->isover =my_gameover;
    m->scores= 0;
}




/**
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 将数据操作与图像，按键合并
 * 
 * 
 * 
 * 
 * */