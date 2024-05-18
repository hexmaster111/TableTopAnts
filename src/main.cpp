extern "C"
{
#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
}

#include "ants.hpp"

Vector2 GetMovementMatrixVector(bool u, bool d, bool l, bool r)
{
    Vector2 res = {0};

    if (u)
        res.y = -1;
    if (d)
        res.y = 1;
    if (l)
        res.x = -1;
    if (r)
        res.x = 1;

    return res;
}

int main(int argc, char **argv)
{
    InitWindow(800, 600, "Ants!");
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MAXIMIZED);
    char sbuff[50];

    Camera2D cam = {0};
    cam.zoom = 1.0f;
    cam.target = (Vector2){1, 1};
    Vector2 cam_off = {0};

    SetTargetFPS(60);

    ANT ants[5];

    for (size_t i = 0; i < 5; i++)
    {
        ANT *a = &ants[i];
        a->Position.z = static_cast<float>(GetRandomValue(0, 360));
        a->Position.w = 0.2f;
        a->Position.x = 0;
        a->Position.y = 0;
    }

    faramone_global_init();

    while (!WindowShouldClose())
    {
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f / cam.zoom);

            cam.target = Vector2Add(cam.target, delta);
        }

        float wheel = GetMouseWheelMove();
        if (wheel != 0)
        {
            // Zoom increment
            const float zoomIncrement = 0.125f;

            cam.zoom += (wheel * zoomIncrement);
            if (cam.zoom < zoomIncrement)
                cam.zoom = zoomIncrement;

            cam.zoom = Clamp(cam.zoom, 0.5f, 10.0f);
        }

        bool left = IsKeyDown(KEY_LEFT),
             right = IsKeyDown(KEY_RIGHT),
             down = IsKeyDown(KEY_DOWN),
             up = IsKeyDown(KEY_UP);

        cam.target = Vector2Add(cam.target,
                                Vector2Scale(
                                    GetMovementMatrixVector(up, down, left, right),
                                    IsKeyDown(KEY_LEFT_ALT)
                                        ? 5.0f
                                        : 2.0f)

        );

        for (size_t i = 0; i < 5; i++)
        {
            ANT *a = &ants[i];
            a->Update();
        }
        faramone_global_update();
        BeginDrawing();
        ClearBackground(WHITE);

        BeginMode2D(cam);
        rlPushMatrix();
        rlTranslatef(0, 25 * 50, 0);
        rlRotatef(90, 1, 0, 0);
        DrawGrid(100, 50);
        rlPopMatrix();

        for (size_t i = 0; i < 5; i++)
        {
            ANT *a = &ants[i];
            a->Draw();
        }

        faramone_global_render_game();

        EndMode2D();
        // hud
        faramone_global_render_hud();
        GuiSlider((Rectangle){10, 10, 200, 16}, "", "Rotation", &ants[0].Position.z, 0, 360);
        GuiSlider((Rectangle){10, 10 + 16 + 2, 200, 16}, "", " Speed  ", &ants[0].Position.w, 0, 5);
        EndDrawing();
    }

    CloseWindow();
}