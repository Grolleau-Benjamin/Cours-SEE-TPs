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
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    int time = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    return time;
}

void task_register(const char *name, uint32_t period_ms, uint32_t max_runs, void (*func)(void)) {
    if(task_count >= MAX_TASKS)
        printf("Il y a déjà 10 tâches (le nombre maximum autorisé)\n");
    else
    {
        tasks[task_count].name = name;
        tasks[task_count].period_ms = period_ms;
        tasks[task_count].max_runs = max_runs;
        tasks[task_count].func = func;
        task_count ++;
        printf("Task créée");
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

    while (true) {
        uint64_t current_time = get_time_ms();
        for(int i = 0; i <= task_count; i++){
            if (tasks[i].run_count < tasks[i].max_runs && current_time - tasks[i].last_run_ms >= tasks[i].period_ms) {
                printf("tache en cours : %s\n", tasks[i].name);
                tasks[i].func();
                tasks[i].last_run_ms = current_time;
                tasks[i].run_count++;
            }
        }

    }

    return 0;
}
