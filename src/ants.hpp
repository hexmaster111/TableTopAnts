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

struct ANT
{
    /// @brief
    ///  z - rotation,
    ///  w - velocity
    Vector4 Position;
    Timer RandomDirectionChangeTimer;
    void Update();
    void Draw();
};

/*
# Faramone trail
- have a decay
- stack
*/

#endif // __ANTS_H