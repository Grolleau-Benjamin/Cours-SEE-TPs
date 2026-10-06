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
    // TODO: return current time
    struct timespec *ts;
    int clk_id = 0;
    if (clock_gettime(clk_id,ts) == 0){
        return ts->tv_sec;
    }
    printf("error with reading time!\n");
    return -1;
}

void task_register(const char *name, uint32_t period_ms, uint32_t max_runs, void (*func)(void)) {
    // TODO
    // register a task
    // !!! Check max tasks

    if(max_runs > MAX_TASKS){
        printf("too many tasks asked, limit is %d\n",MAX_TASKS);
        return;
    }
    for(int i = 0; i < max_runs ; i++){
        tasks[i].name = name;
        tasks[i].period_ms = period_ms;
        tasks[i].max_runs = max_runs;
        tasks[i].func = func;
        printf("tasks number %d created!\n",i);
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
        // TODO: complete the loop
    }

    return 0;
}
