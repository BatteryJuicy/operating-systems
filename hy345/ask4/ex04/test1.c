#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>

#define __NR_set_proc_info 341
#define __NR_get_proc_info 342

struct d_params{
    int deadline;
    int est_runtime;
};

static inline long set_proc_info(long deadline_ns, long exec_ns)
{
    return syscall(__NR_set_proc_info, deadline_ns, exec_ns);
}

static inline long now_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000L + ts.tv_nsec;
}

void spin(void)
{
    volatile unsigned long x = 0;
    while (1) x++;
}

int main(void)
{
    long start = now_ns();
    long deadline = start + 5L * 1000000000L;  // 5s
    long exec     = 2L * 1000000000L;          // 2s

    printf("[test1] pid=%d deadline=5s exec=2s\n", getpid());
    set_proc_info(deadline, exec);

    spin();
    return 0;
}
