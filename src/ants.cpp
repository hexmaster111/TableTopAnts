#include "ants.hpp"
#include <math.h>
#include <stdio.h>

// https://stackoverflow.com/a/48693317
#define DEG_TO_RAD(angleInDegrees) \
    ((angleInDegrees) * M_PI / 180.0f)

struct FARAMONE
{

    void Draw()
    {
        DrawCircleLines(Center.x, Center.y, Strength, BLACK);
    }

    void Update()
    {
        if (InUse && Strength <= 0)
        {
            InUse = false; // mark pharamones that have blown away as free
        }

        if (InUse)
        {
            Strength -= .01; // faramone decay
        }

        if (Strength < 0)
        {
            Strength = 0;
        }
    }

    Vector2 Center;
    float Strength;
    bool InUse;
};

struct
{

    void add_faramone(float strength, Vector2 center)
    {
        for (size_t i = 0; i < 200; i++)
        {
            if (!faramone[i].InUse)
            {
                faramone[i] = (FARAMONE){center, strength, true};
                return;
            }
        }

        fprintf(stdout, "add_faramone(): No more room for faramone!\n");
    }

    void update()
    {
        for (size_t i = 0; i < 200; i++)
        {
            faramone[i].Update();
        }
    }

    void render()
    {
        for (size_t i = 0; i < 200; i++)
        {
            if (faramone[i].InUse)
            {
                faramone[i].Draw();
            }
        }
    }

    void init()
    {
        for (size_t i = 0; i < 200; i++)
        {
            faramone[i] = {0};
        }

        add_faramone(20, (Vector2){1, 1});
    }

    void ant_place_faramone(ANT ant)
    {
        add_faramone(3, (Vector2){ant.Position.x, ant.Position.y});
    }

private:
    FARAMONE faramone[200];
} faramone_global;

void ANT::Update()
{

    switch (BrainState)
    {
    case WANDER:
        // Random direction changes
        if (TimerDone(RandomDirectionChangeTimer))
        {
            StartTimer(&RandomDirectionChangeTimer, GetRandomValue(3, 10));
            Position.z = GetRandomValue(0, 360);
        }
        break;

    default:
        break;
    }

    if (TimerDone(FaramoneDropTimer))
    {
        StartTimer(&FaramoneDropTimer, 1);
        faramone_global.ant_place_faramone(*this);
    }

    Position.x += Position.w * cosf(DEG_TO_RAD(Position.z));
    Position.y += Position.w * sinf(DEG_TO_RAD(Position.z));
}

void ANT::Draw()
{
    const float rw = 5, rh = 10;
    float x = Position.x - rw / 2.0f,
          y = Position.y - rh / 2.0f;

    DrawRectanglePro(
        (Rectangle){x, y, rw, rh},
        (Vector2){rw / 2.0f, rh / 2.0f},
        Position.z - 90.0f,
        BLACK);
}

void faramone_global_render_hud()
{
}
void faramone_global_update()
{
    faramone_global.update();
}

void faramone_global_render_game()
{
    faramone_global.render();
}

void faramone_global_init()
{
    faramone_global.init();
}