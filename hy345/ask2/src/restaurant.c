#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <restaurant.h>

#define ARRIVAL_TIME_MAX 5
#define ARRIVAL_TIME_MIN 1

#define EATING_TIME_MAX 15
#define EATING_TIME_MIN 5

int N; //Nubmer of Tables
int G; //Number of Groups
int X; //Capacity of each Table and cap of Group Size

Table *tables; //table array
Group *groups; //group queue
int groups_done = 0;

pthread_mutex_t mutex;
sem_t waiter_sem;

void init_restaurant(int argc, char* argv[])
{
    srand((unsigned int)time(NULL)); //seed the RNG.

    //initialize program arguments and check values.
    if (argc != 4)
    {
        fprintf(stderr, "Invalid number of parameters.\n");
        exit(EXIT_FAILURE);
    }

    N = atoi(argv[1]);
    G = atoi(argv[2]);
    X = atoi(argv[3]);

    if (N < 1 || G < 1 || X < 1)
    {
        fprintf(stderr, "Invalid Parameter types or values.\n");
        exit(EXIT_FAILURE);
    }
    
    //----------initialize objects----------

    tables = calloc((size_t)N, sizeof(Table));
    for (int i = 0; i < N; i++)
    {
        tables[i].tid = i;
        tables[i].capacity = X;
        tables[i].seats_taken = 0;
    }
    groups = calloc((size_t)G, sizeof(Group));
    for (int i = 0; i < G; i++)
    {
        groups[i].gid = i;
        groups[i].seated = false;
        groups[i].arrived = false;
        sem_init(&groups[i].sem, 0, 0);
    }

    pthread_mutex_init(&mutex, NULL);
    sem_init(&waiter_sem, 0, 0);
}

void* group_thread(void* arg)
{
    Group* g = (Group*)arg;

    //sleep for [ARRIVAL_TIME_MIN, ARRIVAL_TIME_MAX) time.
    unsigned int arrival_delay = (unsigned int)rand()%(ARRIVAL_TIME_MAX - ARRIVAL_TIME_MIN) + ARRIVAL_TIME_MIN;
    sleep(arrival_delay);

    g->size = rand()%(X - 0) + 1; //size from [1, X)

    pthread_mutex_lock(&mutex);
    g->arrived = true;
    printf("[GROUP %d] Arrived with %d people (after %d sec)\n", g->gid, g->size, arrival_delay);
    pthread_mutex_unlock(&mutex);

    //signal the waiter that the group arrived
    sem_post(&waiter_sem);

    //wait for waiter's acknowledgment that a table is ready
    sem_wait(&g->sem);

    pthread_mutex_lock(&mutex);
    printf("[GROUP %d] Seated at Table %d with %d people\n", g->gid, g->table_assigned, g->size);
    pthread_mutex_unlock(&mutex);

    unsigned int eating_time = (unsigned int)rand()%(EATING_TIME_MAX - EATING_TIME_MIN) + EATING_TIME_MIN;

    pthread_mutex_lock(&mutex);
    printf("[GROUP %d] Eating (%d people) for %d seconds...\n", g->gid, g->size, eating_time);
    pthread_mutex_unlock(&mutex);
    
    //sleep for [EATING_TIME_MIN, EATING_TIME_MAX) time.
    sleep(eating_time);

    pthread_mutex_lock(&mutex);
    tables[g->table_assigned].seats_taken -= g->size;
    printf("[GROUP %d] Left Table %d (%d/%d occupied)\n",
            g->gid,
            g->table_assigned,
            tables[g->table_assigned].seats_taken,
            tables[g->table_assigned].capacity);
    
    groups_done++;
    pthread_mutex_unlock(&mutex);
    
    sem_post(&waiter_sem); //tell the waiter the group left the table
    return NULL;
}

void* waiter_thread(void*)
{
    while(true)
    {
        //stop if all groups have left.
        pthread_mutex_lock(&mutex);
        if (groups_done == G){
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);

        sem_wait(&waiter_sem); // wait for a group to arrive or leave

        pthread_mutex_lock(&mutex);
        //for each group check if it's not seated and accomodate them or ignore them
        for (int i = 0; i < G; i++)
        {
            Group* g = &groups[i];

            if (g->seated == true) continue;
            if (g->arrived == false)continue;

            //for each table check if the group can be seated and seat them.
            for (int j = 0; j < N; j++)
            {
                Table* t = &tables[j];

                if (t->capacity - t->seats_taken >= g->size){
                    //seat the group
                    t->seats_taken += g->size;
                    g->seated = true;
                    g->table_assigned = t->tid;
                    printf("[WAITER] Assigned group %d (size=%d) to Table %d (%d/%d occupied)\n",
                            g->gid,
                            g->size,
                            t->tid,
                            t->seats_taken,
                            t->capacity);
                    //signal the group that they have been seated.
                    sem_post(&g->sem);
                    //break to check the next group.
                    break;
                }
            }
        }
        pthread_mutex_unlock(&mutex); //unlock the mutex to allow the group threads to work.
    }

    //no mutex lock needed since this get's executed when all group threads have been closed.
    printf("All groups have left. Closing restaurant.\n");
    return NULL;
}

void cleanup_restaurant()
{
    pthread_mutex_destroy(&mutex);
    sem_destroy(&waiter_sem);
    for (int i = 0; i < G; i++)
    {
        sem_destroy(&groups[i].sem);
    }
    free(groups);
    free(tables);    
}