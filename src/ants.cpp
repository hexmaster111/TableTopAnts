#include "ants.hpp"
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include "raygui.h"

// https://stackoverflow.com/a/48693317
#define DEG_TO_RAD(angleInDegrees) \
    ((angleInDegrees) * M_PI / 180.0f)

#define ANTINA_CHECK_RAIDUS (2.0f)
#define FOOD_CHECK_RAIDUS (2.0f)
#define ANT_RW (10.0f)
#define ANT_RH (10.0f)

#define ANT_TRACK_SPEED (.15)
#define ANT_WANDER_SPEED (.25)
#define ANT_WANDER_FULL (.1)
#define ANT_STOMACH_FULLNESS (2.0)

#define ANT_VISION_ANGLE (120)

#define ANT_VISION_DISTANCE (200)

#define SPIN_SEARCH_SPIN_SPEED (0.5f)

#define faramone_count 5000
#define food_count 5000
void AntBrain(ANT *a);
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
            Strength -= .00005; // faramone decay
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
        const int fontsize = 3;
        const char *text = TextFormat("%.01f", Strength);
        DrawCircle(Center.x, Center.y, Strength, RED);
        DrawText(text, Center.x - (MeasureText(text, fontsize) / 2), Center.y - (fontsize / 2), fontsize, BLACK);
    }

    bool HitTest(Vector2 pos, float radius)
    {
        return CheckCollisionCircles(pos, radius, Center, Strength);
    }

    void Update()
    {
        if (InUse && Strength <= 0)
        {
            InUse = false;
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
    const float Raidus = 32.0f;
    Vector2 Center;
    float FoodStore;

    void update()
    {
    }

    void draw()
    {
        DrawCircleGradient(Center.x, Center.y, Raidus, BROWN, YELLOW);
    }
} dev_hill;

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

    float StomachFullesFaramoneMultiplyer = 2;

    void ant_place_faramone(ANT ant, Vector2 pos, float power)
    {
        add_faramone(power, pos);
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
        ant->StomachFullness += .01;
        f->Strength -= .001;
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

    FOOD food[food_count];

private:
} food_global;

void global_init()
{
    dev_hill.Center = (Vector2){-100, -100};
    dev_hill.FoodStore = 0;
    food_global.init();
    // food_global.add((Vector2){10, 10}, 3);

    for (size_t i = 0; i < 100; i++)
        food_global.add((Vector2){GetRandomValue(1000, 1500), GetRandomValue(1000, 1500)}, GetRandomValue(1, 3));

    for (size_t i = 0; i < 100; i++)
        food_global.add((Vector2){GetRandomValue(-500, -1000), GetRandomValue(500, 1500)}, GetRandomValue(1, 3));

    for (size_t i = 0; i < 100; i++)
        food_global.add((Vector2){GetRandomValue(-1000, -1500), GetRandomValue(1000, -1500)}, GetRandomValue(1, 3));

    for (size_t i = 0; i < 100; i++)
        food_global.add((Vector2){GetRandomValue(-500, -1000), GetRandomValue(500, -1500)}, GetRandomValue(1, 3));
}

void global_render_game()
{
    food_global.render();
    dev_hill.draw();
}

void global_update()
{
    food_global.update();
    dev_hill.update();
}

const char *ToString(VISUAL_ITEM vi)
{
    switch (vi)
    {
    case VIS_ANT:
        return "ANT";
    case VIS_ANTHILL:
        return "ANT HILL";
    case VIS_FOOD:
        return "FOOD";
    case VIS_NOTHING:
        return "NOTHING";

    default:
        return "DEFAULT VISUAL_ITEM ToString()";
    }
}

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float MeasureDistance(Vector2 pt1, Vector2 pt2)
{
    return sqrtf(pow(pt2.x - pt1.x, 2) + pow(pt2.y - pt1.y, 2));
}

bool CheckColisionCircleLine(
    Vector2 circleCenter, float circleRaidus,
    Vector2 p0,
    Vector2 p1,
    Vector2 *contact_point)
{

    // Calculate the direction of the line
    Vector2 lineDir = {p1.x - p0.x, p1.y - p0.y};

    // Normalize the direction
    float lineLength = sqrt(lineDir.x * lineDir.x + lineDir.y * lineDir.y);
    lineDir.x /= lineLength;
    lineDir.y /= lineLength;

    // Calculate the difference between the circle center and the line start point
    Vector2 diff = {circleCenter.x - p0.x, circleCenter.y - p0.y};

    // Project the difference onto the line direction
    float t = diff.x * lineDir.x + diff.y * lineDir.y;

    // Check if the projection is beyond the line segment
    if (t < 0.0f || t > lineLength)
    {
        return false;
    }

    // Calculate the point of intersection on the line
    Vector2 intersection = {p0.x + lineDir.x * t, p0.y + lineDir.y * t};

    // Calculate the distance from the intersection to the circle center
    diff = {circleCenter.x - intersection.x, circleCenter.y - intersection.y};
    float dist = sqrt(diff.x * diff.x + diff.y * diff.y);

    // If the distance is less than the circle radius, there is a collision
    if (dist <= circleRaidus)
    {
        // If a contact point is requested, store the intersection point
        if (contact_point != nullptr)
        {
            contact_point->x = intersection.x;
            contact_point->y = intersection.y;
        }
        return true;
    }

    // No collision
    return false;
}

void ANT::Update()
{
    isLeftAntiTouchingFood = food_global.Test(this->top_left, ANTINA_CHECK_RAIDUS);
    isRightAntiTouchingFood = food_global.Test(this->top_right, ANTINA_CHECK_RAIDUS);

    is_left_antina_touching_faramone = faramone_global.TestLeft(*this);
    is_right_antina_touching_faramone = faramone_global.TestRight(*this);
    is_mouth_touching_food = food_global.TestFoodOnMouth(*this);
    is_full = StomachFullness >= ANT_STOMACH_FULLNESS;

#define FIND_EYE_HITS(vi_eye_target, eye_target)                                                             \
    {                                                                                                        \
        Vector2 contact;                                                                                     \
        VISION_DOT closest = (VISION_DOT){VIS_NOTHING, MAXFLOAT};                                            \
        bool any = false;                                                                                    \
        if (CheckColisionCircleLine(dev_hill.Center, dev_hill.Raidus, mouth_location, eye_target, &contact)) \
        {                                                                                                    \
            vi_eye_target = (VISION_DOT){VIS_ANTHILL, MeasureDistance(mouth_location, contact), contact};    \
            any = true;                                                                                      \
        }                                                                                                    \
        if (!any)                                                                                            \
        {                                                                                                    \
            vi_eye_target = closest; /*uses nothing*/                                                        \
        }                                                                                                    \
        any = false;                                                                                         \
        for (size_t i = 0; i < food_count; i++)                                                              \
        {                                                                                                    \
            FOOD f = food_global.food[i];                                                                    \
            if (!f.InUse)                                                                                    \
                continue;                                                                                    \
            if (CheckColisionCircleLine(f.Center, f.Strength, mouth_location, eye_target, &contact))         \
            {                                                                                                \
                VISION_DOT tmp = (VISION_DOT){VIS_FOOD, MeasureDistance(mouth_location, contact)};           \
                if (tmp.distance < closest.distance)                                                         \
                {                                                                                            \
                    closest = tmp;                                                                           \
                    any = true;                                                                              \
                }                                                                                            \
            }                                                                                                \
        }                                                                                                    \
        if (any)                                                                                             \
            vi_eye_target = closest;                                                                         \
    }

    FIND_EYE_HITS(vi_eye_target_center, eye_target_center);
    FIND_EYE_HITS(vi_eye_target_l0, eye_target_l0);
    FIND_EYE_HITS(vi_eye_target_l1, eye_target_l1);
    FIND_EYE_HITS(vi_eye_target_l2, eye_target_l2);
    FIND_EYE_HITS(vi_eye_target_r0, eye_target_r0);
    FIND_EYE_HITS(vi_eye_target_r1, eye_target_r1);
    FIND_EYE_HITS(vi_eye_target_r2, eye_target_r2);

#undef FIND_EYE_HITS

    is_stomach_empty = 0 >= StomachFullness;
    touching_the_hive = CheckCollisionCircles(dev_hill.Center, dev_hill.Raidus, (Vector2){Position.x, Position.y}, ANT_RW);

    AntBrain(this);

    // antina hitbox calulations, based on DrawRectanglePro impl
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

    top_left.x = x + dx * cosRotation - dy * sinRotation;
    top_left.y = y + dx * sinRotation + dy * cosRotation;

    top_right.x = x + (dx + rec.width) * cosRotation - dy * sinRotation;
    top_right.y = y + (dx + rec.width) * sinRotation + dy * cosRotation;

    bottomLeft.x = x + dx * cosRotation - (dy + rec.height) * sinRotation;
    bottomLeft.y = y + dx * sinRotation + (dy + rec.height) * cosRotation;

    bottomRight.x = x + (dx + rec.width) * cosRotation - (dy + rec.height) * sinRotation;
    bottomRight.y = y + (dx + rec.width) * sinRotation + (dy + rec.height) * cosRotation;

    center_bottom.x = (bottomLeft.x + bottomRight.x) / 2.0f;
    center_bottom.y = (bottomLeft.y + bottomRight.y) / 2.0f;

    mouth_location.x = (top_left.x + top_right.x) / 2.0f;
    mouth_location.y = (top_left.y + top_right.y) / 2.0f;

    eye_target_center = {
        mouth_location.x + ANT_VISION_DISTANCE * cosf(Position.z * DEG2RAD),
        mouth_location.y + ANT_VISION_DISTANCE * sinf(Position.z * DEG2RAD)};
    eye_target_l0 = {
        mouth_location.x + ANT_VISION_DISTANCE * cosf((Position.z - ANT_VISION_ANGLE / 2) * DEG2RAD),
        mouth_location.y + ANT_VISION_DISTANCE * sinf((Position.z - ANT_VISION_ANGLE / 2) * DEG2RAD)};
    eye_target_l1 = {
        mouth_location.x + ANT_VISION_DISTANCE * cosf((Position.z - ANT_VISION_ANGLE / 4) * DEG2RAD),
        mouth_location.y + ANT_VISION_DISTANCE * sinf((Position.z - ANT_VISION_ANGLE / 4) * DEG2RAD)};
    eye_target_l2 = {
        mouth_location.x + ANT_VISION_DISTANCE * cosf((Position.z - ANT_VISION_ANGLE / 8) * DEG2RAD),
        mouth_location.y + ANT_VISION_DISTANCE * sinf((Position.z - ANT_VISION_ANGLE / 8) * DEG2RAD)};
    eye_target_r2 = {
        mouth_location.x + ANT_VISION_DISTANCE * cosf((Position.z + ANT_VISION_ANGLE / 8) * DEG2RAD),
        mouth_location.y + ANT_VISION_DISTANCE * sinf((Position.z + ANT_VISION_ANGLE / 8) * DEG2RAD)};
    eye_target_r1 = {
        mouth_location.x + ANT_VISION_DISTANCE * cosf((Position.z + ANT_VISION_ANGLE / 4) * DEG2RAD),
        mouth_location.y + ANT_VISION_DISTANCE * sinf((Position.z + ANT_VISION_ANGLE / 4) * DEG2RAD)};
    eye_target_r0 = {
        mouth_location.x + ANT_VISION_DISTANCE * cosf((Position.z + ANT_VISION_ANGLE / 2) * DEG2RAD),
        mouth_location.y + ANT_VISION_DISTANCE * sinf((Position.z + ANT_VISION_ANGLE / 2) * DEG2RAD)};
}

bool show_ant_vision;

void ANT::Draw()
{
    const float rw = ANT_RW, rh = ANT_RH;
    float x = Position.x - rw / 2.0f,
          y = Position.y - rh / 2.0f;

    if (show_ant_vision)
    {

        DrawLineV(mouth_location, eye_target_center, BLACK);
        DrawLineV(mouth_location, eye_target_l0, RED);
        DrawLineV(mouth_location, eye_target_l1, GREEN);
        DrawLineV(mouth_location, eye_target_l2, BLUE);
        DrawLineV(mouth_location, eye_target_r2, BLUE);
        DrawLineV(mouth_location, eye_target_r1, GREEN);
        DrawLineV(mouth_location, eye_target_r0, RED);

#define DRAW_HIT_LINE_BLACK(vi_eye_target)                                 \
    {                                                                      \
        if (vi_eye_target.item != VIS_NOTHING)                             \
        {                                                                  \
            DrawLineEx(mouth_location, vi_eye_target.hit_point, 2, BLACK); \
        }                                                                  \
    }

        DRAW_HIT_LINE_BLACK(vi_eye_target_l0);
        DRAW_HIT_LINE_BLACK(vi_eye_target_l1);
        DRAW_HIT_LINE_BLACK(vi_eye_target_l2);
        DRAW_HIT_LINE_BLACK(vi_eye_target_r0);
        DRAW_HIT_LINE_BLACK(vi_eye_target_r1);
        DRAW_HIT_LINE_BLACK(vi_eye_target_r2);
        DRAW_HIT_LINE_BLACK(vi_eye_target_center);
#undef DRAW_HIT_LINE_BLACK
    }

    DrawRectanglePro((Rectangle){x, y, rw, rh}, (Vector2){rw / 2.0f, rh / 2.0f}, Position.z - 90.0f, BLACK);

    float full_width = map(StomachFullness, 0.0f, ANT_STOMACH_FULLNESS, 0.0f, rw / 4.0f);
    DrawCircle(x, y, full_width, RED);
    DrawCircleLines(x, y, rw / 4.0f, RED);
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

void faramone_global_render_hud()
{

    GuiSlider({10, 500, 128, 16}, "",
              TextFormat("Stomch fullness to faramone output %.1f",
                         faramone_global.StomachFullesFaramoneMultiplyer),
              &faramone_global.StomachFullesFaramoneMultiplyer, 0, 3);

    GuiCheckBox({10, 516, 128, 16}, "Show Vision", &show_ant_vision);
}
void faramone_global_update()
{
    // main_main.update();
    faramone_global.update();
}
void faramone_global_render_game() { faramone_global.render(); }
void faramone_global_init() { faramone_global.init(); }

bool AntSeeAny(ANT *a, VISUAL_ITEM vis)
{
    bool any = false;

    if (a->vi_eye_target_center.item == vis)
        any = true;
    if (a->vi_eye_target_l0.item == vis)
        any = true;
    if (a->vi_eye_target_l1.item == vis)
        any = true;
    if (a->vi_eye_target_l2.item == vis)
        any = true;
    if (a->vi_eye_target_r0.item == vis)
        any = true;
    if (a->vi_eye_target_r1.item == vis)
        any = true;
    if (a->vi_eye_target_r2.item == vis)
        any = true;

    return any;
}

void AntBrain(ANT *a)
{

    if (a->touching_the_hive && !a->is_stomach_empty)
        a->BrainState = ABS_TOUCHING_HIVE_WITH_FOOD_IN_STOMACH;

    if (a->is_mouth_touching_food && !a->is_full && a->BrainState != ABS_FEED)
    {
        a->BrainState = ABS_FEED;
        a->food_no_touch_count = 0;
    }

    bool should_move = false;
    bool should_drop_faramones = false;
    float faramones_power = 1.0f;
    switch (a->BrainState)
    {

    case ABS_LOOK_BEGIN_FOOD_ANTHILL_LOOKAROUND:
    {

        fprintf(stdout, "ABS_BEGIN_WANDER: StomachFullness: %.1f\n", a->StomachFullness);
        // if we are less then 50% full, we go look for food
        // if we are gtr then 50%, we go look for the hive

        a->spin_search_turn_left = (bool)GetRandomValue(0, 1);
        a->spin_search_start_pos_z = a->Position.z;
        if (a->StomachFullness > .8f)
        {
            a->BrainState = ABS_LOOK_FOR_ANTHILL_SPINSEARCH;
        }
        else
        {
            a->BrainState = ABS_LOOK_FOR_FOOD_SPINSEARCH;
        }
    }
    break;

    case ABS_TOUCHING_HIVE_WITH_FOOD_IN_STOMACH:
    {
        dev_hill.FoodStore += .01f;
        a->StomachFullness -= .01f;
        if (0 >= a->StomachFullness)
        {
            a->Position.z += 180;
            a->BrainState = ABS_LOOK_BEGIN_FOOD_ANTHILL_LOOKAROUND;
        }
    }
    break;

    case ABS_FEED:
    {

        if (a->food_no_touch_count > 100)
        {
            a->BrainState = ABS_LOOK_FOR_FOOD_SPINSEARCH;
            a->Position.w = ANT_WANDER_SPEED;
        }

        if (!a->is_mouth_touching_food)
        {
            should_move = true;
            a->Position.w = .05;
            a->food_no_touch_count++;
        }
        else
        {
            food_global.feed_ant(a);
            a->food_no_touch_count = 0;
            if (a->is_full)
            {
                a->BrainState = ABS_LOOK_BEGIN_FOOD_ANTHILL_LOOKAROUND;
                a->Position.z += 180;
                a->Position.w = ANT_WANDER_SPEED;
            }
        }
    }
    break;

    case ABS_LOOK_FOR_FOOD_SPINSEARCH:
    {
        if (a->vi_eye_target_center.item == VIS_FOOD)
        {
            a->BrainState = ABS_LOOK_FOR_FOOD_SPINSEARCH_FOUND;
        }
        else
        {
            a->Position.z += a->spin_search_turn_left ? -SPIN_SEARCH_SPIN_SPEED : SPIN_SEARCH_SPIN_SPEED;
            if (fabs(a->Position.z - a->spin_search_start_pos_z) >= 360.0)
            {
                a->BrainState = ABS_LOOK_FOR_FOOD_SPINSEARCH_NOTFOUND;
            }
        }
    }
    break;

    case ABS_LOOK_FOR_ANTHILL_SPINSEARCH:
    {
        if (a->vi_eye_target_center.item == VIS_ANTHILL)
        {
            a->BrainState = ABS_LOOK_FOR_ANTHILL_SPIN_SEARCH_FOUND;
        }
        else
        {
            a->Position.z += a->spin_search_turn_left ? -SPIN_SEARCH_SPIN_SPEED : SPIN_SEARCH_SPIN_SPEED;

            if (fabs(a->Position.z - a->spin_search_start_pos_z) >= 360.0)
            {
                a->BrainState = ABS_LOOK_FOR_ANTHILL_SPIN_SEARCH_NOTFOUND;
            }
        }
    }
    break;

    case ABS_LOOK_FOR_ANTHILL_SPIN_SEARCH_FOUND:
    {

        if (a->vi_eye_target_center.item != VIS_ANTHILL)
        {
            a->BrainState = ABS_LOOK_FOR_ANTHILL_SPINSEARCH;
            break;
        }

        should_move = true;
    }
    break;

    case ABS_LOOK_FOR_FOOD_SPINSEARCH_FOUND:
    {

        if (a->vi_eye_target_center.item != VIS_FOOD)
        {
            a->BrainState = ABS_LOOK_FOR_FOOD_SPINSEARCH;
            break;
        }

        should_move = true;
        should_drop_faramones = true;
    }
    break;

    case ABS_LOOK_FOR_FOOD_SPINSEARCH_NOTFOUND:
    {
        a->BrainState = ABS_BEGIN_WANDER_FOR_FOOD;
    }
    break;

    case ABS_LOOK_FOR_ANTHILL_SPIN_SEARCH_NOTFOUND:
    {
        a->BrainState = ABS_BEGIN_SNIFF_HOME;
    }
    break;

    case ABS_BEGIN_SNIFF_HOME:
    {
        if (a->is_left_antina_touching_faramone)
        {
            a->Position.z -= 2;
        }

        if (a->is_right_antina_touching_faramone)
        {
            a->Position.z += 2;
        }
        should_move = true;
    }
    break;

    case ABS_BEGIN_WANDER_FOR_FOOD:
    {
        if (AntSeeAny(a, VIS_FOOD))
        {
            a->BrainState = ABS_LOOK_BEGIN_FOOD_ANTHILL_LOOKAROUND;
            break;
        }

        should_move = true;
        should_drop_faramones = true;
        faramones_power = 3.0f;

        if (a->is_left_antina_touching_faramone)
        {
            a->Position.z -= 2;
        }

        if (a->is_right_antina_touching_faramone)
        {
            a->Position.z += 2;
        }

        if (a->wander_stumble-- <= 0)
        {
            a->wander_stumble = GetRandomValue(10, 100);
            a->Position.z = GetRandomValue(a->Position.z - 30, a->Position.z + 30);
        }
    }
    break;

    default:
        break;
    }

    if (should_move)
    {
        a->Position.x += a->Position.w * cosf(DEG_TO_RAD(a->Position.z));
        a->Position.y += a->Position.w * sinf(DEG_TO_RAD(a->Position.z));
    }

    if (should_drop_faramones)
    {
        if (TimerDone(a->FaramoneDropTimer))
        {
            StartTimer(&a->FaramoneDropTimer, 1);
            faramone_global.ant_place_faramone(*a, a->center_bottom, faramones_power);
        }
    }
}