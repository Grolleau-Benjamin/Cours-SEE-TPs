#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE     1024
#define PRIORITY       5
#define WAKER_PRIORITY (PRIORITY - 1)
#define ITERATIONS     10

static int shared_counter;

K_SEM_DEFINE(sem, 1, 1); // 1 valeur init et 1 max

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

		k_sem_take(&sem, K_FOREVER);

        int local = shared_counter;

        print_word(name);
        k_sleep(K_MSEC(5));

        shared_counter = local + 1;

        k_sem_give(&sem);

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

static void waker_entry(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    for (int i = 0; i < 20; i++) {
        k_sleep(K_MSEC(20));
        k_sem_give(&sem);
    }
}

K_THREAD_STACK_DEFINE(ping_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(pong_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(waker_stack, STACK_SIZE);

static struct k_thread ping_thread_data;
static struct k_thread pong_thread_data;
static struct k_thread waker_thread_data;

int main(void)
{
    k_tid_t ping_tid, pong_tid, waker_tid;

    printk("=== Semaphore lab ===\n");

    ping_tid = k_thread_create(&ping_thread_data,
                   ping_stack, K_THREAD_STACK_SIZEOF(ping_stack),
                   ping_entry,
                   NULL, NULL, NULL,
                   PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(ping_tid, "ping");

    pong_tid = k_thread_create(&pong_thread_data,
                   pong_stack, K_THREAD_STACK_SIZEOF(pong_stack),
                   pong_entry,
                   NULL, NULL, NULL,
                   PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(pong_tid, "pong");

	waker_tid = k_thread_create(&waker_thread_data,
				waker_stack, K_THREAD_STACK_SIZEOF(waker_stack),
				waker_entry,
				NULL, NULL, NULL,
				WAKER_PRIORITY, 0, K_NO_WAIT);

	k_thread_name_set(waker_tid, "waker");

    k_thread_join(ping_tid, K_FOREVER);
    k_thread_join(pong_tid, K_FOREVER);
	k_thread_join(waker_tid, K_FOREVER);

    printk("counter = %d (expected %d)\n", shared_counter, 2 * ITERATIONS);
    printk("=== Done ===\n");
    return 0;
}