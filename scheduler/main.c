#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define MAX_TASKS 10

typedef struct {
    const char *name;
    uint32_t period_ms;
    uint32_t max_runs;
    uint64_t last_run_ms;
    uint32_t run_count;
    void (*func)(void);
} task_t;

static task_t tasks[MAX_TASKS];
static int task_count = 0;

uint64_t get_time_ms(void) {
   struct _timespec64 time;
   clock_gettime64(CLOCK_REALTIME, &time);
   return time.tv_sec*1000 + time.tv_nsec/1000000;
}

void task_register(const char *name, uint32_t period_ms, uint32_t max_runs, void (*func)(void)) {
    task_t task;
    task.name = name;
    task.period_ms = period_ms;
    task.max_runs = max_runs;
    task.func = func;

    if (task_count >= MAX_TASKS)
    {
        printf("Maximum amount of tasks registered reached: %d\n", task_count);
        return;
    }

    tasks[task_count] = task;
    task_count += 1;
    printf("Registered task: name: %s | period_ms: %d | max_runs: %d | task count: %d\n", task.name, task.period_ms, task.max_runs, task_count);
}

void task_1_handler(void) {
    printf("-> Task 1 logic executed\n");
}

void task_2_handler(void) {
    printf("-> Task 2 logic executed\n");
}

int main(void) {
    task_register("SensorTask", 100, 12, task_1_handler); // Runs 12 times
    task_register("LoggerTask", 500, 2, task_2_handler); // Runs 2 time

    while (true) {
        //printf("Time (ms): %d\n", get_time_ms());
    }

    return 0;
}