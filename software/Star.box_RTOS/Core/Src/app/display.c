#include "display.h"

// 矩阵屏配置
#define TOP_MATRIX     0  // 顶部矩阵编号
#define SIDE_MATRIX_1  1  // 四个侧面矩阵编号
#define SIDE_MATRIX_2  2
#define SIDE_MATRIX_3  3
#define SIDE_MATRIX_4  4



// 波浪的基础频率
#define WAVE_FREQUENCY 0.5
#define WAVE_AMPLITUDE 15



#define MATRIX_COUNT 5 // 矩阵数量



// 到底的点数量
#define MAX_BOTTOM_POINTS 20
uint8_t bottom_points = 0;

// 流动亮点的当前位置
typedef struct {
    int8_t x;      // x坐标
    int8_t y;      // y坐标
    uint8_t matrix; // 当前所在矩阵：0为顶部，1~4为侧面（1=前，2=后，3=左，4=右）
    uint8_t target; // 目标矩阵：流动方向（1=左，2=前，3=左，4=后）
} FlowPoint;

FlowPoint point1,point2,point3,point4,point5,point6;

// 初始化亮点：随机生成顶部点
void initFlow(FlowPoint *point) {
    point->x = rand() % 8;  // 随机列号
    point->y = rand() % 8;  // 随机行号
    point->matrix = 0;      // 顶部矩阵
    //bottom_points = 0;     // 重置计数器


    // 计算x和y分别与0和7的距离
    uint8_t dist_x_0 = abs(point->x - 0);
    uint8_t dist_x_7 = abs(point->x - 7);
    uint8_t dist_y_0 = abs(point->y - 0);
    uint8_t dist_y_7 = abs(point->y - 7);
    // 计算x和y分别离0或7更近的距离
    int min_dist_x = (dist_x_0 < dist_x_7) ? dist_x_0 : dist_x_7;
    int min_dist_y = (dist_y_0 < dist_y_7) ? dist_y_0 : dist_y_7;
    // 判断哪个更接近0或7
    if (min_dist_x < min_dist_y) {
        //printf("x 更接近 0 或 7\n");
        if (point->x<4)
        {
            point->target = 3; // 前侧
        } else {
            point->target = 1; // 后侧
        }
        
    } else if (min_dist_x >= min_dist_y) {
        //printf("y 更接近 0 或 7\n");
        if (point->y<4)
        {
            point->target = 4; 
        } else {
            point->target = 2; 
        }
    }
}


// 清屏函数
void clearAllMatrices(uint8_t (**led)[8]) {
    for (uint8_t i = 0; i < 5; i++) {
        // 清空每个矩阵
        for(uint8_t j = 0; j<8; j++)
        {
            (*led)[i][j]=0;
        }
    }
}


// 计算波浪的亮度
uint8_t getWaveBrightness(uint8_t x, uint8_t y, float time) {
    float wave = sin(WAVE_FREQUENCY * (x + y + time)) * WAVE_AMPLITUDE;
    return (uint8_t)(WAVE_AMPLITUDE + wave); // 亮度在 0-15 之间
}



// 更新波浪效果
void updateWaveEffect(uint8_t (**display)[8],float time) {
    for (uint8_t matrix = 0; matrix < 5; matrix++) {
        for (uint8_t y = 0; y < 8; y++) {
            for (uint8_t x = 0; x < 8; x++) {
                uint8_t brightness = getWaveBrightness(x, y, time);
                if (brightness > 0) {
                    (*display)[matrix][y] |= (1 << x); // 点亮对应的像素
                } else {
                    (*display)[matrix][y] &= ~(1 << x); // 熄灭对应的像素
                }
            }
        }
    }
    
}


// 检查并更新到底的点数量
void checkBottom(uint8_t (**led)[8],FlowPoint *point) {
    if ((point->matrix != 0 && point->y == 7)||(point->matrix != 0 &&point->x==0)) { // 非顶部矩阵且到达底部
        bottom_points++;
        if (bottom_points >= MAX_BOTTOM_POINTS) {
            bottom_points = 0;     // 重置计数器
            clearAllMatrices(led);
            initFlow(&point1); // 生成新的亮点
            initFlow(&point2); // 生成新的亮点
        } else {
            initFlow(point); // 生成新的亮点
        }
    }
}


// 亮点向下流动
void flowDown(uint8_t (**led)[8],FlowPoint *point) {
    // 点亮当前亮点
     (*led)[point->matrix][point->y] |= (1 << point->x);

    // 更新亮点位置
    if (point->matrix == 0) { // 如果在顶部矩阵
        if (point->target == 1 && point->x < 7) {
            point->x++; 
        } else if (point->target == 2 && point->y < 7) {
            point->y++; 
        } else if (point->target == 3 && point->x > 0) {
            point->x--; 
        } else if (point->target == 4 && point->y > 0) {
            point->y--; 
        } else {
            // 到达边缘，流向对应侧面矩阵
            point->matrix = point->target;
            //point->y = 0; // 重置行号

            if(point->matrix==1||point->matrix==3)
            {
                if(point->matrix==1)point->x = 7-point->y;
                else if(point->matrix==3)point->x=point->y;
                point->y = 0; // 重置行号
            }
            else if(point->matrix==2||point->matrix==4)
            {
                if(point->matrix==2)point->y=point->x;
                else if(point->matrix==4)point->y=7-point->x;
                point->x=7;
            }
        }
    } else { // 如果在侧面矩阵
        if (point->y < 7&&(point->matrix==1||point->matrix==3)) {
            point->y++;
        } else if (point->x>0&&(point->matrix==2||point->matrix==4)){
            point->x--;
        }
        else {
            checkBottom(led,point); // 检查是否到底
        }
    }
}


// 点亮或熄灭指定矩阵的正方形区域
void drawSquare(uint8_t matrix, uint8_t size, uint8_t state,uint8_t (**led)[8]) {
    if (size > 8) size = 8; // 限制最大范围
    uint8_t start = (8 - size) / 2; // 计算正方形起点
    uint8_t end = start + size;    // 计算正方形终点

    for (uint8_t y = start; y < end; y++) {
        for (uint8_t x = start; x < end; x++) {
            if (state) {
                (*led)[matrix][y] |= (1 << x); // 点亮像素
            } else {
                (*led)[matrix][y] &= ~(1 << x); // 熄灭像素
            }
        }
    }
}

// 点亮和熄灭效果
void lightEffect(uint8_t (**led)[8],uint8_t *status) {
    uint8_t size = 2;       // 起始正方形大小
    uint8_t increasing = 1; // 1表示扩大正方形，0表示缩小正方形

    while (1) {
        if (*status==0) break; 
        // 更新每个矩阵
        for (uint8_t matrix = 0; matrix < MATRIX_COUNT; matrix++) {
            drawSquare(matrix, size, increasing,led); // 绘制正方形
        }

        vTaskDelay(150);   // 控制动画速度

        // 调整正方形大小
        if (increasing) {
            size += 2; // 扩大正方形
            if (size > 8) {
                size = 2;
                increasing = 0; // 切换为缩小
            }
        } else {
            size += 2; // 缩小正方形
            if (size >8 ) {
                size = 0;
                //clearAllMatrices(led); // 整个面清空
                increasing = 1; // 切换为扩大
            }
        }
    }
}

void display1Main(uint8_t (**led)[8],uint8_t *status)
{
    clearAllMatrices(led);
    
    float time = 0.0;
    
    while (1) {
        if (*status==0) break; 
        updateWaveEffect(led,time); // 更新波浪效果
        time += 0.1;            // 控制波浪移动速度
        vTaskDelay(50);        // 控制速度
    }
}



void display2Main(uint8_t (**led)[8],uint8_t *status)
{

    srand(SysTick->VAL);
    clearAllMatrices(led);
    initFlow(&point1);
    vTaskDelay(3);
    initFlow(&point2);
    vTaskDelay(3);
    initFlow(&point3);
    vTaskDelay(3);
    initFlow(&point4);
    vTaskDelay(3);
    initFlow(&point5);
    vTaskDelay(3);
    initFlow(&point6);
    while (1) {
        if (*status==0) break; 
        flowDown(led,&point1);      // 更新亮点流动
        flowDown(led,&point2);      // 更新亮点流动
        //flowDown(led,&point3);      // 更新亮点流动
        //lowDown(led,&point4);      // 更新亮点流动
        //flowDown(led,&point5);      // 更新亮点流动
        //flowDown(led,&point6);      // 更新亮点流动

        vTaskDelay(230);        // 控制速度
    }
}

void display3Main(uint8_t (**led)[8],uint8_t *status)
{
    clearAllMatrices(led);

    // 执行动画效果
    lightEffect(led,status);


}

