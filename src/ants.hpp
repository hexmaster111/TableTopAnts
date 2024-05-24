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
    TRACK
};

const char *ToString(ANT_BRAIN_STATE bs);

struct ANT
{
    /// @brief
    ///  z - rotation,
    ///  w - velocity
    Vector4 Position;

    Vector2 LeftAntinaHitCircle;
    Vector2 RightAntinaHitCircle;
    Vector2 center_bottom; // faramone
    Vector2 center_front;  // mouth

    Timer RandomDirectionChangeTimer;
    Timer FaramoneDropTimer;

    void Update();
    void Draw();

    ANT_BRAIN_STATE BrainState;
    bool left_antina, right_antina;
    bool mouth_touching_food;

    // track state stuff
    Timer TrackToWanderTimer;
    int TrackTime;
};

#endif // __ANTS_H