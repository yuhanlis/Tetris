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

void delay_ms(unsigned int time_ms);    //延时函数(毫秒)
void init (unsigned int map[HEIGH][WIDTH]);       //初始化背景
void fall(void);        //方块下降


