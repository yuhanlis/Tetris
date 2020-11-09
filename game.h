#define HEIGH 24
#define WIDTH 40
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

#define LEFT 0
#define RIGHT 1
#define DOWN 2
#define CHANG 3


unsigned int mapA [HEIGH][WIDTH];
unsigned int mapB [HEIGH][WIDTH];


typedef struct block BLOCK;
typedef struct map*  MAP;

typedef    void (*pfunc_fall) (MAP);         //下落   
typedef    void (*pfunc_change)(MAP);       //旋转
typedef    void (*pfunc_move)(struct map*,unsigned int );       //移动
typedef    unsigned int (*pfunc_iscollision)(MAP,unsigned int);   //判断移动后是否会碰撞
typedef    unsigned int (*pfunc_movtognd)(MAP);  //判断是否落到底
typedef    void (*pfunc_clearline)(unsigned int ,MAP);        //消除一行
typedef    void (*pfunc_creatblk)(MAP);     //生成方块
typedef    unsigned int (*pfunc_gameover)(MAP); //判断游戏是否结束
typedef    unsigned int (*pfunc_linestatus)(unsigned int,MAP);   //判断一行状态
typedef    void (*pfunc_drawblk)(MAP);  //显示游戏中唯一的方块
typedef    void (*pfunc_clearfulllines)(MAP);   //清除所有满行

unsigned int init_map(MAP m);






void delay_ms(unsigned int time_ms);    //延时函数(毫秒)

static unsigned int SHAPES [7][4][4][4]= 
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


struct block
{
    unsigned int shape;
    unsigned int status;
    unsigned int addr[2];
    unsigned int color;
    
};
struct map
{
    unsigned int stage[HEIGH][WIDTH];
    unsigned int scores;
    BLOCK blk;
    pfunc_change change;
    pfunc_fall fall;
    pfunc_iscollision iscollision;
    pfunc_move  move;
    pfunc_movtognd movtognd;




    pfunc_creatblk creatblk;
    pfunc_clearline clearline;
    pfunc_linestatus linestatus;
    pfunc_clearfulllines clearfulllines;
    pfunc_drawblk drawblk;
    pfunc_gameover isover;

};

