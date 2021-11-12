#ifndef KU_TIMER_H
#define KU_TIMER_H

#include <time.h>

#include "types.h"

typedef struct CPUTimer CPUTimer;

struct CPUTimer {
    u64 mark;
    u64 elapsed;
};

u64 get_clock();
u64 get_clock_since(u64 start);

CPUTimer init_cpu_timer();
u64 get_cpu_timer_clock(CPUTimer timer);
void start_cpu_timer(CPUTimer *timer);
void pause_cpu_timer(CPUTimer *timer);
void resume_cpu_timer(CPUTimer *timer);


#endif // KU_TIMER_H
