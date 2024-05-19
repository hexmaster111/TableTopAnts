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

enum ANT_BRAIN_STATE
{
    WANDER,
    TRACK
};

struct ANT
{
    /// @brief
    ///  z - rotation,
    ///  w - velocity
    Vector4 Position;

    Vector2 LeftAntinaHitCircle;
    Vector2 RightAntinaHitCircle;

    Timer RandomDirectionChangeTimer;
    Timer FaramoneDropTimer;

    void Update();
    void Draw();

    ANT_BRAIN_STATE BrainState;
    bool left_antina, right_antina;

    // track state stuff
};

#endif // __ANTS_H