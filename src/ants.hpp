#ifndef __ANTS_H
#define __ANTS_H
extern "C"
{
#include <raylib.h>
};
#include "timer.hpp"

void faramone_global_render_hud();
void faramone_global_render_game();
void faramone_global_update();
void faramone_global_init();

void global_init();
void global_render_game();
void global_update();

#define FOREACH_BRAINSTATE(BRAINSTATE)                    \
    BRAINSTATE(ABS_TOUCHING_HIVE_WITH_FOOD_IN_STOMACH)    \
    BRAINSTATE(ABS_FEED)                                  \
    BRAINSTATE(ABS_LOOK_BEGIN_FOOD_ANTHILL_LOOKAROUND)    \
    BRAINSTATE(ABS_LOOK_FOR_ANTHILL_SPINSEARCH)           \
    BRAINSTATE(ABS_LOOK_FOR_ANTHILL_SPIN_SEARCH_FOUND)    \
    BRAINSTATE(ABS_LOOK_FOR_ANTHILL_SPIN_SEARCH_NOTFOUND) \
    BRAINSTATE(ABS_LOOK_FOR_FOOD_SPINSEARCH)              \
    BRAINSTATE(ABS_LOOK_FOR_FOOD_SPINSEARCH_FOUND)        \
    BRAINSTATE(ABS_LOOK_FOR_FOOD_SPINSEARCH_NOTFOUND)     \
    BRAINSTATE(ABS_BEGIN_WANDER_FOR_FOOD)                 \
    BRAINSTATE(ABS_BEGIN_SNIFF_HOME)

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum ANT_BRAIN_STATE
{
    FOREACH_BRAINSTATE(GENERATE_ENUM)
};

static const char *ANT_BRAIN_STATE_STRING[] = {FOREACH_BRAINSTATE(GENERATE_STRING)};

const char inline *ToString(ANT_BRAIN_STATE bs)
{
    return ANT_BRAIN_STATE_STRING[bs];
}

enum VISUAL_ITEM
{
    VIS_NOTHING,
    VIS_ANTHILL,
    VIS_FOOD,
    VIS_ANT
};

struct VISION_DOT
{
    VISUAL_ITEM item;
    float distance;
    Vector2 hit_point;
};
const char *ToString(ANT_BRAIN_STATE bs);
const char *ToString(VISUAL_ITEM vs);

struct ANT
{
    /// @brief
    ///  z - rotation,
    ///  w - velocity
    Vector4 Position;

    Vector2 top_left;
    Vector2 top_right;
    Vector2 center_bottom;  // faramone
    Vector2 mouth_location; // mouth

    Timer RandomDirectionChangeTimer;
    Timer FaramoneDropTimer;

    void Update();
    void Draw();

    /* ant simulation stuff */
    ANT_BRAIN_STATE BrainState;
    // 0.0 = empty, 1.0 = full;
    float StomachFullness;
    bool is_mouth_touching_food,
        isLeftAntiTouchingFood,
        is_stomach_empty, touching_the_hive,
        isRightAntiTouchingFood, is_full,
        is_left_antina_touching_faramone,
        is_right_antina_touching_faramone;

    /* Ant vision
     *
     *
     * [l0,l1,l2,ct,r2,r1,r0]
     *
     *        \   |   /
     *         \  |  /
     *          \ | /
     *         __\|/__
     *         |     |
     *         |     |
     *         |     |
     *         |_____|
     */

    Vector2 eye_target_l0,
        eye_target_l1,
        eye_target_l2,
        eye_target_r2,
        eye_target_r1,
        eye_target_r0,
        eye_target_center;

    VISION_DOT vi_eye_target_l0,
        vi_eye_target_l1,
        vi_eye_target_l2,
        vi_eye_target_r2,
        vi_eye_target_r1,
        vi_eye_target_r0,
        vi_eye_target_center;

    bool spin_search_turn_left;
    float spin_search_start_pos_z;
    int food_no_touch_count;

    int wander_stumble;

    // track state stuff
    Timer TrackToWanderTimer;
    int TrackTime;
};

#endif // __ANTS_H