/*  ********************************************************************************************
 *
 *   Adapted from https://github.com/raysan5/raylib/blob/master/examples/core/core_2d_camera_split_screen.c
 *   can now dynamically add and remove players (up to 4) automatically resizes screens and show/hide players.
 *
 *   *******************************************************************************************/

#include "raylib.h"
#include "./math.h"

#define PLAYER_SIZE 40
#define FONT_SIZE 20.0f
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

void UpdatePlayer1(Rectangle *player1, Rectangle *player2, Rectangle *player3, Rectangle *player4, RenderTexture *screenCamera1, Camera2D *camera1, int playerCount);
void UpdatePlayer2(Rectangle *player1, Rectangle *player2, Rectangle *player3, Rectangle *player4, RenderTexture *screenCamera2, Camera2D *camera2, int playerCount);
void UpdatePlayer3(Rectangle *player1, Rectangle *player2, Rectangle *player3, Rectangle *player4, RenderTexture *screenCamera3, Camera2D *camera3, int playerCount);
void UpdatePlayer4(Rectangle *player1, Rectangle *player2, Rectangle *player3, Rectangle *player4, RenderTexture *screenCamera4, Camera2D *camera4, int playerCount);
void DrawScene();
RenderTexture UpdateScreenCamera1(RenderTexture *screenCamera1, int playerCount);
RenderTexture UpdateScreenCamera2(RenderTexture *screenCamera2, int playerCount);
RenderTexture UpdateScreenCamera3(RenderTexture *screenCamera3, int playerCount);
void UpdateSplitScreenRectTop(Rectangle *splitScreenRectTop, RenderTexture *screenCamera1);
void UpdateSplitScreenRectBottom(Rectangle *splitScreenRectBottom, RenderTexture *screenCamera3);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int split_screen_run(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib [core] example - 2d camera split screen");

    Rectangle player1 = {200, 200, PLAYER_SIZE, PLAYER_SIZE};
    Rectangle player2 = {250, 200, PLAYER_SIZE, PLAYER_SIZE};
    Rectangle player3 = {200, 250, PLAYER_SIZE, PLAYER_SIZE};
    Rectangle player4 = {250, 250, PLAYER_SIZE, PLAYER_SIZE};

    int playerCount = 2;
    bool resizeNeeded = false;

    Camera2D camera1 = {0};
    camera1.target = (Vector2){player1.x, player1.y};
    camera1.offset = (Vector2){200.0f, 200.0f};
    camera1.rotation = 0.0f;
    camera1.zoom = 1.0f;

    Camera2D camera2 = {0};
    camera2.target = (Vector2){player2.x, player2.y};
    camera2.offset = (Vector2){200.0f, 200.0f};
    camera2.rotation = 0.0f;
    camera2.zoom = 1.0f;

    Camera2D camera3 = {0};
    camera3.target = (Vector2){player3.x, player3.y};
    camera3.offset = (Vector2){200.0f, 200.0f};
    camera3.rotation = 0.0f;
    camera3.zoom = 1.0f;

    Camera2D camera4 = {0};
    camera4.target = (Vector2){player4.x, player4.y};
    camera4.offset = (Vector2){200.0f, 200.0f};
    camera4.rotation = 0.0f;
    camera4.zoom = 1.0f;

    RenderTexture screenCamera1 = LoadRenderTexture(SCREEN_WIDTH / 2, SCREEN_HEIGHT);
    RenderTexture screenCamera2 = LoadRenderTexture(SCREEN_WIDTH / 2, SCREEN_HEIGHT);
    RenderTexture screenCamera3 = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT / 2);
    RenderTexture screenCamera4 = LoadRenderTexture(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

    // Build a flipped rectangle the size of the split view to use for drawing later
    Rectangle splitScreenRectTop = {0.0f, 0.0f, (float)screenCamera1.texture.width, (float)-screenCamera1.texture.height};
    Rectangle splitScreenRectBottom = {0.0f, (float)SCREEN_HEIGHT / 2, (float)screenCamera4.texture.width, (float)-screenCamera4.texture.height};

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        if (IsKeyPressed(KEY_M))
        {
            playerCount = min(playerCount + 1, 4);
            resizeNeeded = true;
        }
        if (IsKeyPressed(KEY_N))
        {
            playerCount = max(playerCount - 1, 1);
            resizeNeeded = true;
        }

        if (resizeNeeded)
        {
            screenCamera1 = UpdateScreenCamera1(&screenCamera1, playerCount);
            if (playerCount > 1)
                screenCamera2 = UpdateScreenCamera2(&screenCamera2, playerCount);
            if (playerCount > 2)
                screenCamera3 = UpdateScreenCamera3(&screenCamera3, playerCount);
            UpdateSplitScreenRectTop(&splitScreenRectTop, &screenCamera1);
            if (playerCount > 2)
                UpdateSplitScreenRectBottom(&splitScreenRectBottom, &screenCamera3);
            resizeNeeded = false;
        }

        UpdatePlayer1(&player1, &player2, &player3, &player4, &screenCamera1, &camera1, playerCount);
        if (playerCount > 1)
            UpdatePlayer2(&player1, &player2, &player3, &player4, &screenCamera2, &camera2, playerCount);
        if (playerCount > 2)
            UpdatePlayer3(&player1, &player2, &player3, &player4, &screenCamera3, &camera3, playerCount);
        if (playerCount > 3)
            UpdatePlayer4(&player1, &player2, &player3, &player4, &screenCamera4, &camera4, playerCount);

        // Draw both views render textures to the screen side by side
        BeginDrawing();
        ClearBackground(BLACK);

        DrawTextureRec(screenCamera1.texture, splitScreenRectTop, (Vector2){0, 0}, WHITE);
        if (playerCount > 1)
            DrawTextureRec(screenCamera2.texture, splitScreenRectTop, (Vector2){SCREEN_WIDTH / 2.0f, 0}, WHITE);
        if (playerCount > 2)
            DrawTextureRec(screenCamera3.texture, splitScreenRectBottom, (Vector2){0, SCREEN_HEIGHT / 2.0f}, WHITE);
        if (playerCount > 3)
            DrawTextureRec(screenCamera4.texture, splitScreenRectBottom, (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f}, WHITE);

        DrawRectangle(GetScreenWidth() / 2 - 2, 0, 4, splitScreenRectTop.height, LIGHTGRAY);
        EndDrawing();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadRenderTexture(screenCamera1); // Unload render texture
    UnloadRenderTexture(screenCamera2); // Unload render texture
    UnloadRenderTexture(screenCamera3); // Unload render texture
    UnloadRenderTexture(screenCamera4); // Unload render texture

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void UpdatePlayer1(Rectangle *player1, Rectangle *player2, Rectangle *player3, Rectangle *player4, RenderTexture *screenCamera1, Camera2D *camera1, int playerCount)
{
    if (IsKeyDown(KEY_S))
        player1->y += 3.0f;
    else if (IsKeyDown(KEY_W))
        player1->y -= 3.0f;
    if (IsKeyDown(KEY_D))
        player1->x += 3.0f;
    else if (IsKeyDown(KEY_A))
        player1->x -= 3.0f;

    camera1->target = (Vector2){player1->x, player1->y};
    camera1->offset = (Vector2){screenCamera1->texture.width / 2.0f, screenCamera1->texture.height / 2.0f};

    BeginTextureMode(*screenCamera1);
    ClearBackground(RAYWHITE);

    BeginMode2D(*camera1);

    DrawScene();

    if (playerCount > 1)
        DrawRectangleRec(*player2, BLUE);
    if (playerCount > 2)
        DrawRectangleRec(*player3, GREEN);
    if (playerCount > 3)
        DrawRectangleRec(*player4, YELLOW);
    // draw player1 above others in this camera
    DrawRectangleRec(*player1, RED);

    EndMode2D();

    int rectWidth = (playerCount > 1) ? GetScreenWidth() / 2 : GetScreenWidth();
    DrawRectangle(0, 0, rectWidth, 30, Fade(RAYWHITE, 0.6f));
    DrawText("PLAYER1: W/S/A/D to move", 10, 10, FONT_SIZE, MAROON);
    DrawText(TextFormat("player count %i", playerCount), 10, 30, FONT_SIZE, MAROON);

    EndTextureMode();
}

void UpdatePlayer2(Rectangle *player1, Rectangle *player2, Rectangle *player3, Rectangle *player4, RenderTexture *screenCamera2, Camera2D *camera2, int playerCount)
{
    if (IsKeyDown(KEY_UP))
        player2->y -= 3.0f;
    else if (IsKeyDown(KEY_DOWN))
        player2->y += 3.0f;
    if (IsKeyDown(KEY_RIGHT))
        player2->x += 3.0f;
    else if (IsKeyDown(KEY_LEFT))
        player2->x -= 3.0f;

    camera2->target = (Vector2){player2->x, player2->y};
    camera2->offset = (Vector2){screenCamera2->texture.width / 2.0f, screenCamera2->texture.height / 2.0f};

    BeginTextureMode(*screenCamera2);
    ClearBackground(RAYWHITE);

    BeginMode2D(*camera2);

    DrawScene();

    DrawRectangleRec(*player1, RED);
    if (playerCount > 2)
        DrawRectangleRec(*player3, GREEN);
    if (playerCount > 3)
        DrawRectangleRec(*player4, YELLOW);
    // draw player2 above others in this camera
    DrawRectangleRec(*player2, BLUE);

    EndMode2D();

    DrawRectangle(0, 0, GetScreenWidth() / 2, 30, Fade(RAYWHITE, 0.6f));
    DrawText("PLAYER2: UP/DOWN/LEFT/RIGHT to move", 10, 10, FONT_SIZE, DARKBLUE);

    EndTextureMode();
}

void UpdatePlayer3(Rectangle *player1, Rectangle *player2, Rectangle *player3, Rectangle *player4, RenderTexture *screenCamera3, Camera2D *camera3, int playerCount)
{
    if (IsKeyDown(KEY_K))
        player3->y -= 3.0f;
    else if (IsKeyDown(KEY_J))
        player3->y += 3.0f;
    if (IsKeyDown(KEY_L))
        player3->x += 3.0f;
    else if (IsKeyDown(KEY_H))
        player3->x -= 3.0f;

    camera3->target = (Vector2){player3->x, player3->y};
    camera3->offset = (Vector2){screenCamera3->texture.width / 2.0f, screenCamera3->texture.height / 2.0f};

    BeginTextureMode(*screenCamera3);
    ClearBackground(RAYWHITE);

    BeginMode2D(*camera3);

    DrawScene();

    DrawRectangleRec(*player1, RED);
    DrawRectangleRec(*player2, BLUE);
    if (playerCount > 3)
        DrawRectangleRec(*player4, YELLOW);
    // draw player3 above others in this camera
    DrawRectangleRec(*player3, GREEN);

    EndMode2D();

    int rectWidth = (playerCount > 3) ? GetScreenWidth() / 2 : GetScreenWidth();
    DrawRectangle(0, 0, rectWidth, 30, Fade(RAYWHITE, 0.6f));
    DrawText("PLAYER3: H/J/K/L to move", 10, 10, FONT_SIZE, DARKGREEN);

    EndTextureMode();
}

void UpdatePlayer4(Rectangle *player1, Rectangle *player2, Rectangle *player3, Rectangle *player4, RenderTexture *screenCamera4, Camera2D *camera4, int playerCount)
{
    if (IsKeyDown(KEY_C))
        player4->y -= 3.0f;
    else if (IsKeyDown(KEY_X))
        player4->y += 3.0f;
    if (IsKeyDown(KEY_V))
        player4->x += 3.0f;
    else if (IsKeyDown(KEY_Z))
        player4->x -= 3.0f;

    camera4->target = (Vector2){player4->x, player4->y};
    camera4->offset = (Vector2){screenCamera4->texture.width / 2.0f, screenCamera4->texture.height / 2.0f};

    BeginTextureMode(*screenCamera4);
    ClearBackground(RAYWHITE);

    BeginMode2D(*camera4);

    DrawScene();

    DrawRectangleRec(*player1, RED);
    DrawRectangleRec(*player2, BLUE);
    DrawRectangleRec(*player3, GREEN);
    // draw player4 above others in this camera
    DrawRectangleRec(*player4, YELLOW);

    EndMode2D();

    DrawRectangle(0, 0, GetScreenWidth() / 2, 30, Fade(RAYWHITE, 0.6f));
    DrawText("PLAYER4: Z/X/C/V to move", 10, 10, FONT_SIZE, YELLOW);

    EndTextureMode();
}

void DrawScene()
{
    // Draw full scene with second camera
    for (int i = 0; i < SCREEN_WIDTH / PLAYER_SIZE + 1; i++)
    {
        DrawLineV((Vector2){(float)PLAYER_SIZE * i, 0}, (Vector2){(float)PLAYER_SIZE * i, (float)SCREEN_HEIGHT}, LIGHTGRAY);
    }

    for (int i = 0; i < SCREEN_HEIGHT / PLAYER_SIZE + 1; i++)
    {
        DrawLineV((Vector2){0, (float)PLAYER_SIZE * i}, (Vector2){(float)SCREEN_WIDTH, (float)PLAYER_SIZE * i}, LIGHTGRAY);
    }

    for (int i = 0; i < SCREEN_WIDTH / PLAYER_SIZE; i++)
    {
        for (int j = 0; j < SCREEN_HEIGHT / PLAYER_SIZE; j++)
        {
            DrawText(TextFormat("[%i,%i]", i, j), 10 + PLAYER_SIZE * i, 15 + PLAYER_SIZE * j, 10, LIGHTGRAY);
        }
    }
}

RenderTexture UpdateScreenCamera1(RenderTexture *screenCamera1, int playerCount)
{
    UnloadRenderTexture(*screenCamera1);

    int width;
    int height;
    switch (playerCount)
    {
    case 1:
        width = SCREEN_WIDTH;
        height = SCREEN_HEIGHT;
        break;
    case 2:
        width = SCREEN_WIDTH / 2;
        height = SCREEN_HEIGHT;
        break;
    case 3:
        width = SCREEN_WIDTH / 2;
        height = SCREEN_HEIGHT / 2;
        break;
    case 4:
        width = SCREEN_WIDTH / 2;
        height = SCREEN_HEIGHT / 2;
        break;
    }

    return LoadRenderTexture(width, height);
}

RenderTexture UpdateScreenCamera2(RenderTexture *screenCamera2, int playerCount)
{
    UnloadRenderTexture(*screenCamera2);

    int width;
    int height;
    switch (playerCount)
    {
    case 2:
        width = SCREEN_WIDTH / 2;
        height = SCREEN_HEIGHT;
        break;
    case 3:
        width = SCREEN_WIDTH / 2;
        height = SCREEN_HEIGHT / 2;
        break;
    case 4:
        width = SCREEN_WIDTH / 2;
        height = SCREEN_HEIGHT / 2;
        break;
    }

    return LoadRenderTexture(width, height);
}

RenderTexture UpdateScreenCamera3(RenderTexture *screenCamera3, int playerCount)
{
    UnloadRenderTexture(*screenCamera3);

    int width;
    int height;
    switch (playerCount)
    {
    case 3:
        width = SCREEN_WIDTH;
        height = SCREEN_HEIGHT / 2;
        break;
    case 4:
        width = SCREEN_WIDTH / 2;
        height = SCREEN_HEIGHT / 2;
        break;
    }

    return LoadRenderTexture(width, height);
}

void UpdateSplitScreenRectTop(Rectangle *splitScreenRectTop, RenderTexture *screenCamera1)
{
    splitScreenRectTop->width = (float)screenCamera1->texture.width;
    splitScreenRectTop->height = (float)-screenCamera1->texture.height;
}

void UpdateSplitScreenRectBottom(Rectangle *splitScreenRectBottom, RenderTexture *screenCamera3)
{
    splitScreenRectBottom->width = (float)screenCamera3->texture.width;
    splitScreenRectBottom->height = (float)-screenCamera3->texture.height;
}