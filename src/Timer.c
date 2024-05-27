#include "Settings.h"

void start_timer(Timer* timer) {
    timer->start_time = clock();
}

Timer* start_create_timer() {
    Timer* timer = (Timer*)calloc(1, sizeof(Timer));
    AssertNew(timer);
    start_timer(timer);

    return timer;
}

void stop_timer(Timer* timer) {
    timer->end_time = clock();
}

double get_elapsed_time(Timer* timer) {
    stop_timer(timer);
    return ((double)(timer->end_time - timer->start_time)) / CLOCKS_PER_SEC;
}

void reset_timer(Timer* timer) {
    timer->start_time = 0;
    timer->end_time = 0;
}

void print_timer(Timer* timer) {
    printf("Temps ecoule: %.2f secondes\n", get_elapsed_time(timer));
    free(timer);
}