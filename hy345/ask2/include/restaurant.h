#ifndef RESTAURANT_H
#define RESTAURANT_H

#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>

typedef struct
{
    int tid;
    int capacity;
    int seats_taken;
} Table;

typedef struct
{
    int gid;
    int size;
    bool seated;
    bool arrived;
    int table_assigned;
    sem_t sem;
} Group;

//Global variables
extern int N;
extern int G;
extern int X;

extern Table *tables;
extern Group *groups;
extern int groups_done;

extern pthread_mutex_t mutex;
extern sem_t waiter_sem;

void init_restaurant(int argc, char* argv[]);
void* group_thread(void* arg);
void* waiter_thread(void* arg);
void cleanup_restaurant();

#endif