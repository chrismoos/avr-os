#ifndef MAX_TASKS
#ifdef ARDUINO

#define MAX_TASKS 5
#define TICK_INTERVAL 10
#define TASK_STACK_SIZE 256

#endif
#endif

struct task_definition {
    void *arg;
    void *address;
    uint8_t running;
    uint8_t done;
    void *original_sp;
    void *saved_sp;
    uint32_t start_delay_secs;
    uint32_t delayMillis;
};
typedef struct task_definition task_definition;

int start_task(void *addr, void *arg, uint16_t start_delay);

extern task_definition tasks[MAX_TASKS];

extern volatile int cur_task;
extern volatile int num_tasks;

extern uint64_t uptime_secs;
extern uint64_t uptime_millis;
