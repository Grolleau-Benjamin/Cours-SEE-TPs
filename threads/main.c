
/*
 * Copyright (c) 2026, CATIE
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define PRIORITY   5
#define ITERATIONS 10

/* Reserve one stack per thread */
K_THREAD_STACK_DEFINE(thread1_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread2_stack, STACK_SIZE);

/* Declare one thread control block per thread */
struct k_thread thread1_data;
struct k_thread thread2_data;

/* Declare the semaphores */
K_SEM_DEFINE(ping_sem, 1, 1);
K_SEM_DEFINE(pong_sem, 0, 1);

static void ping_entry(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    for (int i = 0; i < ITERATIONS; i++) {
        /* Wait for your turn */
        k_sem_take(&ping_sem, K_FOREVER);

        printk("Ping\n");

        /* Hand the turn over */
        k_sem_give(&pong_sem);
    }
}

static void pong_entry(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    for (int i = 0; i < ITERATIONS; i++) {
        /* Wait for your turn */
        k_sem_take(&pong_sem, K_FOREVER);

        printk("Pong\n");

        /* Hand the turn over */
        k_sem_give(&ping_sem);
    }
}

int main(void)
{
    printk("=== Threads lab ===\n");

    /* Create both threads */
    k_tid_t ping_tid = k_thread_create(
        &thread1_data,
        thread1_stack,
        K_THREAD_STACK_SIZEOF(thread1_stack),
        ping_entry,
        NULL, NULL, NULL,
        PRIORITY,
        0,
        K_NO_WAIT
    );

    k_tid_t pong_tid = k_thread_create(
        &thread2_data,
        thread2_stack,
        K_THREAD_STACK_SIZEOF(thread2_stack),
        pong_entry,
        NULL, NULL, NULL,
        PRIORITY,
        0,
        K_NO_WAIT
    );

    /* Wait for both threads to finish */
    k_thread_join(ping_tid, K_FOREVER);
    k_thread_join(pong_tid, K_FOREVER);

    printk("=== Done ===\n");
    return 0;
}