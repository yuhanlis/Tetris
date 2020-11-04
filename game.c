#include <stdio.h>
#define HEIGH 27
#define WEIGH 15
#define CLK   1000  //降落速度

#define SHAPE_L 0
#define SHAPE_I 1
#define SHAPE_O 2
#define SHAPE_S 3
#define SHAPE_T 4

#define DIR_UP 0;
#define DIR_DO 1;
#define DIR_LE 2;
#define DIR_RE 3;


unsigned int mapA [HEIGH][WEIGH];
unsigned int mapB [HEIGH][WEIGH];
unsigned int shape [5][2][4];


shape [SHAPE_I]={
                0,0,0,0,
                1,1,1,1
                };
shape [SHAPE_L]={
                0,0,0,1,
                0,1,1,1
                };
shape [SHAPE_O]={
                0,1,1,0,
                0,1,1,0
                };
shape [SHAPE_S]={
                0,1,1,0,
                0,0,1,1
                };
shape [SHAPE_T]={
                0,0,1,0,
                0,1,1,1
                };

struct block
{
    unsigned int shape[2][4];
    unsigned int direction;
    void (*down) (void);
    
};


void delay_ms(unsigned int t);
void 
