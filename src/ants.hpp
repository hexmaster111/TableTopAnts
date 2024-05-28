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

enum ANT_BRAIN_STATE
{
    WANDER,
    TRACK,
    FEED
};

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
        isRightAntiTouchingFood, is_full,
        is_left_antina_touching_faramone,
        is_right_antina_touching_faramone;

    // Ant vision
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

    // track state stuff
    Timer TrackToWanderTimer;
    int TrackTime;
};

#endif // __ANTS_H