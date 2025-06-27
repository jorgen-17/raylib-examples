#include "raylib.h"
#include "./math.h"

#define LIVES 3
#define PLAYER_HEIGHT 15.0f
#define PLAYER_WIDTH 100.0f
#define PLAYER_SPEED 10.0f
#define BRICK_HEIGHT 20.0f
#define BRICK_WIDTH 90.0f
#define BRICK_SPACING 10.0f
#define WALL_THICKNESS 5.0f
#define COLLISION_MAX_DISTANCE 20
#define MESSAGE_TIMER 2.0f
#define FONT_SIZE 20.0f
#define SCORE_FONT_SIZE 40.0f
#define MESSAGE_TIMER 2.0f
#define INITIAL_BALL_SPEED_X 4.0f
#define INITIAL_BALL_SPEED_Y -4.0f
#define MAX_BALL_SPEED_X 8.0f
#define MAX_BALL_SPEED_Y 8.0f
#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 1080

typedef struct Ball
{
    Vector2 position;
    Vector2 speed;
    int radius;
    bool isLaunched;
} Ball;

typedef struct Brick
{
    Rectangle rect;
    bool broken;
} Brick;

typedef struct State
{
    int score;
    bool lifeLost;
    bool gameOver;
    bool gameWon;
} State;

void ResetState(State *state, Ball *ball, Brick *bricks, int numberOfBricks);
void UpdatePlayer(Rectangle *player, Ball *ball);
void UpdateBall(Ball *ball, Rectangle *player, Rectangle *walls, Brick *bricks, int numberOfBricks, State *state);

int brick_breaker()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "brick breaker");

    Rectangle player = {0};
    player.x = SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2;
    player.y = SCREEN_HEIGHT - PLAYER_HEIGHT - 2;
    player.width = PLAYER_WIDTH;
    player.height = PLAYER_HEIGHT;

    Ball ball = {0};
    ball.position.x = player.x + player.width / 2;
    ball.position.y = player.y - 20;
    ball.speed.x = INITIAL_BALL_SPEED_X;
    ball.speed.y = INITIAL_BALL_SPEED_Y;
    ball.radius = 10;
    ball.isLaunched = false;

    Rectangle leftWall = {0};
    leftWall.x = 0;
    leftWall.y = 0;
    leftWall.width = WALL_THICKNESS;
    leftWall.height = SCREEN_HEIGHT;
    Rectangle topWall = {0};
    topWall.x = 0;
    topWall.y = 0;
    topWall.width = SCREEN_WIDTH;
    topWall.height = WALL_THICKNESS;
    Rectangle rightWall = {0};
    rightWall.x = SCREEN_WIDTH - WALL_THICKNESS;
    rightWall.y = 0;
    rightWall.width = WALL_THICKNESS;
    rightWall.height = SCREEN_HEIGHT;
    Rectangle walls[] = {leftWall, topWall, rightWall};

    int numberOfBricks = 64;
    int rows = 8;
    int columns = 8;
    Brick bricks[numberOfBricks];
    int currentY = 85;
    int brickIndex = 0;
    for (int i = 0; i < rows; i++)
    {
        int currentX = 85;
        for (int j = 0; j < columns; j++)
        {
            Brick brick = {0};
            brick.broken = false;
            Rectangle rect = {0};
            rect.height = BRICK_HEIGHT;
            rect.width = BRICK_WIDTH;
            rect.x = currentX;
            rect.y = currentY;
            brick.rect = rect;
            bricks[brickIndex] = brick;
            brickIndex++;
            currentX += BRICK_WIDTH + BRICK_SPACING;
        }
        currentY += BRICK_HEIGHT + BRICK_SPACING;
    }

    Camera2D camera = {0};
    camera.target = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    State state = {0};
    state.lifeLost = false;
    state.gameOver = false;
    state.gameWon = false;
    state.score = 0;
    int lives = LIVES;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        if (IsKeyPressed(KEY_SPACE))
        {
            ball.isLaunched = true;
            state.gameOver = false;
            state.gameWon = false;
        }

        UpdatePlayer(&player, &ball);
        UpdateBall(&ball, &player, walls, bricks, numberOfBricks, &state);

        if (state.lifeLost)
        {
            ball.speed.x = INITIAL_BALL_SPEED_X;
            ball.speed.y = INITIAL_BALL_SPEED_Y;
            ball.isLaunched = false;
            state.lifeLost = false;
            lives--;
            if (lives <= 0)
            {
                state.gameOver = true;
                lives = LIVES;
                ResetState(&state, &ball, bricks, numberOfBricks);
            }
        }
        if (state.score >= numberOfBricks)
        {
            state.gameWon = true;
            lives = LIVES;
            ResetState(&state, &ball, bricks, numberOfBricks);
        }

        BeginDrawing();

        ClearBackground(DARKGRAY);

        DrawText(TextFormat("Lives: %i", lives), WALL_THICKNESS, WALL_THICKNESS, FONT_SIZE, GREEN);
        DrawText(TextFormat("Score: %i", state.score), SCREEN_WIDTH / 2 - 40, WALL_THICKNESS, FONT_SIZE, GREEN);

        if (state.gameOver)
            DrawText("Game over...press space to play again.", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2, FONT_SIZE, GREEN);
        if (state.gameWon)
            DrawText("You won! Press space to play again.", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2, FONT_SIZE, GREEN);
        // useful for debugging what the ball is doing
        // DrawText(TextFormat("ball speed (x: %0.2f, y: %0.2f)", ball.speed.x, ball.speed.y), SCREEN_WIDTH - 285, WALL_THICKNESS, FONT_SIZE, GREEN);

        for (int i = 0; i < 3; i++)
            DrawRectangleRec(walls[i], GREEN);

        for (int i = 0; i < numberOfBricks; i++)
            if (!bricks[i].broken)
                DrawRectangleRec(bricks[i].rect, GREEN);

        DrawRectangleRec(player, GREEN);

        DrawCircleV(ball.position, ball.radius, GREEN);

        EndMode2D();

        EndDrawing();
    }
}

void ResetState(State *state, Ball *ball, Brick *bricks, int numberOfBricks)
{
    ball->isLaunched = false;
    ball->speed.x = INITIAL_BALL_SPEED_X;
    ball->speed.y = INITIAL_BALL_SPEED_Y;
    state->lifeLost = false;
    state->score = 0;
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].broken = false;
}

void UpdatePlayer(Rectangle *player, Ball *ball)
{
    if ((IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) && player->x > WALL_THICKNESS)
        player->x -= PLAYER_SPEED;
    if ((IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) && player->x < SCREEN_WIDTH - player->width)
        player->x += PLAYER_SPEED;
}

void UpdateBall(Ball *ball, Rectangle *player, Rectangle *walls, Brick *bricks, int numberOfBricks, State *state)
{
    if (ball->isLaunched)
    {
        ball->position.x += ball->speed.x;
        ball->position.y += ball->speed.y;

        for (int i = 0; i < numberOfBricks; i++)
        {
            Brick currentBrick = bricks[i];
            if (!currentBrick.broken)
            {
                if (ball->position.x >= currentBrick.rect.x && ball->position.x <= currentBrick.rect.x + currentBrick.rect.width && ball->position.y >= currentBrick.rect.y && ball->position.y <= currentBrick.rect.y + currentBrick.rect.height)
                {
                    bool topOrBottomCollision = abs(ball->position.y - currentBrick.rect.y) <= COLLISION_MAX_DISTANCE || abs(ball->position.y - (currentBrick.rect.y + currentBrick.rect.height)) <= COLLISION_MAX_DISTANCE;
                    ball->speed.y *= topOrBottomCollision ? -1 : 1;

                    bool leftOrRightCollision = abs(ball->position.x - currentBrick.rect.x) <= COLLISION_MAX_DISTANCE || abs(ball->position.x - (currentBrick.rect.x + currentBrick.rect.width)) <= COLLISION_MAX_DISTANCE;
                    ball->speed.x *= leftOrRightCollision ? -1 : 1;

                    if (abs(ball->speed.x) < MAX_BALL_SPEED_X)
                        ball->speed.x += ball->speed.x > 0 ? 0.06 : -0.06;
                    if (abs(ball->speed.y) < MAX_BALL_SPEED_Y)
                        ball->speed.y += ball->speed.y > 0 ? 0.06 : -0.06;
                    bricks[i].broken = true;
                    state->score++;
                }
            }
        }

        Rectangle leftWall = walls[0];
        Rectangle topWall = walls[1];
        Rectangle rightWall = walls[2];
        // TODO theres a bug where if the ball hits the very corner bw two walls it'll ride one of the walls and sometimes leave the screen
        // probably have to take the ball radius into consideration in these calculations
        if (ball->position.x <= leftWall.width || ball->position.x >= rightWall.x)
        {
            ball->speed.x = -ball->speed.x;
        }
        else if (ball->position.y <= topWall.height)
        {
            ball->speed.y = -ball->speed.y;
        }
        // TODO: need to take ball radius into consideration with these collisions as I am seeing a bug where the ball drags along the player paddle
        else if (ball->position.y >= player->y && ball->position.x >= player->x && ball->position.x <= player->x + player->width)
        {
            ball->speed.y = -ball->speed.y;
        }
        else if (ball->position.y > SCREEN_HEIGHT)
        {
            state->lifeLost = true;
        }
    }
    else
    {
        ball->position.x = player->x + player->width / 2;
        ball->position.y = player->y - 20;
    }
}
