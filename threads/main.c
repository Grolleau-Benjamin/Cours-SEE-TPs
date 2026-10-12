#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define PRIORITY   5
#define ITERATIONS 10

/* TODO: reserve one stack per thread */
K_THREAD_STACK_DEFINE(ping_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(pong_stack, STACK_SIZE);

/* TODO: declare one thread control block per thread */
struct k_thread ping_thread;
struct k_thread pong_thread;
/* TODO: q2 - declare the semaphores */
K_SEM_DEFINE(ping_sem, 1, 1);  /* Ping commence */
K_SEM_DEFINE(pong_sem, 0, 1);  /* Pong attend */

static void ping_entry(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	for (int i = 0; i < ITERATIONS; i++) {
		/* TODO: q2 - wait for your turn */
        k_sem_take(&ping_sem, K_FOREVER);


		printk("Ping\n");

		/* TODO: q2 - hand the turn over */
        k_sem_give(&pong_sem);
	}
}

static void pong_entry(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	for (int i = 0; i < ITERATIONS; i++) {
		/* TODO: q2 - wait for your turn */
        k_sem_take(&pong_sem, K_FOREVER);
		printk("Pong\n");

		/* TODO: q2 - hand the turn over */
        k_sem_give(&ping_sem);
	}
}

int main(void)
{
	printk("=== Threads lab ===\n");

	/* TODO: create both threads */
k_tid_t ping_tid = k_thread_create(&ping_thread, ping_stack, STACK_SIZE,
                ping_entry, NULL, NULL, NULL,
                PRIORITY, 0, K_NO_WAIT);

k_tid_t pong_tid = k_thread_create(&pong_thread, pong_stack, STACK_SIZE,
                pong_entry, NULL, NULL, NULL,
                PRIORITY, 0, K_NO_WAIT);

	/* TODO: wait for both threads to finish */
	k_thread_join(ping_tid, K_FOREVER);
	k_thread_join(pong_tid, K_FOREVER);

	printk("=== Done ===\n");
	return 0;
}
