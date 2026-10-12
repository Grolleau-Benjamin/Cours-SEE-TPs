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

    struct timespec ts;
    int clk_id = 0;
    if (clock_gettime(clk_id,&ts) == 0){
        return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
    }
    printf("error with reading time!\n");
    return -1;
}

void task_register(const char *name, uint32_t period_ms, uint32_t max_runs, void (*func)(void)) {
    // TODO
    // register a task
    // !!! Check max tasks

    if(task_count > MAX_TASKS){
        printf("limit of tasks reached, limit is %d\n",MAX_TASKS);
        return;
    }
    tasks[task_count].name = name;
    tasks[task_count].period_ms = period_ms;
    tasks[task_count].max_runs = max_runs;
    tasks[task_count].last_run_ms = get_time_ms();
    tasks[task_count].run_count = 0;
    tasks[task_count].func = func;
    printf("tasks number %d created!\n",task_count);
    task_count ++;
    return;
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

    int cnt = 0;

    while (true) {
        // TODO: complete the loop
        for(int i = 0; i < task_count ; i++){
            if((tasks[i].run_count < tasks[i].max_runs) && (get_time_ms()-tasks[i].last_run_ms >= tasks[i].period_ms)){
                printf("Run #%d/%d | delta : %d ms (target : %d ms)\n",tasks[i].run_count+1,tasks[i].max_runs,get_time_ms()-tasks[i].last_run_ms,tasks[i].period_ms);
                tasks[i].func();
                tasks[i].run_count ++;
                if(tasks[i].run_count == tasks[i].max_runs) cnt++;
                tasks[i].last_run_ms = get_time_ms();
            }
        }
        if (cnt == task_count) break;
    }

    return 0;
}
