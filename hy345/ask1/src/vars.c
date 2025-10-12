#include <vars.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

var* globals[TABLE_SIZE] = {0};

unsigned long hash(const char *str) //sdbm
{
    unsigned long hash = 0;
    unsigned long c;

    while ((c = (unsigned char)*str++))
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

var* create_var(const char *name, char *value)
{
    var* new_node = (var*) malloc(sizeof(var));
    if(new_node == NULL){
        fprintf(stderr, "cannot allocate var node");
        exit(-1);
    }
    new_node->next = NULL;
    new_node->name = strdup(name);
    new_node->value = strdup(value);

    return new_node;
}

void set_var(const char *name, char *value)
{
    unsigned long index = hash(name)%TABLE_SIZE;

    //if not empty check if variabe is already declared and overrite previous value
    var *p = globals[index];
    while (p != NULL)
    {
        if (strcmp(p->name, name) == 0){
            free(p->value); //free old string
            p->value = strdup(value); //allocate new string
            return;
        }
        p = p->next;
    }

    var* new_node = create_var(name, value);

    //add bucket as the first element of the linked list
    new_node->next = globals[index];
    globals[index] = new_node;
}

char* get_var(const char *name)
{
    unsigned long index = hash(name)%TABLE_SIZE;
    
    var* result = globals[index];

    while (result != NULL && strcmp(result->name, name) != 0) 
    {
        result = result->next;
    }
    if(result == NULL)
        return NULL;
    return result->value;
}

void delete_var(const char *name)
{
    unsigned long index = hash(name)%TABLE_SIZE;
    
    var* prev = NULL;
    var* p = globals[index];

    //search the SLL for the wanted node
    while (p != NULL && strcmp(p->name, name) != 0)
    {
        prev = p;
        p = p->next;
    }
    if (p == NULL){
        fprintf(stderr, "No var with name %s exists\n", name);
        return;
    }
    if (prev == NULL) // remove the first node
    {
        globals[index] = p->next;
    }
    else{ //node found. reassign node pointers before free node
        prev->next = p->next;
    }
    free((char*)p->name);
    free(p->value);
    free(p);
}

void free_table()
{
    //free all SLL's (globals doesn't need free)
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        var* p = globals[i];
        while (p != NULL)
        {
            globals[i] = p->next; //storing next node in SLL (if it doesn't exist it's null)
            //freeing node data and node
            free((char*)p->name);
            free(p->value);
            free(p);
            p=globals[i]; //setting p to the old second now first node of the SLL
        }
    }
}