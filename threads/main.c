#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define PRIORITY   5
#define ITERATIONS 10

static int shared_counter;
/* TODO: reserve one stack per thread */
K_THREAD_STACK_DEFINE(ping_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(pong_stack, STACK_SIZE);

/* TODO: declare one thread control block per thread */
K_SEM_DEFINE(sem_ping, 1, 1);
K_SEM_DEFINE(sem_pong, 0, 1);
/* TODO: q2 - declare the semaphores */




static struct k_thread ping_thread_data;
static struct k_thread pong_thread_data;

static void ping_entry(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	for (int i = 0; i < ITERATIONS; i++) {
		/* TODO: q2 - wait for your turn */
		k_sem_take(&sem_ping, K_FOREVER);
		printk("Ping\n");
		k_sem_give(&sem_pong);
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
		k_sem_take(&sem_pong, K_FOREVER);
		printk("Pong\n");
		k_sem_give(&sem_ping);
		/* TODO: q2 - hand the turn over */
	}
}

int main(void)
{
	printk("=== Threads lab ===\n");
	k_tid_t ping_tid, pong_tid;
	
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

	/* TODO: wait for both threads to finish */

	k_thread_join(ping_tid, K_FOREVER);
    k_thread_join(pong_tid, K_FOREVER);

    printk("counter = %d (expected %d)\n", shared_counter, 2 * ITERATIONS);
    


	printk("=== Done ===\n");
	return 0;
}
