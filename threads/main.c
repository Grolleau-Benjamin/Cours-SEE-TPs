#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define PRIORITY   5
#define ITERATIONS 10

// Def piles
K_THREAD_STACK_DEFINE(ping_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(pong_stack, STACK_SIZE);

static struct k_thread ping_thread;
static struct k_thread pong_thread;

K_SEM_DEFINE(sem_ping, 1, 1);
K_SEM_DEFINE(sem_pong, 0, 1);

static void ping_entry(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	for (int i = 0; i < ITERATIONS; i++) {

        k_sem_take(&sem_ping, K_FOREVER);
        printk("Ping\n");
        k_sem_give(&sem_pong);
    }
}

static void pong_entry(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	for (int i = 0; i < ITERATIONS; i++) {

        k_sem_take(&sem_pong, K_FOREVER);
        printk("Pong\n");
        k_sem_give(&sem_ping);
    }
}

int main(void)
{
	printk("=== Threads lab ===\n");

	k_thread_create(&ping_thread, ping_stack,
                    K_THREAD_STACK_SIZEOF(ping_stack),
                    ping_entry, NULL, NULL, NULL,
                    PRIORITY, 0, K_NO_WAIT);
	
	k_thread_create(&pong_thread, pong_stack,
                    K_THREAD_STACK_SIZEOF(pong_stack),
                    pong_entry, NULL, NULL, NULL,
                    PRIORITY, 0, K_NO_WAIT);
	
	k_thread_join(&ping_thread, K_FOREVER);
    k_thread_join(&pong_thread, K_FOREVER);

	printk("=== Done ===\n");
	
	return 0;
}