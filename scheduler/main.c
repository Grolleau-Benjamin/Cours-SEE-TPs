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
    clock_gettime(CLOCK_MONOTONIC, &time );
    return (time.tv_sec*1000 + time.tv_nsec/1000000);
}

void task_register(const char *name, uint32_t period_ms, uint32_t max_runs, void (*func)(void)) {

    if (task_count == MAX_TASKS)
    {
        printf("ERROR: TASKS QUEUE FULL\n");
        return;
    }
    tasks[task_count].name = name;
    tasks[task_count].period_ms = period_ms;
    tasks[task_count].max_runs = max_runs;
    tasks[task_count].func = func;
    tasks[task_count].run_count = 0;
    tasks[task_count].last_run_ms = get_time_ms();

    printf("SUCCESS: TASK %s ADDED \n", tasks[task_count].name);
    task_count++;
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
    printf("Number of task: %d \n", task_count);

    while (true) {
        for (int i = 0; i <= task_count; i++)
        {
            if (tasks[i].run_count < tasks[i].max_runs)
            {
                uint64_t current_time = get_time_ms();
                uint64_t delta = current_time - tasks[i].last_run_ms;
                if( delta >= tasks[i].period_ms)
                {
                    printf("Task: %s | Run %d/%d | Delta: %d ms (Target: %d)\n", tasks[i].name, tasks[i].run_count + 1, tasks[i].max_runs, delta, tasks[i].period_ms);
                    tasks[i].func();
                    tasks[i].last_run_ms = current_time;
                    tasks[i].run_count++;
                }
            }
        }
    }

    return 0;
}
