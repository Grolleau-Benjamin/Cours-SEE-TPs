#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define MAX_TASKS 10

// Définition de la structure de tâche
typedef struct {
    const char *name;
    uint32_t period_ms; // Période d'exécution
    uint32_t max_runs; // Maximum d'exécutions
    uint64_t last_run_ms; // Dernière exécution
    uint32_t run_count; // Compteur d'exécutions
    void (*func)(void); // Pointeur vers la fonction de la tâche
} task_t;

static task_t tasks[MAX_TASKS]; // Tableau pour stocker les tâches
static int task_count = 0; // Compteur de tâches enregistrées

uint64_t get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (uint64_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

void task_register(const char *name, uint32_t period_ms, uint32_t max_runs, void (*func)(void)) {

    if (task_count >= MAX_TASKS) return;

    tasks[task_count].name = name;
    tasks[task_count].period_ms = period_ms;
    tasks[task_count].max_runs = max_runs;
    tasks[task_count].last_run_ms = 0;
    tasks[task_count].run_count = 0;
    tasks[task_count].func = func;

    task_count++;
}

void task_1_handler(void) {
    printf("-> Task 1 logic executed\n");
}

void task_2_handler(void) {
    printf("-> Task 2 logic executed\n");
}

void task_3_handler(void) {
    printf("-> Task 3 logic executed\n");
}

int main(void) {
    task_register("SensorTask", 500, 12, task_1_handler); // Runs 12 times
    task_register("LoggerTask", 1000, 2, task_2_handler); // Runs 2 time
    task_register("PowerTask", 2000, 3, task_3_handler); // Runs 3 times

    while (true) {
        uint64_t time = get_time_ms();

        for(int i = 0; i < task_count; i++){

            if (tasks[i].run_count < tasks[i].max_runs) { // Check le nombre d'executions

                if (time - tasks[i].last_run_ms >= tasks[i].period_ms) { // Check la période
                    tasks[i].func();
                    tasks[i].last_run_ms = time;
                    tasks[i].run_count++;
                }
            }
        }
    }

    return 0;
}
