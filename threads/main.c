#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define PRIORITY   5
#define ITERATIONS 10

/* TODO: reserve one stack per thread */

/* TODO: declare one thread control block per thread */

/* TODO: q2 - declare the semaphores */

static void ping_entry(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	for (int i = 0; i < ITERATIONS; i++) {
		/* TODO: q2 - wait for your turn */

		printk("Ping\n");

		/* TODO: q2 - hand the turn over */
	}
}

static void pong_entry(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	for (int i = 0; i < ITERATIONS; i++) {
		/* TODO: q2 - wait for your turn */

		printk("Pong\n");

		/* TODO: q2 - hand the turn over */
	}
}

int main(void)
{
	printk("=== Threads lab ===\n");

	/* TODO: create both threads */

	/* TODO: wait for both threads to finish */

	printk("=== Done ===\n");
	return 0;
}
