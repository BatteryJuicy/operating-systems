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

#include <sys/wait.h>

int main(void)
{
    pid_t p1, p2;

    if ((p1 = fork()) == 0) {
        long d = now_ns() + 10L * 1000000000L;
        long e = 5L  * 1000000000L;
        printf("[test3:P1] pid=%d D=10 E=5\n", getpid());
        set_proc_info(d, e);
        spin();
    }

    if ((p2 = fork()) == 0) {
        long d = now_ns() + 11L * 1000000000L;
        long e = 4L  * 1000000000L;
        printf("[test3:P2] pid=%d D=11 E=4\n", getpid());
        set_proc_info(d, e);
        spin();
    }

    wait(NULL);
    wait(NULL);
    return 0;
}
