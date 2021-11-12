#include "timer.h"

u64 get_clock() {
    clock_t start_clock = clock();
    if (start_clock < 0) {
        return 0;
    }
    return (u64)start_clock;
}

u64 get_clock_since(u64 start) {
    return get_clock() - start;
}

CPUTimer init_cpu_timer() {
    CPUTimer timer = {
        .mark    = get_clock(),
        .elapsed = 0,
    };
    return timer;
}

u64 get_cpu_timer_clock(CPUTimer timer) {
    if (timer.mark == 0) {
        return timer.elapsed;
    }
    return get_clock() - timer.mark + timer.elapsed;
}

void start_cpu_timer(CPUTimer *timer) {
    timer->elapsed = 0;
    timer->mark    = get_clock();
}

void pause_cpu_timer(CPUTimer *timer) {
    timer->elapsed += get_clock() - timer->mark;
    timer->mark = 0;
}

void resume_cpu_timer(CPUTimer *timer) {
    timer->mark = get_clock();
}
