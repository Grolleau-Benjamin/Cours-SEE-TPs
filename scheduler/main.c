#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define MAX_TASKS 10
#define MSEC_IN_NSEC 1000000

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
bool task_active = false;

uint64_t get_time_ms(void) {
    struct timespec tspec;
    clock_gettime(CLOCK_REALTIME, &tspec);

    return (uint64_t)(tspec.tv_nsec/MSEC_IN_NSEC);
}

void task_register(const char *name, uint32_t period_ms, uint32_t max_runs, void (*func)(void)) {
    
    if (task_count > MAX_TASKS)
    {
        printf("Error : Maximum number of task reached. Task %s was dropped. \n", name);
        return;
    }

    tasks[task_count].name = name;
    tasks[task_count].period_ms = period_ms;
    tasks[task_count].max_runs = max_runs;
    tasks[task_count].run_count = 0;
    tasks[task_count].last_run_ms = 0;
    tasks[task_count].func = func;

    printf("Register task %s, ID = %d \n", name, task_count);

    task_count++;
}

void print_task_stats(int task_id)
{
    task_t t = tasks[task_id];
    printf("Task ID = %d | Task name = %s | Run %02d/%02d, delay %d ms \n", task_id, t.name, t.run_count+1, t.max_runs, t.last_run_ms - get_time_ms());
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
        task_active = false;

        for (int i = 0; i < task_count; i++)
        {
            // Check if task period has run out, or if task has never been run
            if ((tasks[i].last_run_ms - get_time_ms()) > tasks[i].period_ms)
            {
                // Check for remaining runs
                if(tasks[i].run_count < tasks[i].max_runs)
                {
                    task_active = true;

                    // Execute function
                    tasks[i].func();

                    // Print stats
                    print_task_stats(i);

                    // Update last_run_ms
                    tasks[i].last_run_ms = get_time_ms();

                    // Increment run_count
                    tasks[i].run_count++;
                }
            }
            else
            {
                // Timer didn’t run out -> set to true to prevent breaking early
                task_active = true;
            }
        }

        if (!task_active)
        {
            printf("All tasks have been executed. Exiting...");
            break;
        }
    }

    return 0;
}
