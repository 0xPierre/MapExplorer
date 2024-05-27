#include <time.h>

typedef struct {
    clock_t start_time;
    clock_t end_time;
} Timer;

Timer* start_create_timer(void);

void start_timer(Timer* timer);

void stop_timer(Timer * timer);

double get_elapsed_time(Timer* timer);

void reset_timer(Timer* timer);

void print_timer(Timer* timer);
