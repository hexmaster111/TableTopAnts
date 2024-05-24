#include "ants.hpp"
#include <math.h>
#include <stdio.h>
#include <assert.h>

// https://stackoverflow.com/a/48693317
#define DEG_TO_RAD(angleInDegrees) \
    ((angleInDegrees) * M_PI / 180.0f)

#define ANTINA_CHECK_RAIDUS (2.0f)
#define FOOD_CHECK_RAIDUS (2.0f)
#define ANT_RW (10.0f)
#define ANT_RH (10.0f)

#define ANT_TRACK_SPEED (.15)
#define ANT_WANDER_SPEED (.2)
#define ANT_WANDER_FULL (.1)
#define ANT_STOMACH_FULLNESS (1.0)

#define faramone_count 5000
#define food_count 5000

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
            Strength -= .0005; // faramone decay
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

struct FOOD
{

    void Draw()
    {
        DrawCircle(Center.x, Center.y, Strength, RED);
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

        if (Strength < 0)
        {
            Strength = 0;
        }
    }

    Vector2 Center;
    float Strength;
    bool InUse;
};

struct ANT_HILL
{
private:
    Vector2 Center;
};

struct FARAMONE_GLOBAL_STRUCT
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

    void ant_place_faramone(ANT ant, Vector2 pos)
    {
        add_faramone(3, pos);
    }

    bool TestRight(ANT ant)
    {
        return Test(ant, ant.top_right);
    }

    bool TestLeft(ANT ant)
    {
        return Test(ant, ant.top_left);
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

struct FOOD_GLOBAL_STRUCT
{

    void add(Vector2 center, float strength)
    {
        for (size_t i = 0; i < food_count; i++)
        {
            if (!food[i].InUse)
            {
                food[i] = (FOOD){center, strength, true};
                return;
            }
        }

        fprintf(stdout, "FOOD_GLOBAL_STRUCT::add(): No more room for food!\n");
    }

    void init()
    {
        for (size_t i = 0; i < food_count; i++)
        {
            food[i] = {0};
        }
    }

    void update()
    {
        for (size_t i = 0; i < food_count; i++)
        {
            food[i].Update();
        }
    }

    void render()
    {
        for (size_t i = 0; i < faramone_count; i++)
        {
            if (food[i].InUse)
            {
                food[i].Draw();
            }
        }
    }

    bool TestFoodOnMouth(ANT ant)
    {
        return Test(ant.mouth_location, FOOD_CHECK_RAIDUS);
    }

    void feed_ant(ANT *ant)
    {
        assert(ant != NULL);

        if (!TestFoodOnMouth(*ant)) // no food to feed the ant!
            return;

        FOOD *f = GetFoodTouching(*ant);

        assert(f != NULL);
        ant->StomachFullness += .05;
        f->Strength -= .05;
    }

    /// @return FOOD* or NULL
    FOOD *GetFoodTouching(ANT ant)
    {
        FOOD *retfood = NULL;
        for (size_t i = 0; i < food_count; i++)
        {
            if (!food[i].InUse)
                continue;

            if (food[i].HitTest(ant.mouth_location, FOOD_CHECK_RAIDUS))
            {
                retfood = &food[i];
                break;
            }
        }

        return retfood;
    }

    bool Test(Vector2 circle, float raidus)
    {
        bool hit = false;

        for (size_t i = 0; i < food_count; i++)
        {
            if (!food[i].InUse)
                continue;

            if (food[i].HitTest(circle, raidus))
            {
                hit = true;
                break;
            }
        }

        return hit;
    }

private:
    FOOD food[food_count];
} food_global;

void food_global_init()
{
    food_global.init();
    food_global.add((Vector2){10, 10}, 10);
}
void food_global_render_game() { food_global.render(); }
void food_global_update() { food_global.update(); }

void ANT::Update()
{
    bool isLeftAntiTouchingFood = food_global.Test(this->top_left, ANTINA_CHECK_RAIDUS);
    bool isRightAntiTouchingFood = food_global.Test(this->top_right, ANTINA_CHECK_RAIDUS);

    is_left_antina_touching_faramone = faramone_global.TestLeft(*this);
    is_right_antina_touching_faramone = faramone_global.TestRight(*this);
    is_mouth_touching_food = food_global.TestFoodOnMouth(*this);

    bool is_full = StomachFullness >= ANT_STOMACH_FULLNESS;

    switch (BrainState)
    {
    case WANDER:
    {
        if (is_mouth_touching_food && !is_full)
        {
            BrainState = FEED;
            Position.w = 0;
        }

        // we smell something
        if (is_left_antina_touching_faramone || is_right_antina_touching_faramone)
        {
            BrainState = TRACK;
            Position.w = ANT_TRACK_SPEED;
            // timer to reset tracking back to wander
            StartTimer(&TrackToWanderTimer, TrackTime = GetRandomValue(7, 15));
        }

        // Random direction changes
        if (TimerDone(RandomDirectionChangeTimer))
        {
            StartTimer(&RandomDirectionChangeTimer, GetRandomValue(3, 10));
            Position.z = GetRandomValue(0, 360);
        }
    }
    break;

    case TRACK:
    {
        if (is_mouth_touching_food && !is_full)
        {
            BrainState = FEED;
        }

        if (is_left_antina_touching_faramone || is_right_antina_touching_faramone)
        {
            // Reset timer if we touch
            BrainState = TRACK;
            StartTimer(&TrackToWanderTimer, TrackTime);
        }

        if (TimerDone(TrackToWanderTimer))
        {
            // we havent touched in a while, switch back to looking around
            BrainState = WANDER;
            Position.w = ANT_WANDER_SPEED;
        }

        if ((is_left_antina_touching_faramone) || (isLeftAntiTouchingFood && !is_full))
            Position.z -= 2.0f;
        else if ((is_right_antina_touching_faramone) || (isRightAntiTouchingFood && !is_full))
            Position.z += 2.0f;
    }
    break;

    case FEED:
    {
        if (!is_mouth_touching_food)
        {
            BrainState = TRACK;
            Position.w = ANT_TRACK_SPEED;
        }
        else
        {
            food_global.feed_ant(this);
            if (is_full)
            {
                BrainState = WANDER;
                Position.w = ANT_WANDER_FULL;
                Position.z += 180;
            }
        }
    }
    break;

    default:
        BrainState = WANDER;
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

    top_left = topLeft;
    top_right = topRight;

    bottomLeft.x = x + dx * cosRotation - (dy + rec.height) * sinRotation;
    bottomLeft.y = y + dx * sinRotation + (dy + rec.height) * cosRotation;

    bottomRight.x = x + (dx + rec.width) * cosRotation - (dy + rec.height) * sinRotation;
    bottomRight.y = y + (dx + rec.width) * sinRotation + (dy + rec.height) * cosRotation;

    center_bottom.x = (bottomLeft.x + bottomRight.x) / 2.0f;
    center_bottom.y = (bottomLeft.y + bottomRight.y) / 2.0f;

    mouth_location.x = (topLeft.x + topRight.x) / 2.0f;
    mouth_location.y = (topLeft.y + topRight.y) / 2.0f;

    if (TimerDone(FaramoneDropTimer))
    {
        StartTimer(&FaramoneDropTimer, 1);
        faramone_global.ant_place_faramone(*this, center_bottom);
    }
}

const char *ToString(ANT_BRAIN_STATE bs)
{
    switch (bs)
    {
    case WANDER:
        return "Wander";
    case TRACK:
        return "Track";
    case FEED:
        return "Feed";
    default:
        return "Invalid Brain State";
    }
}

// output = output_start + round(slope * (input - input_start))
float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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

    float full_width = map(StomachFullness, 0.0f, 1.0f, 0.0f, rw / 4.0f);

    DrawCircle(x, y, full_width, RED);

    DrawCircle(center_bottom.x, center_bottom.y, 1, BLACK);

    if (is_mouth_touching_food)
    {
        DrawCircle(mouth_location.x, mouth_location.y, FOOD_CHECK_RAIDUS, RED);
    }
    else
    {
        DrawCircleLines(mouth_location.x, mouth_location.y, FOOD_CHECK_RAIDUS, DARKGRAY);
    }

    if (is_left_antina_touching_faramone)
    {
        DrawCircle(
            top_left.x,
            top_left.y,
            ANTINA_CHECK_RAIDUS, // ant antian check raidus
            BLACK);
    }
    else
    {
        DrawCircleLines(
            top_left.x,
            top_left.y,
            ANTINA_CHECK_RAIDUS, // ant antian check raidus
            BLACK);
    }

    if (is_right_antina_touching_faramone)
    {
        DrawCircle(
            top_right.x,
            top_right.y,
            ANTINA_CHECK_RAIDUS, // ant antian check raidus
            BLACK);
    }
    else
    {
        DrawCircleLines(
            top_right.x,
            top_right.y,
            ANTINA_CHECK_RAIDUS, // ant antian check raidus
            BLACK);
    }
}

void faramone_global_render_hud() {}
void faramone_global_update() { faramone_global.update(); }
void faramone_global_render_game() { faramone_global.render(); }
void faramone_global_init() { faramone_global.init(); }