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

    // TODO: return current time
uint64_t get_time_ms(void)
    {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
    }

void task_register(const char *name, uint32_t period_ms, uint32_t max_runs, void (*func)(void)) {
    // !!! Check max tasks
    if(task_count >= MAX_TASKS){
        printf("erreur, max atteint");
        return;
    }
    // register a task
    tasks[task_count] = (task_t){.name = name,.period_ms = period_ms,.max_runs = max_runs,.run_count = 0,.last_run_ms = get_time_ms(),.func = func
    };
    task_count++;
    // TODO
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

bool taches_terminees = false;

while (!taches_terminees) {

    taches_terminees = true;

    uint64_t now = get_time_ms();

    for (int i = 0; i < task_count; i++) {

        if (tasks[i].run_count < tasks[i].max_runs) {

            taches_terminees = false;

            if (now - tasks[i].last_run_ms >= tasks[i].period_ms) {
                tasks[i].func();
                tasks[i].run_count++;
                tasks[i].last_run_ms = now;
            }
        }
    }

    usleep(1000);
}
}