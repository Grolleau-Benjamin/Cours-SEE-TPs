#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define PRIORITY   5
#define ITERATIONS 10

static int shared_counter;

/* TODO: q2 - declare the mutex */
K_MUTEX_DEFINE(my_mutex);


static void print_word(const char *word)
{
    for (const char *c = word; *c != '\0'; c++) {
        printk("%c", *c);
        k_sleep(K_MSEC(5));
    }
    printk("\n");
}

static void worker(const char *name)
{
    for (int i = 0; i < ITERATIONS; i++) {
        /* TODO: q2 - enter the critical section */
		k_mutex_lock(&my_mutex, K_FOREVER);  //avec mutex
        int local = shared_counter;

        print_word(name);
        k_sleep(K_MSEC(5));

        shared_counter = local + 1;

        /* TODO: q2 - leave the critical section */
		k_mutex_unlock(&my_mutex); //avec mutex

        k_sleep(K_MSEC(1));
    }
}

static void ping_entry(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    worker("Ping");
}

static void pong_entry(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    worker("Pong");
}

/* TODO: q1 - reserve one stack per thread */
K_THREAD_STACK_DEFINE(ping_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(pong_stack, STACK_SIZE);

/* TODO: q1 - declare one thread control block per thread */
struct k_thread ping_thread;
struct k_thread pong_thread;

int main(void)
{
    printk("=== Mutex lab ===\n");

    /* TODO: q1 - create both threads, same priority */
    k_tid_t ping_tid = k_thread_create(&ping_thread, ping_stack, STACK_SIZE,
                       ping_entry,
                       NULL, NULL, NULL,
                       PRIORITY, 0, K_NO_WAIT);
    k_tid_t pong_tid = k_thread_create(&pong_thread, pong_stack, STACK_SIZE,
                       pong_entry,
                       NULL, NULL, NULL,
                       PRIORITY, 0, K_NO_WAIT);
    /* TODO: q1 - wait for both threads to finish */
    k_thread_join(ping_tid, K_FOREVER);
    k_thread_join(pong_tid, K_FOREVER);

    printk("counter = %d (expected %d)\n", shared_counter, 2 * ITERATIONS);
    printk("=== Done ===\n");
    return 0;
}




/* SANS MUTEX : 
=== Mutex lab ===
PPionngg

PPionngg

PPionngg

PPionngg

PPionngg

PPionngg

PPionngg

PPionngg

PPionngg

PPionngg

counter = 10 (expected 20)
=== Done ===*/

/* AVEC MUTEX 
Ping
Pong
Ping
Pong
Ping
Pong
Ping
Pong
Ping
Pong
Ping
Pong
Ping
Pong
Ping
Pong
Ping
Pong
Ping
Pong
*/