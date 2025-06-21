#include "raylib.h"

#define PLAYER_HEIGHT 100.0f
#define PLAYER_WIDTH 10.0f
#define WALL_THICKNESS 5.0f
#define FONT_SIZE 20.0f
#define SCORE_FONT_SIZE 40.0f
#define MESSAGE_TIMER 2.0f
#define WINNING_SCORE 5
#define NUMBER_OF_WALLS 2
#define INITIAL_BALL_SPEED_X 5.0f;
#define INITIAL_BALL_SPEED_Y 4.0f;
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

typedef struct Score
{
    int player1;
    int player2;
    bool player1Goal;
    bool player2Goal;
} Score;

typedef struct Ball
{
    Vector2 position;
    Vector2 speed;
    int radius;
} Ball;

typedef struct Timers
{
    float player1Goal;
    float player2Goal;
    float player1Won;
    float player2Won;
} Timers;

void ResetBall(Ball *ball, bool player1Goal);
void UpdatePlayer1(Rectangle *player1, int playerSpeed);
void UpdatePlayer2(Rectangle *player2, Ball *ball, int playerSpeed, bool player2Computer);
void UpdateBall(Ball *ball, Rectangle *player1, Rectangle *player2, Rectangle *walls, Score *score);

int pong()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib [core] example - 2d camera");

    Rectangle player1 = {0};
    player1.x = 2;
    player1.y = SCREEN_HEIGHT / 2;
    player1.width = PLAYER_WIDTH;
    player1.height = PLAYER_HEIGHT;

    Rectangle player2 = {0};
    player2.x = SCREEN_WIDTH - PLAYER_WIDTH - 2;
    player2.y = SCREEN_HEIGHT / 2;
    player2.width = PLAYER_WIDTH;
    player2.height = PLAYER_HEIGHT;

    Ball ball = {0};
    ball.position.x = SCREEN_WIDTH / 2;
    ball.position.y = SCREEN_HEIGHT / 2;
    ball.speed.x = INITIAL_BALL_SPEED_X;
    ball.speed.y = INITIAL_BALL_SPEED_Y;
    ball.radius = 10;

    Rectangle topWall = {0};
    topWall.x = 0;
    topWall.y = 0;
    topWall.width = SCREEN_WIDTH;
    topWall.height = WALL_THICKNESS;
    Rectangle bottomWall = {0};
    bottomWall.x = 0;
    bottomWall.y = SCREEN_HEIGHT - WALL_THICKNESS;
    bottomWall.width = SCREEN_WIDTH;
    bottomWall.height = WALL_THICKNESS;
    Rectangle walls[NUMBER_OF_WALLS] = {topWall, bottomWall};

    int playerSpeed = 10;
    bool player2Computer = true;
    Score score = {0};
    score.player1 = 0;
    score.player2 = 0;
    score.player1Goal = false;
    score.player2Goal = false;

    Camera2D camera = {0};
    camera.target = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    Timers timers = {0.0f, 0.0f, 0.0f, 0.0f};

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        if (IsKeyPressed(KEY_ENTER))
            player2Computer = !player2Computer;

        UpdatePlayer1(&player1, playerSpeed);
        UpdatePlayer2(&player2, &ball, playerSpeed, player2Computer);
        UpdateBall(&ball, &player1, &player2, walls, &score);

        BeginDrawing();

        ClearBackground(DARKGRAY);

        DrawText("Press W/D to move up and down", PLAYER_WIDTH, WALL_THICKNESS, FONT_SIZE, GREEN);
        if (player2Computer)
            DrawText("Press Enter to control player2", SCREEN_WIDTH - 340, WALL_THICKNESS, FONT_SIZE, GREEN);
        else
        {
            DrawText("Press UP/DOWN arrows to move", SCREEN_WIDTH - 350, WALL_THICKNESS, FONT_SIZE, GREEN);
        }

        // useful for debugging what the ball is doing
        // DrawText(TextFormat("ball speed (x: %0.2f, y: %0.2f)", ball.speed.x, ball.speed.y), PLAYER_WIDTH, SCREEN_HEIGHT - FONT_SIZE - WALL_THICKNESS, FONT_SIZE, GREEN);
        DrawText(TextFormat("%i", score.player1), SCREEN_WIDTH / 16, SCREEN_HEIGHT / 9, SCORE_FONT_SIZE, GREEN);
        DrawText(TextFormat("%i", score.player2), SCREEN_WIDTH * 15 / 16, SCREEN_HEIGHT / 9, SCORE_FONT_SIZE, GREEN);

        if (score.player1 >= WINNING_SCORE || score.player2 >= WINNING_SCORE)
        {
            if (score.player1 >= WINNING_SCORE)
                timers.player1Won = MESSAGE_TIMER;
            else
                timers.player2Won = MESSAGE_TIMER;

            ResetBall(&ball, score.player1Goal);
            score.player1 = 0;
            score.player2 = 0;
            score.player1Goal = false;
            score.player2Goal = false;
        }
        else if (score.player1Goal || score.player2Goal)
        {
            if (score.player1Goal)
                timers.player1Goal = MESSAGE_TIMER;
            else
                timers.player2Goal = MESSAGE_TIMER;

            ResetBall(&ball, score.player1Goal);
            score.player1Goal = false;
            score.player2Goal = false;
        }

        BeginMode2D(camera);

        for (int i = 0; i < NUMBER_OF_WALLS; i++)
            DrawRectangleRec(walls[i], GREEN);

        DrawRectangleRec(player1, GREEN);
        DrawRectangleRec(player2, GREEN);

        DrawCircleV(ball.position, ball.radius, GREEN);

        if (timers.player1Goal > 0)
        {
            DrawText("player1 scores!", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, FONT_SIZE, GREEN);
            timers.player1Goal -= deltaTime;
        }
        else if (timers.player2Goal > 0)
        {
            DrawText("player2 scores!", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, FONT_SIZE, GREEN);
            timers.player2Goal -= deltaTime;
        }
        else if (timers.player1Won > 0)
        {
            DrawText("player1 wins!", SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2, FONT_SIZE, GREEN);
            timers.player1Won -= deltaTime;
        }
        else if (timers.player2Won > 0)
        {
            DrawText("player2 wins!", SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2, FONT_SIZE, GREEN);
            timers.player2Won -= deltaTime;
        }

        EndMode2D();

        EndDrawing();
    }
}

void ResetBall(Ball *ball, bool player1Goal)
{
    ball->position.x = SCREEN_WIDTH / 2;
    ball->position.y = SCREEN_HEIGHT / 2;
    int xDirection = player1Goal ? -1 : 1;
    ball->speed.x = xDirection * INITIAL_BALL_SPEED_X;
    ball->speed.y = INITIAL_BALL_SPEED_Y;
}

void UpdatePlayer1(Rectangle *player1, int playerSpeed)
{
    if (IsKeyDown(KEY_W) && player1->y > WALL_THICKNESS)
        player1->y -= playerSpeed;
    if (IsKeyDown(KEY_S) && player1->y < SCREEN_HEIGHT - WALL_THICKNESS - player1->height)
        player1->y += playerSpeed;
}

void UpdatePlayer2(Rectangle *player2, Ball *ball, int playerSpeed, bool player2Computer)
{
    if (player2Computer)
    {
        if (ball->position.y < SCREEN_HEIGHT - WALL_THICKNESS - player2->height / 2 && ball->position.y > player2->height / 2)
            player2->y = ball->position.y - PLAYER_HEIGHT / 2;
        // todo make platform move at uniform speed instead of basically following the speed of the ball
        // also would probably look/work better if it only moved towards the ball while the ball was coming towards it
        // when the ball is going away it should go towards SCREEN_HEIGHT / 2
    }
    else
    {
        if (IsKeyDown(KEY_UP) && player2->y > WALL_THICKNESS)
            player2->y -= playerSpeed;
        if (IsKeyDown(KEY_DOWN) && player2->y < SCREEN_HEIGHT - WALL_THICKNESS - player2->height)
            player2->y += playerSpeed;
    }
}

void UpdateBall(Ball *ball, Rectangle *player1, Rectangle *player2, Rectangle *walls, Score *score)
{
    ball->position.x += ball->speed.x;
    ball->position.y += ball->speed.y;

    Rectangle topWall = walls[0];
    Rectangle bottomWall = walls[1];
    if (ball->position.y >= bottomWall.y || ball->position.y <= topWall.height)
    {
        ball->speed.y = -ball->speed.y;
    }
    bool player1Collision = ball->position.x <= player1->width && ball->position.y >= player1->y && ball->position.y <= player1->y + player1->height;
    bool player2Collision = ball->position.x >= player2->x && ball->position.y >= player2->y && ball->position.y <= player2->y + player2->height;
    if (player1Collision || player2Collision)
    {
        ball->speed.x = -ball->speed.x;
        ball->speed.y += ball->speed.y > 0 ? 0.09 : -0.09;
        ball->speed.x += ball->speed.x > 0 ? 0.16 : -0.16;
    }
    if (ball->position.x <= 0)
    {
        score->player2++;
        score->player2Goal = true;
    }
    if (ball->position.x >= SCREEN_WIDTH)
    {
        score->player1++;
        score->player1Goal = true;
    }
}
