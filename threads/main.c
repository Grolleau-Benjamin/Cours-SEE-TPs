#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define PRIORITY   5
#define ITERATIONS 10

/* TODO: reserve one stack per thread */
k_thread_stack_t ping_stack[STACK_SIZE];
k_thread_stack_t pong_stack[STACK_SIZE];

/* TODO: declare one thread control block per thread */
struct k_thread ping_thread_data;
struct k_thread pong_thread_data;

/* TODO: q2 - declare the semaphores */
K_SEM_DEFINE(ping_sem, 1, 1);
K_SEM_DEFINE(pong_sem, 0, 1);

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
	k_thread_create(&ping_thread_data, ping_stack, STACK_SIZE,
		       ping_entry, NULL, NULL, NULL,
		       PRIORITY, 0, K_NO_WAIT);
	k_thread_create(&pong_thread_data, pong_stack, STACK_SIZE,
		       pong_entry, NULL, NULL, NULL,
		       PRIORITY, 0, K_NO_WAIT);


	/* TODO: wait for both threads to finish */
	k_thread_join(&ping_thread_data, K_FOREVER);
	k_thread_join(&pong_thread_data, K_FOREVER);

	printk("=== Done ===\n");
	return 0;
}
