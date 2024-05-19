#include "ants.hpp"
#include <math.h>
#include <stdio.h>

// https://stackoverflow.com/a/48693317
#define DEG_TO_RAD(angleInDegrees) \
    ((angleInDegrees) * M_PI / 180.0f)

#define ANTINA_CHECK_RAIDUS (2.0f)
#define ANT_RW (5.0f)
#define ANT_RH (10.0f)

struct FARAMONE
{

    void Draw()
    {
        DrawCircleLines(Center.x, Center.y, Strength, BLACK);
    }

    bool HitTest(Vector2 pos, float radius)
    {
        return CheckCollisionCircles(pos, radius, Center, Strength);
    }

    void Update()
    {
        if (InUse && Strength <= 0)
        {
            InUse = false; // mark pharamones that have blown away as free
        }

        if (InUse)
        {
            Strength -= .001; // faramone decay
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

#define faramone_count 1024
struct
{
    void add_faramone(float strength, Vector2 center)
    {
        for (size_t i = 0; i < faramone_count; i++)
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
        for (size_t i = 0; i < faramone_count; i++)
        {
            faramone[i].Update();
        }
    }

    void render()
    {
        for (size_t i = 0; i < faramone_count; i++)
        {
            if (faramone[i].InUse)
            {
                faramone[i].Draw();
            }
        }
    }

    void init()
    {
        for (size_t i = 0; i < faramone_count; i++)
        {
            faramone[i] = {0};
        }
    }

    bool ant_place_faramone(ANT ant, Vector2 pos)
    {
        add_faramone(3, pos);
    }

    bool FaramoneTestRight(ANT ant)
    {
        return Test(ant, ant.RightAntinaHitCircle);
    }

    bool FaramoneTestLeft(ANT ant)
    {
        return Test(ant, ant.LeftAntinaHitCircle);
    }

private:
    bool Test(ANT ant, Vector2 circle)
    {
        bool hit = false;

        for (size_t i = 0; i < faramone_count; i++)
        {
            if (hit = CheckCollisionCircles(
                    faramone[i].Center,
                    faramone[i].Strength,
                    circle,
                    ANTINA_CHECK_RAIDUS))
            {
                hit = true;
                break;
            }
        }

        return hit;
    }

    FARAMONE faramone[faramone_count];
} faramone_global;

void ANT::Update()
{
    left_antina = faramone_global.FaramoneTestLeft(*this);
    right_antina = faramone_global.FaramoneTestRight(*this);

    switch (BrainState)
    {
    case WANDER:

        // we smell something
        if (left_antina || right_antina)
        {
            BrainState = TRACK;
        }

        // Random direction changes
        if (TimerDone(RandomDirectionChangeTimer))
        {
            StartTimer(&RandomDirectionChangeTimer, GetRandomValue(3, 10));
            Position.z = GetRandomValue(0, 360);
        }

        break;

    case TRACK:
        break;

    default:
        break;
    }

    Position.x += Position.w * cosf(DEG_TO_RAD(Position.z));
    Position.y += Position.w * sinf(DEG_TO_RAD(Position.z));

    // antina hitbox calulations, based on DrawRectanglePro impl
    Vector2 topLeft = {0};
    Vector2 topRight = {0};
    Vector2 bottomLeft = {0};
    Vector2 bottomRight = {0};

    float rotation = Position.z + 90.0f,
          rw = ANT_RW,
          rh = ANT_RH,
          x = Position.x - rw / 2.0f,
          y = Position.y - rh / 2.0f;

    Rectangle rec = {x, y, rw, rh};
    Vector2 origin = (Vector2){rw / 2.0f, rh / 2.0f};

    float sinRotation = sinf(rotation * DEG2RAD);
    float cosRotation = cosf(rotation * DEG2RAD);
    float dx = -origin.x;
    float dy = -origin.y;

    topLeft.x = x + dx * cosRotation - dy * sinRotation;
    topLeft.y = y + dx * sinRotation + dy * cosRotation;

    topRight.x = x + (dx + rec.width) * cosRotation - dy * sinRotation;
    topRight.y = y + (dx + rec.width) * sinRotation + dy * cosRotation;

    bottomLeft.x = x + dx * cosRotation - (dy + rec.height) * sinRotation;
    bottomLeft.y = y + dx * sinRotation + (dy + rec.height) * cosRotation;

    bottomRight.x = x + (dx + rec.width) * cosRotation - (dy + rec.height) * sinRotation;
    bottomRight.y = y + (dx + rec.width) * sinRotation + (dy + rec.height) * cosRotation;

    LeftAntinaHitCircle = topLeft;
    RightAntinaHitCircle = topRight;

    if (TimerDone(FaramoneDropTimer))
    {
        StartTimer(&FaramoneDropTimer, 1.0);
        faramone_global.ant_place_faramone(*this, bottomLeft);
    }
}

void ANT::Draw()
{
    const float rw = ANT_RW, rh = ANT_RH;
    float x = Position.x - rw / 2.0f,
          y = Position.y - rh / 2.0f;

    DrawRectanglePro(
        (Rectangle){x, y, rw, rh},
        (Vector2){rw / 2.0f, rh / 2.0f},
        Position.z - 90.0f,
        BLACK);

    if (left_antina)
    {
        DrawCircle(
            LeftAntinaHitCircle.x,
            LeftAntinaHitCircle.y,
            ANTINA_CHECK_RAIDUS, // ant antian check raidus
            BLACK);
    }
    else
    {
        DrawCircleLines(
            LeftAntinaHitCircle.x,
            LeftAntinaHitCircle.y,
            ANTINA_CHECK_RAIDUS, // ant antian check raidus
            BLACK);
    }

    if (right_antina)
    {
        DrawCircle(
            RightAntinaHitCircle.x,
            RightAntinaHitCircle.y,
            ANTINA_CHECK_RAIDUS, // ant antian check raidus
            BLACK);
    }
    else
    {
        DrawCircleLines(
            RightAntinaHitCircle.x,
            RightAntinaHitCircle.y,
            ANTINA_CHECK_RAIDUS, // ant antian check raidus
            BLACK);
    }
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