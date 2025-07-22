#include <stdlib.h>
#include "raylib.h"

#include "./math.h"

#define FONT_SIZE 20.0f
#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 1080

#define FOOD_RADIUS 10.0f
#define SEGMENT_SIZE 20.0f
#define SEGMENT_SPACING 10.0f
#define SNAKE_SPEED (SEGMENT_SIZE + SEGMENT_SPACING) // distance snake covers each tick
#define NUM_ROWS (SCREEN_WIDTH / SNAKE_SPEED)
#define NUM_COLUMNS (SCREEN_HEIGHT / SNAKE_SPEED)
#define MAX_SEGMENTS (int)(NUM_ROWS * NUM_COLUMNS) // longest length snake can be
#define COLLISION_SPACING ((FOOD_RADIUS + SEGMENT_SIZE / 2) / 2) // max spacing where collision is detected
#define MESSAGE_TIMER 2.0f
#define TICK_TIME 0.2f

typedef struct Segment
{
    Vector2 pos;
    Vector2 currDir; // current direction
    Vector2 nextDir; // next direction
    struct Segment *next;
    struct Segment *prev;
} Segment;

typedef struct Snake
{
    struct Segment *segments;
    struct Segment *tail;
} Snake;

typedef struct Food
{
    Vector2 pos;
} Food;

void CheckInputs(Snake *snake);
void UpdateSnake(Snake *snake, Food *food);
void WrapAroundScreen(Segment *segment);
void HandleSnakeFoodCollision(Snake *snake, Food *food);
bool CheckSegmentFoodCollision(Segment segment, Food food);
void MoveFoodOnceEaten(Snake *snake, Food *food);

// manuall including raymath funtions here bc was getting an error including raymath.h
Vector2 Vector2Zero();
Vector2 Vector2Scale(Vector2 v, float scale);
Vector2 Vector2Add(Vector2 v1, Vector2 v2);
Vector2 Vector2Multiply(Vector2 v1, Vector2 v2);
Vector2 Vector2Negate(Vector2 v);
bool Vector2Equals(Vector2 v1, Vector2 v2);
float Vector2Distance(Vector2 v1, Vector2 v2);

int snake()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "snake");

    Snake snake = {
        .segments = malloc(MAX_SEGMENTS * sizeof(Segment)),
        .tail = 0
    };
    Segment *head = &snake.segments[0];
    head->pos = (Vector2){
        .x = SCREEN_WIDTH / 2,
        .y = SCREEN_HEIGHT / 2
    };
    head->currDir = (Vector2){.x = -1, .y = 0}; // moving left
    head->nextDir = Vector2Zero();
    head->next = NULL;
    head->prev = NULL;
    snake.tail = head;

    Food food = {0};
    food.pos = (Vector2){.x = SCREEN_WIDTH / 4 + FOOD_RADIUS, .y = SCREEN_HEIGHT / 2 +  FOOD_RADIUS};

    SetTargetFPS(60);

    float tickTimer = 0.0f;

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        CheckInputs(&snake);
        if (tickTimer >= TICK_TIME)
        {
            tickTimer = 0.0f;
            UpdateSnake(&snake, &food);
        }
        else
        {
            tickTimer += deltaTime;
        }

        BeginDrawing();

        ClearBackground(DARKGRAY);

        int index = 0;
        Segment *current = &snake.segments[index];
        while (current <= snake.tail)
        {
            DrawRectangleRec((Rectangle){.x = current->pos.x, .y = current->pos.y, .width = SEGMENT_SIZE, .height = SEGMENT_SIZE}, GREEN);
            index += sizeof(Segment);
            current = &snake.segments[index];
        }

        DrawCircleV(food.pos, FOOD_RADIUS, GREEN);

        EndMode2D();

        EndDrawing();
    }
}

void CheckInputs(Snake *snake)
{
    Segment *head = &snake->segments[0];
    if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && head->currDir.x <= 0)
    {
        head->currDir.x = -1;
        head->currDir.y = 0;
    }
    else if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && head->currDir.x >= 0)
    {
        head->currDir.x = 1;
        head->currDir.y = 0;
    }
    else if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && head->currDir.y <= 0)
    {
        head->currDir.x = 0;
        head->currDir.y = -1;
    }
    else if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && head->currDir.y >= 0)
    {
        head->currDir.x = 0;
        head->currDir.y = 1;
    }
}

void UpdateSnake(Snake *snake, Food *food)
{
    int index = 0;
    Segment *current = &snake->segments[index];
    while (current <= snake->tail)
    {
        Vector2 zero = Vector2Zero();
        if (!Vector2Equals(current->nextDir, zero))
        {
            current->currDir = current->nextDir;
            current->nextDir = zero;
        }
        Vector2 displacement = Vector2Scale(current->currDir, SNAKE_SPEED);
        current->pos = Vector2Add(current->pos, displacement);
        WrapAroundScreen(current);
        if (current->prev != NULL)
            current->nextDir = Vector2Equals(current->currDir, current->prev->currDir) ? zero : current->prev->currDir;
        index += sizeof(Segment);
        current = &snake->segments[index];
    }
    HandleSnakeFoodCollision(snake, food);
}

void WrapAroundScreen(Segment *segment)
{
    if (segment->pos.x + SEGMENT_SIZE <= 0)
        segment->pos.x = SCREEN_WIDTH;
    else if (segment->pos.x >= SCREEN_WIDTH)
        segment->pos.x = 0;
    if (segment->pos.y + SEGMENT_SIZE <= 0)
        segment->pos.y = SCREEN_HEIGHT;
    else if (segment->pos.y >= SCREEN_HEIGHT)
        segment->pos.y = 0;
}

void HandleSnakeFoodCollision(Snake *snake, Food *food)
{
    if (CheckSegmentFoodCollision(snake->segments[0], *food))
    {
        TraceLog(LOG_INFO, "snake collided with food");

        Segment *tail = snake->tail;
        Vector2 offset = Vector2Scale(Vector2Negate(tail->currDir), SNAKE_SPEED); 

        Segment *nextSeg = snake->tail + sizeof(Segment);
        nextSeg->pos = Vector2Add(tail->pos, offset);
        nextSeg->currDir = tail->currDir;
        nextSeg->nextDir = Vector2Zero();
        nextSeg->next = NULL;
        nextSeg->prev = tail; // segmentation fault here when snake reaches length of 33

        tail->next = nextSeg;
        snake->tail = nextSeg;

        MoveFoodOnceEaten(snake, food);
    }
}

bool CheckSegmentFoodCollision(Segment segment, Food food)
{
    Vector2 segmentCenter = {
        .x = segment.pos.x + SEGMENT_SIZE / 2,
        .y = segment.pos.y + SEGMENT_SIZE / 2,
    };
    int distance = Vector2Distance(segmentCenter, food.pos);

    return distance <= COLLISION_SPACING;
}

void MoveFoodOnceEaten(Snake *snake, Food *food)
{
    bool spaceFound = false;
    int newX = 10;
    int newY = 10;

    while (!spaceFound)
    {
        // f(x) = SNAKE_SPEED * x + FOOD_RADIUS where x can be from 0 to NUM_ROWS
        int randomRow = GetRandomValue(0, NUM_ROWS - 1);
        newX = (SNAKE_SPEED * randomRow) + FOOD_RADIUS;
        // f(y) = SNAKE_SPEED * y + FOOD_RADIUS where y can be from 0 to NUM_COLUMNS
        int randomColumn = GetRandomValue(0, NUM_COLUMNS - 1);
        newY = (SNAKE_SPEED * randomColumn) + FOOD_RADIUS;

        int index = 0;
        Segment *current = &snake->segments[index];
        while (current <= snake->tail)
        {
            Food newFood = {
                .pos = (Vector2){newX, newY}
            };
            if(CheckSegmentFoodCollision(*current, newFood))
                break;

            index += sizeof(Segment);
            current = &snake->segments[index];
        }
        if (current > snake->tail) // iterated through whole snake
            spaceFound = true;
        // else won the game?
    }

    food->pos.x = newX;
    food->pos.y = newY;
}