#include "raylib.h"

struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame)
{
    // update running time
    data.runningTime += deltaTime;
    if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        // update anim frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if (data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }
    return data;
}

int main()
{
    // window dimensions
    int windowDimensions[2];
    windowDimensions[0] = 512; // windowwidth
    windowDimensions[1] = 250; // windowheight

    InitWindow(windowDimensions[0], windowDimensions[1], "Dasher");

    // acceleration due to gravity (pixels/sec)/sec
    const int gravity{1'000};
    // nebula texture
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    // nebula array and loop
    const int sizeOfNebulae{10};
    AnimData nebulae[sizeOfNebulae]{};

    for (int i = 0; i < sizeOfNebulae; i++)
    {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width / 8;
        nebulae[i].rec.height = nebula.height / 8;
        nebulae[i].pos.y = windowDimensions[1] - nebula.height / 8;
        nebulae[i].frame = 0;
        nebulae[i].updateTime = 0.0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].pos.x = windowDimensions[0] + (i * 300);
    }

    float finishLine{nebulae[sizeOfNebulae - 1].pos.x};

    // dasher textures
    Texture2D dasher = LoadTexture("textures/dasher.png");
    AnimData dasherData;
    // Rectangle
    dasherData.rec.x = 0.0;
    dasherData.rec.y = 0.0;
    dasherData.rec.width = dasher.width / 6;
    dasherData.rec.height = dasher.height;
    // Vector2
    dasherData.pos.x = windowDimensions[0] / 2 - dasherData.rec.width / 2;
    dasherData.pos.y = windowDimensions[1] - dasherData.rec.height;
    // frame, update and running Time
    dasherData.frame = 0;
    dasherData.updateTime = 1.0 / 12.0;
    dasherData.runningTime = 0.0;

    // dasher jump
    Texture2D dasherJump = LoadTexture("textures/dasher_jump.png");
    AnimData jumpData;
    jumpData.rec.x = 0.0;
    jumpData.rec.y = 0.0;
    jumpData.rec.width = dasherJump.width / 4;
    jumpData.rec.height = dasherJump.height;
    jumpData.frame = 0;
    jumpData.updateTime = 1.0 / 6.0;
    jumpData.frame = 0.0;

    // velocity
    int velocity{0};
    const int jumpVel{-600}; // pixels per second

    const int dashVel{100};

    // nebula x velocity pps
    const int nebVel{-200};

    // is the rect in the air?
    bool isInAir{};

    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgX{};

    Texture2D midground = LoadTexture("textures/back-buildings.png");
    float mgX{};

    Texture2D foreground = LoadTexture("textures/foreground.png");
    float fgX{};

    bool collision{};

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        // deltaTime (time since last frame)
        const float dT{GetFrameTime()};

        BeginDrawing();
        ClearBackground(GRAY);

        // scroll background
        bgX -= 20 * dT; // update position and delta time, don't forget!
        if (bgX <= -background.width * 2)
        {
            bgX = 0.0;
        }

        // scroll midground
        mgX -= 40 * dT; // update position and delta time, don't forget!
        if (mgX <= -midground.width * 2)
        {
            mgX = 0.0;
        }

        // scroll foreground
        fgX -= 80 * dT; // update position and delta time, don't forget!
        if (fgX <= -foreground.width * 2)
        {
            fgX = 0.0;
        }

        // draw background
        Vector2 bg1Pos{bgX, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE);
        Vector2 bg2Pos{bgX + background.width * 2, 0.0};
        DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);

        // draw midground
        Vector2 mg1Pos{mgX, 0.0};
        DrawTextureEx(midground, mg1Pos, 0.0, 2.0, WHITE);
        Vector2 mg2Pos{mgX + midground.width * 2, 0.0};
        DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);

        // draw foreground
        Vector2 fg1Pos{fgX, 0.0};
        DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
        Vector2 fg2Pos{fgX + foreground.width * 2, 0.0};
        DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);

        // perform ground check
        if (isOnGround(dasherData, windowDimensions[1]))
        {
            // rect is on ground
            dasherData.pos.y = windowDimensions[1] - dasherData.rec.height;
            velocity = 0;
            isInAir = false;
        }
        else
        {
            // rect is in air, apply gravity
            // multiply velocity when applying gravity
            velocity += gravity * dT;
            isInAir = true;
        }

        // perform jump check
        if (IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            velocity += jumpVel;
        }

        if (IsKeyDown(KEY_Q) && isInAir)
        {
            dasherData.pos.x += dashVel;
            dasherData.pos.x = windowDimensions[0] / 2 - dasherData.rec.width / 2;
        }

        for (int i = 0; i < sizeOfNebulae; i++)
        {
            // update each nebula position
            nebulae[i].pos.x += nebVel * dT;
        }
        // update finish line
        finishLine += nebVel * dT;

        // update Y position of dasher and dashJump
        // multiply dT when  applying velocity
        dasherData.pos.y += velocity * dT;

        // dasher update animframe
        if (!isInAir)
        {
            dasherData = updateAnimData(dasherData, dT, 5);
        }
        else
        {
            jumpData = updateAnimData(jumpData, dT, 3);
        }

        // nebulae update anim frame
        for (int i = 0; i < sizeOfNebulae; i++)
        {
            nebulae[i] = updateAnimData(nebulae[i], dT, 7);
        }

        for (AnimData nebula : nebulae)
        { // check for collision
            float pad{20};
            Rectangle nebRec{
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2 * pad,
                nebula.rec.height - 2 * pad,
            };
            Rectangle dasherRec{
                dasherData.pos.x,
                dasherData.pos.y + pad,
                dasherData.rec.width - 2 * pad,
                dasherData.rec.height - 2 * pad,
            };
            if (CheckCollisionRecs(nebRec, dasherRec))
            {
                collision = true;
            }
        }

        if (collision)
        { // you lose
            DrawText("Game Over!", windowDimensions[0] / 4, windowDimensions[1] / 2, 20, RED);
        }
        else if (dasherData.pos.x >= finishLine)
        {
            DrawText("YOU WIN!", windowDimensions[0] / 4, windowDimensions[1] / 2, 20, RED);
        }
        else
        {
            for (int i = 0; i < sizeOfNebulae; i++)
            {
                // draw nebula
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }
            // dasher jump
            if (isInAir)
            {
                DrawTextureRec(dasherJump, jumpData.rec, dasherData.pos, WHITE);
            }
            else
            {
                // draw dasher
                DrawTextureRec(dasher, dasherData.rec, dasherData.pos, WHITE);
            }
        }

        EndDrawing();
    }
    UnloadTexture(nebula);
    UnloadTexture(dasherJump);
    UnloadTexture(dasher);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();
}