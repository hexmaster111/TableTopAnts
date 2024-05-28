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

const int screen_width = 800, screen_height = 600;

int main(int argc, char **argv)
{
    const int ant_count = 1;

    bool pause = false;

    InitWindow(screen_width, screen_height, "Ants!");
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MAXIMIZED);
    char sbuff[50];

    Camera2D cam = {0};
    cam.zoom = 1.0f;
    cam.target = (Vector2){0, 0};
    cam.offset = (Vector2){screen_width / 2, screen_height / 2};

    SetTargetFPS(60);

    ANT ants[ant_count];

    for (size_t i = 0; i < ant_count; i++)
    {
        ANT *a = &ants[i];
        a->Position.z = static_cast<float>(GetRandomValue(0, 360));
        a->Position.w = 0.2f;
        a->Position.x = -100 + GetRandomValue(-50, 50);
        a->Position.y = -100 + GetRandomValue(-50, 50);
        a->BrainState = ABS_BEGIN_WANDER;
        a->StomachFullness = 0.0;
    }

    faramone_global_init();
    global_init();

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
                                    IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)
                                        ? 5.0f
                                        : 2.0f)

        );

        if (!pause)
        {

            for (size_t i = 0; i < ant_count; i++)
            {
                ANT *a = &ants[i];
                a->Update();
            }
            faramone_global_update();
        }

        global_update();
        BeginDrawing();
        ClearBackground(WHITE);

        BeginMode2D(cam);
        rlPushMatrix();
        rlTranslatef(0, 25 * 50, 0);
        rlRotatef(90, 1, 0, 0);
        DrawGrid(100, 50);
        rlPopMatrix();

        faramone_global_render_game();
        global_render_game();
        for (size_t i = 0; i < ant_count; i++)
        {
            ANT *a = &ants[i];
            a->Draw();
        }

        EndMode2D();
        // hud
        faramone_global_render_hud();
        GuiSlider((Rectangle){10, 10, 200, 16}, "", "Rotation", &ants[0].Position.z, -1024, 1024);
        GuiSlider((Rectangle){10, 10 + 16, 200, 16}, "", " Speed  ", &ants[0].Position.w, 0, 5);
        GuiSlider((Rectangle){10, 10 + 16 + 16, 200, 16}, "", " Fullness  ", &ants[0].StomachFullness, 0, 1);

        GuiCheckBox((Rectangle){10, 60, 100, 16}, "", &ants[0].is_left_antina_touching_faramone);
        GuiCheckBox((Rectangle){110, 60, 100, 16}, "LFaramone | RFaramone", &ants[0].is_right_antina_touching_faramone);

        GuiCheckBox((Rectangle){10, 60 + 16, 100, 16}, "", &ants[0].isLeftAntiTouchingFood);
        GuiCheckBox((Rectangle){110, 60 + 16, 100, 16}, "LFood | RFood", &ants[0].isRightAntiTouchingFood);

        GuiCheckBox((Rectangle){10, 60 + 16 + 16, 100, 16}, "", &ants[0].is_full);
        GuiCheckBox((Rectangle){110, 60 + 16 + 16, 100, 16}, "Is Full | Mouth Touching Food", &ants[0].is_mouth_touching_food);
        GuiCheckBox((Rectangle){10, 60 + 16 + 16 + 16, 16, 16}, "Pause", &pause);

        DrawText(TextFormat("Brain State: %s", ToString(ants[0].BrainState)), 10, 60 + 96, 16, BLACK);

        if (ants[0].vi_eye_target_center.item != VIS_NOTHING)
            DrawText(TextFormat("CF EYE:%.1f ,%s", ants[0].vi_eye_target_center.distance, ToString(ants[0].vi_eye_target_center.item)), 10, 60 + 96 + 16, 16, BLACK);
        if (ants[0].vi_eye_target_l0.item != VIS_NOTHING)
            DrawText(TextFormat("l0 EYE:%.1f ,%s", ants[0].vi_eye_target_l0.distance, ToString(ants[0].vi_eye_target_l0.item)), 10, 60 + 96 + 16 + 16, 16, BLACK);
        if (ants[0].vi_eye_target_r0.item != VIS_NOTHING)
            DrawText(TextFormat("r0 EYE:%.1f ,%s", ants[0].vi_eye_target_r0.distance, ToString(ants[0].vi_eye_target_r0.item)), 10, 60 + 96 + 16 + 16 + 16, 16, BLACK);
        if (ants[0].vi_eye_target_l1.item != VIS_NOTHING)
            DrawText(TextFormat("l1 EYE:%.1f ,%s", ants[0].vi_eye_target_l1.distance, ToString(ants[0].vi_eye_target_l1.item)), 10, 60 + 96 + 16 + 16 + 16 + 16, 16, BLACK);
        if (ants[0].vi_eye_target_r1.item != VIS_NOTHING)
            DrawText(TextFormat("r1 EYE:%.1f ,%s", ants[0].vi_eye_target_r1.distance, ToString(ants[0].vi_eye_target_r1.item)), 10, 60 + 96 + 16 + 16 + 16 + 16 + 16, 16, BLACK);
        if (ants[0].vi_eye_target_l2.item != VIS_NOTHING)
            DrawText(TextFormat("l2 EYE:%.1f ,%s", ants[0].vi_eye_target_l2.distance, ToString(ants[0].vi_eye_target_l2.item)), 10, 60 + 96 + 16 + 16 + 16 + 16 + 16 + 16, 16, BLACK);
        if (ants[0].vi_eye_target_r2.item != VIS_NOTHING)
            DrawText(TextFormat("r2 EYE:%.1f ,%s", ants[0].vi_eye_target_r2.distance, ToString(ants[0].vi_eye_target_r2.item)), 10, 60 + 96 + 16 + 16 + 16 + 16 + 16 + 16 + 16, 16, BLACK);

        EndDrawing();
    }

    CloseWindow();
}