#ifndef __TIMER_H
#define __TIMER_H

#include <raylib.h>

typedef struct Timer
{
    double startTime; // Start time (seconds)
    double lifeTime;  // Lifetime (seconds)
} Timer;

void StartTimer(Timer *timer, double lifetime);
bool TimerDone(Timer timer);
double GetElapsed(Timer timer);
#endif //__TIMER_H