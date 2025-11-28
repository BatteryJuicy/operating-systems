files modified:

1. include/linux/sched.h

    Added an int deadline and an int est_runtime attribute to the task_struct.

2. include/asm-generic/syscalls.h

    Added the declarations of the custom syscalls.

3. arch/x86/include/asm/unistd_32.h

    Defined the 2 sycall invocation number and update the total number of syscalls.

4. arch/x86/kernel syscall_table_32.S

    Added a pointer for each new syscall so the kernel can find them.

5. kernel/Makefile

        Added the object file of the source code for the 2 syscalls so it gets
        compiled and linked.
    
6. include/linux/d_params.h

    Defined the d_params struct that allows communication between the 2 
    syscalls.

7. kernel/proc_info.c

    Implements the custom syscalls.
    
    i) sys_set_proc_info(int deadline, int est_runtime)
        
        It gets a pointer to the caller, prints my info and the function name, 
        checks that the parameters satisfy that they are potitive and changes the 
        values of the task_struct of the caller.
    
    ii) sys_get_proc_info(struct d_params *params)
    
            It gets a pointer to the caller, prints my info and the function name, 
            creates temporary d_params struct, checks that the parameter was  passed 
            correctly from user to kernel space and modifies its values using the 
            temp d_params struct and the caller's task_struct.

8. execution/test.c

    Tests that the syscalls handle all edge cases. Also defines wrapper 
    functions for the 2 syscalls.

9. execution/makefile

    A simple makefile to compile and clean the test.c nothing worth mentioning.

##Notes

    struct d_params only exists in the kernel space. To use the 2 new syscalls 
    the user must define an identical struct in his implementation.

