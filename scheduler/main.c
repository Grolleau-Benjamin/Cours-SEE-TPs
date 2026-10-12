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
    struct timespec time;
    if(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time)!=0){
        printf("Erreur pour récupérer le temps");
        return -1;
    }

    printf("Current time: %ld ms\n",time.tv_sec);

    return time.tv_sec;
}

void task_register(const char *name, uint32_t period_ms, uint32_t max_runs, void (*func)(void)) {
    if(task_count >= MAX_TASKS){
        printf("Impossible de créer une nouvelle task, nombre de task max atteint");
    }
    tasks[task_count].name = name;
    tasks[task_count].period_ms = period_ms;
    tasks[task_count].max_runs = max_runs;
    tasks[task_count].name = 0;
    tasks[task_count].func = func;
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
    int t = 0;
    while (true) {
        // TODO: complete the loop
        while(t<=MAX_TASKS){
            for(int i=0; i == tasks[t].max_runs -1; i++){
                printf("%s is executed\n",tasks[t].name);
            }
        t++;
        
        }


    }

    return 0;
}
