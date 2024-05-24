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

void food_global_init();
void food_global_render_game();
void food_global_update();

enum ANT_BRAIN_STATE
{
    WANDER,
    TRACK,
    FEED
};

const char *ToString(ANT_BRAIN_STATE bs);

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
    float StomachFullness; // 0.0 = empty, 1.0 = full;
    bool is_mouth_touching_food,
        is_left_antina_touching_faramone,
        is_right_antina_touching_faramone;

    // track state stuff
    Timer TrackToWanderTimer;
    int TrackTime;
};

#endif // __ANTS_H