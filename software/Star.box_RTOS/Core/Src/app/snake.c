#include "snake.h"

#define SIZE 8       // 每个面为8x8
#define FACE_COUNT 5 // 5个面：上、前、后、左、右
#define MAX_FOODS 8  // 最大豆子数量

// 表示方向
typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

// 定义一个点
typedef struct {
    int8_t x;
    int8_t y;
    int8_t face;
} Point;

// 定义蛇结构
typedef struct {
    Point body[SIZE * SIZE * FACE_COUNT];
    uint16_t length;
    Direction direction;
} Snake;

// 初始化立方体每个面和豆子位置
uint8_t cube[FACE_COUNT][SIZE][SIZE] = {0};
Point foods[MAX_FOODS]; // 保存豆子的位置

// 方向转移矩阵
uint8_t dx[4] = {-1, 1, 0, 0}; // UP, DOWN, LEFT, RIGHT
uint8_t dy[4] = {0, 0, -1, 1};


uint8_t map[5][8];

// 跨面映射函数
Point wrap(Point p) {
    if (p.x < 0) {
        if (p.face == 0) { p.face = 4; p.x = p.y; p.y=0; }
        else if (p.face == 1) { p.face = 0; p.x = p.y; p.y=0; }
        else if (p.face == 2) { p.face = 3; p.x = p.y; p.y=0;}
        else if (p.face == 3) { p.face = 0; p.x = SIZE-p.y-1; p.y=SIZE-1; }
        else if (p.face == 4) { p.face = 1; p.x = p.y; p.y=0;}
    } else if (p.x >= SIZE) {
        if (p.face == 0) { p.face = 2; p.x = SIZE-p.y-1; p.y=0; }
        else if (p.face == 1) { p.face = 3; p.x = SIZE-1; p.y=SIZE-p.y-1; }
        else if (p.face == 2) { p.face = 1; p.x = p.y; p.y=SIZE-1; }
        else if (p.face == 3) { p.face = 1; p.x = SIZE-1; p.y=SIZE-p.y-1; }
        else if (p.face == 4) { p.face = 3; p.x = p.y; p.y=SIZE-1; }
    } else if (p.y < 0) {
        if (p.face == 0) { p.face = 1; p.y = p.x; p.x=0;}
        else if (p.face == 1) { p.face = 4; p.y = p.x; p.x=0; }
        else if (p.face == 2) { p.face = 0; p.y = SIZE-p.x-1; p.x=SIZE-1; }
        else if (p.face == 3) { p.face = 2; p.y = p.x; p.x=0; }
        else if (p.face == 4) { p.face = 0; p.y = p.x; p.x=0; }
    } else if (p.y >= SIZE) {
        if (p.face == 0) { p.face = 3; p.y = SIZE-p.x-1;p.x=0; }
        else if (p.face == 1) { p.face = 2; p.y=p.x; p.x=SIZE-1;}
        else if (p.face == 2) { p.face = 4; p.y = SIZE-1; p.x=SIZE-p.x-1; }
        else if (p.face == 3) { p.face = 4; p.y = p.x; p.x=SIZE-1; }
        else if (p.face == 4) { p.face = 2; p.y = SIZE-1; p.x=SIZE-p.x-1; }
    }
    return p;
}

// 随机生成豆子，确保不与蛇位置重叠
void generate_food(Snake *snake) {
    for (int i = 0; i < MAX_FOODS; ++i) {
        int valid = 0;
        while (!valid) {
            foods[i].face = rand() % FACE_COUNT;
            foods[i].x = rand() % SIZE;
            foods[i].y = rand() % SIZE;
            valid = 1;

            // 检查是否和蛇重叠
            for (int j = 0; j < snake->length; ++j) {
                if (foods[i].face == snake->body[j].face &&
                    foods[i].x == snake->body[j].x &&
                    foods[i].y == snake->body[j].y) {
                    valid = 0;
                    break;
                }
            }
        }
    }
}


// 初始化蛇
void init_snake(Snake *snake) {
    snake->length = 3;
    snake->direction = RIGHT;
    snake->body[0] = (Point){3, 3, 0}; // 从上面的中央开始
    snake->body[1] = (Point){3, 2, 0};
    snake->body[2] = (Point){3, 1, 0};
    generate_food(snake); // 初始生成豆子
}

// 更新方向
void update_direction(Snake *snake) {
    if(datampu.Ax>0.40)
    {
        if(snake->body[0].face==0)if (snake->direction != UP) snake->direction = DOWN;
        if(snake->body[0].face==1)if (snake->direction != LEFT) snake->direction = RIGHT;
        if(snake->body[0].face==3)if (snake->direction != RIGHT) snake->direction = LEFT;
    }
    if(datampu.Ax<-0.40)
    {
        if(snake->body[0].face==0)if (snake->direction != DOWN) snake->direction = UP;
        if(snake->body[0].face==1)if (snake->direction != RIGHT) snake->direction = LEFT;
        if(snake->body[0].face==3)if (snake->direction != LEFT) snake->direction = RIGHT;
    }
    if(datampu.Ay>0.40)
    {
        if(snake->body[0].face==0)if (snake->direction != RIGHT) snake->direction = LEFT;
        if(snake->body[0].face==2)if (snake->direction != UP) snake->direction = DOWN;
        if(snake->body[0].face==4)if (snake->direction != DOWN) snake->direction = UP;
    }
    if(datampu.Ay<-0.40)
    {
        if(snake->body[0].face==0)if (snake->direction != LEFT) snake->direction = RIGHT;
        if(snake->body[0].face==2)if (snake->direction != DOWN) snake->direction = UP;
        if(snake->body[0].face==4)if (snake->direction != UP) snake->direction = DOWN;
    }
    if(datampu.Az>0.40)
    {
        if(snake->body[0].face==1)if (snake->direction != DOWN) snake->direction = UP;
        if(snake->body[0].face==2)if (snake->direction != RIGHT) snake->direction = LEFT;
        if(snake->body[0].face==3)if (snake->direction != DOWN) snake->direction = UP;
        if(snake->body[0].face==4)if (snake->direction != RIGHT) snake->direction = LEFT;
    }
    if(datampu.Az<-0.40)
    {
        if(snake->body[0].face==1)if (snake->direction != UP) snake->direction = DOWN;
        if(snake->body[0].face==2)if (snake->direction != LEFT) snake->direction = RIGHT;
        if(snake->body[0].face==3)if (snake->direction != UP) snake->direction = DOWN;
        if(snake->body[0].face==4)if (snake->direction != LEFT) snake->direction = RIGHT;
    }
}

// 更新蛇的位置
uint8_t update_snake(Snake *snake) {
    Point new_head = snake->body[0];
    new_head.x += dx[snake->direction];
    new_head.y += dy[snake->direction];
    new_head = wrap(new_head); // 处理跨面移动

    // 检查是否碰到自己
    for (int i = 0; i < snake->length; ++i) {
        if (snake->body[i].face == new_head.face &&
            snake->body[i].x == new_head.x &&
            snake->body[i].y == new_head.y) {
            //printf("Game Over! Snake hit itself.\n");
            return 0; // 游戏结束
        }
    }

    // 检查是否吃到豆子
    //int ate_food = 0;
    for (int i = 0; i < MAX_FOODS; ++i) {
        if (foods[i].face == new_head.face &&
            foods[i].x == new_head.x &&
            foods[i].y == new_head.y) {
            //ate_food = 1;
            snake->length++;
            generate_food(snake); // 生成新的豆子
            break;
        }
    }

    // 移动身体
    for (int i = snake->length - 1; i > 0; --i) {
        snake->body[i] = snake->body[i - 1];
    }
    snake->body[0] = new_head;

    // 更新立方体上蛇的位置
    for (int i = 0; i < FACE_COUNT; ++i)
        for (int x = 0; x < SIZE; ++x)
            for (int y = 0; y < SIZE; ++y)
                cube[i][x][y] = 0;

    // 更新蛇和豆子的位置
    for (int i = 0; i < snake->length; ++i) {
        cube[snake->body[i].face][snake->body[i].x][snake->body[i].y] = 1;
    }
    for (int i = 0; i < MAX_FOODS; ++i) {
        //cube[foods[i].face][foods[i].x][foods[i].y] = 2; // 用2表示豆子
        cube[foods[i].face][foods[i].x][foods[i].y] = 1; 
    }

    return 1; // 游戏继续
}

void print_cube(uint8_t (**ledBlock)[8])
{
    for (int f = 0; f < FACE_COUNT; ++f) 
    {
        for (int x = 0; x < SIZE; ++x)
        {
            map[f][x]=0;
            for (int y = 0; y < SIZE; ++y)
            {
                map[f][x] |= (cube[f][x][y] << (7 - y));
            }
        }
    }
    *ledBlock=map;
}

uint8_t snakeGame(uint8_t (**ledBlock)[8],uint8_t *status)
{
    srand(SysTick->VAL);
    Snake snake;
    init_snake(&snake);

    while (1) {
        update_direction(&snake); // 更新方向
        if (!update_snake(&snake)||*status==0) break; // 如果游戏结束则退出循环
        print_cube(ledBlock);
        //getchar(); // 暂停，用于查看每次移动
        vTaskDelay(600);
    }

    return 0;
}