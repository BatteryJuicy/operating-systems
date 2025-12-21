## Modified Files

### 1. `include/linux/sched.h`

Two new fields were added to the `task_struct`:
- `int deadline`
- `int est_runtime`

These fields store process-specific scheduling parameters required by the implemented policy.

---

### 2. `include/asm-generic/syscalls.h`

Declarations for the two custom system calls were added, enabling their visibility across the kernel.

---

### 3. `arch/x86/include/asm/unistd_32.h`

The syscall invocation numbers for the two new system calls were defined, and the total number of system calls was updated accordingly.

---

### 4. `arch/x86/kernel/syscall_table_32.S`

Entries for the newly implemented system calls were added to the syscall table, allowing the kernel to correctly dispatch them.

---

### 5. `kernel/Makefile`

The object file corresponding to the new syscall implementation was added to the build system so that it is compiled and linked into the kernel.

---

### 6. `include/linux/d_params.h`

The `d_params` structure was defined to facilitate communication between the two system calls.

---

## Created Files

### 1. `kernel/proc_info.c`

This file contains the implementation of the two custom system calls.

#### i) `sys_set_proc_info(int deadline, int est_runtime)`

This system call:
- Obtains a pointer to the calling process’s `task_struct`,
- Prints diagnostic information, including the function name,
- Verifies that both parameters are strictly positive,
- Updates the corresponding fields in the caller’s `task_struct`.

#### ii) `sys_get_proc_info(struct d_params *params)`

This system call:
- Obtains a pointer to the calling process’s `task_struct`,
- Prints diagnostic information, including the function name,
- Creates a temporary `d_params` structure in kernel space,
- Verifies that the user-space pointer was passed correctly,
- Copies the relevant values from the caller’s `task_struct` into the temporary structure and returns them to user space.

---

### 2. `execution/test.c`

A user-space test program that:
- Defines wrapper functions for the two system calls,
- Verifies correct behavior under normal conditions,
- Tests edge cases and invalid inputs to ensure robust error handling.

---

### 3. `execution/Makefile`

A minimal Makefile used to compile and clean the `test.c` program.

---

## Note

The `d_params` structure exists exclusively in kernel space. Consequently, any user-space program that invokes the new system calls must define an identical structure to ensure correct data exchange between user space and kernel space.
