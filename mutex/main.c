
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define PRIORITY   5
#define ITERATIONS 10

static int shared_counter;

/* Q2: Declare the mutex */
K_MUTEX_DEFINE(counter_mutex);

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
        /* Enter the critical section */
        k_mutex_lock(&counter_mutex, K_FOREVER);

        int local = shared_counter;

        print_word(name);
        k_sleep(K_MSEC(5));

        shared_counter = local + 1;

        /* Leave the critical section */
        k_mutex_unlock(&counter_mutex);

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

/* Q1: Reserve one stack per thread */
K_THREAD_STACK_DEFINE(ping_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(pong_stack, STACK_SIZE);

/* Q1: Declare one thread control block per thread */
static struct k_thread ping_thread_data;
static struct k_thread pong_thread_data;

int main(void)
{
    printk("=== Mutex lab ===\n");

    /* Q1: Create both threads at the same priority */
    k_tid_t ping_tid = k_thread_create(
        &ping_thread_data,
        ping_stack,
        K_THREAD_STACK_SIZEOF(ping_stack),
        ping_entry,
        NULL, NULL, NULL,
        PRIORITY, 0, K_NO_WAIT);

    k_tid_t pong_tid = k_thread_create(
        &pong_thread_data,
        pong_stack,
        K_THREAD_STACK_SIZEOF(pong_stack),
        pong_entry,
        NULL, NULL, NULL,
        PRIORITY, 0, K_NO_WAIT);

    /* Q1: Wait for both threads to finish */
    k_thread_join(ping_tid, K_FOREVER);
    k_thread_join(pong_tid, K_FOREVER);

    printk("counter = %d (expected %d)\n",
           shared_counter, 2 * ITERATIONS);
    printk("=== Done ===\n");

    return 0;
}