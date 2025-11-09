/*============================================================================
    Author:      Andreas Ektoras Thanopoulos
    AM:          5401
============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <restaurant.h>
#include <pthread.h>

int N; //Nubmer of Tables
int G; //Number of Groups
int X; //Capacity of each Table and cap of Group Size

Table *tables; //table array
Group *groups; //group queue
int groups_top = 0;

pthread_mutex_t mutex;
sem_t waiter_sem;

int main(int argc, char* argv[])
{
    init_restaurant(argc, argv);

    pthread_t waiter;
    pthread_t group_threads[G];

    pthread_create(&waiter, NULL, waiter_thread, NULL); // create the waiter thread.
    for (int i = 0; i < G; i++)
    {
        pthread_create(&group_threads[i], NULL, group_thread, &groups[i]); //create group threads and pass a pointer to each group that corresponds to the thread.
    }
    
    // wait for all threads to finish
    for (int i = 0; i < G; i++)
    {
        pthread_join(group_threads[i], NULL);
    }
    pthread_join(waiter, NULL);

    cleanup_restaurant();
    return 0;
}
