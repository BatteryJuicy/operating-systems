# Restaurant Simulation

### Overview
This program simulates a restaurant using **POSIX threads** and **semaphores**.

---

## Program Structure

The program consists of a number of group threads (G) and a waiter thread. Main initializes the restaurant and the threads and then waits for the threads to finish before returning.

### Threads
- **waiter Thread:**
    - Runs a constant check for groups that have arrived and gives them tables to seat,
      if the table's available seats can fit the whole group (not split between 2+ tables).
    - It uses a semaphore to wait for updates from the groups (arrival/departure).
  
- **Group Threads:**
  
    Each group:
  
    1. Waits a random amount of time to simulate arrival time.
    2. Requests a table from the waiter using his semaphore.
    3. waits for the waiter to signal the group's semaphore letting them know a table is ready.
    4. Eats for a random ammount of time.
    5. Leaves and signals the waiter's semaphore that a table is now free so he can check again if any group waiting in queue can be seated.
 
---

## Notes
- **Mutex for stdout:** All `printf` calls are wrapped with `pthread_mutex_lock()`,  `pthread_mutex_unlock()` to avoid race conditions since stdout is shared between threads.
