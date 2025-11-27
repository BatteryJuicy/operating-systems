#include <asm-generic/errno-base.h>
#include <linux/sched.h>
#include <asm/current.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/d_params.h>

/*
This system call returns 0 if the process information has been stored
successfully, or it will return EINVAL in case of an error.
*/
asmlinkage long sys_set_proc_info(int deadline, int est_runtime)
{
    struct task_struct *t = current;

    printk("Andreas Ektoras Thanopoulos\n \
            AM:5401\n \
            syscall name: set_proc_info\n" \
    );
    
    if (deadline <= 0 || est_runtime <= 0){
        printk("SYSCALL <set_proc_info> was called with non-positive argument.\n");
        return EINVAL;
    }

    t->deadline = deadline;
    t->est_runtime = est_runtime;

    return 0;
}

/*
This system call returns 0 if the process information has been stored
successfully, or it will return EINVAL in case of an error.
*/
asmlinkage long sys_get_proc_info(struct d_params *params)
{
    struct task_struct *caller_task = current;
    struct d_params kernel_params;

    printk("Andreas Ektoras Thanopoulos\n \
            AM:5401\n \
            syscall name: get_proc_info\n" \
    );
    
    if (params == NULL) {
        printk("SYSCALL <get_proc_info> was called with NULL argument.\n");
        return EINVAL;
    }
    if (!access_ok(VERIFY_WRITE, params, sizeof(struct d_params))){
        printk("SYSCALL <get_proc_info> cannot access user defined d_params struct.\n");
        return EINVAL;
    }
    
    kernel_params.deadline = caller_task->deadline;
    kernel_params.est_runtime = caller_task->est_runtime;
    
    if (copy_to_user(params, &kernel_params, sizeof(struct d_params))){
        return EINVAL;
    }


    return 0;
}
