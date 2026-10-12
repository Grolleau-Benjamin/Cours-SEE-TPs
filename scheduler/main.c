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
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return (uint64_t)(time.tv_sec * 1000 + time.tv_nsec / 1000000);
}

void task_register(const char *name, uint32_t period_ms, uint32_t max_runs, void (*func)(void)) {
    if (task_count < MAX_TASKS) {
        tasks[task_count++] = (task_t){
            .name = name,
            .period_ms = period_ms,
            .max_runs = max_runs,
            .last_run_ms = 0,
            .run_count = 0,
            .func = func
        };
    }
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

    int i = 0;
    int l_alldone = 0;
    while (true) {
        l_alldone = 1;
        for (i = 0; i < task_count; i++) {
            if (tasks[i].run_count < tasks[i].max_runs) {
                l_alldone = 0;
                uint64_t l_time = get_time_ms();
                if (l_time >= tasks[i].last_run_ms + tasks[i].period_ms) {
                    printf("Task \"%s\" executed, run: %d/%d, dT: %ldms\n",
                        tasks[i].name,
                        tasks[i].run_count + 1,
                        tasks[i].max_runs,
                        l_time - tasks[i].last_run_ms
                    );
                    tasks[i].last_run_ms = l_time;
                    tasks[i].run_count++;
                    tasks[i].func();
                }
            }
        }
        if (l_alldone)
            break;
    }

    printf("All Tasks executed\n");

    return 0;
}
