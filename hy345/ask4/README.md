# Overview

This assignment builds upon **Assignment 3** and reuses the same modified files.  
The Linux scheduler was extended to support an alternative scheduling policy called **Least Tolerance First (LTF)**.

Under the LTF policy, a process may request deadline-based scheduling by specifying:
- a **deadline**, and
- an **estimated execution time**.

The scheduler always selects and executes the **most urgent process**.  
Urgency is determined by the **tolerance** value, defined as:

**tolerance = (deadline − current_time) / remaining_execution_time**

A smaller tolerance indicates a more urgent process.

---

## Modified Files

### 1. `kernel/fork.c`
During process creation, the following fields are initialized to zero:
- `deadline`
- `est_runtime`

---

### 2. `kernel/sched.c`
The scheduler was extended with:
- A function that identifies processes with defined deadlines and applies the **LTF scheduling algorithm**.
- A helper function that retrieves the current system time in **nanoseconds**, which is used in tolerance and urgency calculations.

---

## Created Files

### 1. Test Files
A collection of test programs was implemented to validate the correctness and behavior of the LTF scheduling policy.

---

### 2. `execution/Makefile`
A minimal `Makefile` used to compile and clean the test programs.

---

## Note

The `d_params` structure exists **exclusively in kernel space**.  
Any user-space program that invokes the new system calls must define an **identical structure** to ensure correct and consistent data exchange between user space and kernel space.
