#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define MAX_TASKS 10

typedef struct {
    const char *name;
    uint32_t period_ms; //tache faites tous les x temps
    uint32_t max_runs; //nbre d'executions de la tache
    uint64_t last_run_ms; //derniere fois que la tache a été faite
    uint32_t run_count; //nbre de fois ou elle a était executé
    void (*func)(void);
} task_t; //tache avec plusieurs infos

static task_t tasks[MAX_TASKS]; //creer le nbre de taches demandées
static int task_count = 0; //nbre de taches (apres chaque register)

uint64_t get_time_ms(void) {
    struct timespec temps;
    clock_gettime(CLOCK_MONOTONIC, &temps);
    return (uint64_t)(temps.tv_sec * 1000 + temps.tv_nsec / 1000000);
}

void task_register(const char *name, uint32_t period_ms, uint32_t max_runs, void (*func)(void)) {
    
    if (task_count >= MAX_TASKS) {
        printf("Nombre de tâches max dépassé\n");
        return;
    }

    tasks[task_count].name = name;
    tasks[task_count].period_ms = period_ms;
    tasks[task_count].max_runs = max_runs;
    tasks[task_count].func = func;
    tasks[task_count].last_run_ms = get_time_ms();
    tasks[task_count].run_count = 0;
    task_count++;
    // TODO
    // register a task
    // !!! Check max tasks
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
        uint64_t temps = get_time_ms();
        bool taches_finis = true;
        int i = 0;
        for (i ; i<task_count ;i++) {   
            if (tasks[i].run_count < tasks[i].max_runs) {
                taches_finis = false;
                if (temps - tasks[i].last_run_ms >= tasks[i].period_ms) {
                tasks[i].func();
                tasks[i].last_run_ms = temps;
                tasks[i].run_count++;
                }
            }
        }
        if (taches_finis == true) {
            break;     
        }
    }
    return 0;
}
