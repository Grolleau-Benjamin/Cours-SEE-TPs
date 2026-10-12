#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define PRIORITY   5
#define ITERATIONS 10

static int shared_counter;

/* TODO: q2 - declare the mutex */

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

		int local = shared_counter;

		print_word(name);
		k_sleep(K_MSEC(5));

		shared_counter = local + 1;

		/* TODO: q2 - leave the critical section */

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
k_thread_stack_t ping_stack[STACK_SIZE];
k_thread_stack_t pong_stack[STACK_SIZE];

/* TODO: q1 - declare one thread control block per thread */
struct k_thread ping_thread_data;
struct k_thread pong_thread_data;

int main(void)
{
	printk("=== Mutex lab ===\n");

	/* TODO: q1 - create both threads, same priority */
	thread_create(&ping_thread_data, ping_stack, STACK_SIZE,
		       ping_entry, NULL, NULL, NULL,
		       PRIORITY, 0, K_NO_WAIT);
	thread_create(&pong_thread_data, pong_stack, STACK_SIZE,
		       pong_entry, NULL, NULL, NULL,
		       PRIORITY, 0, K_NO_WAIT);

	/* TODO: q1 - wait for both threads to finish */
	k_thread_join(&ping_thread_data, K_FOREVER);
	k_thread_join(&pong_thread_data, K_FOREVER);

	printk("counter = %d (expected %d)\n", shared_counter, 2 * ITERATIONS);
	printk("=== Done ===\n");
	return 0;
}
