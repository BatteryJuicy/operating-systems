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
    new_node->name = name;
    new_node->value = value;

    return new_node;
}

void set_var(const char *name, char *value)
{
    unsigned long index = hash(name);
    var* new_node = create_var(name, value);

    if (globals[index] == NULL){
        globals[index] = new_node;
        return;
    }
    new_node->next = globals[index];
    globals[index] = new_node;
}

char* get_var(const char *name)
{
    unsigned long index = hash(name);
    
    var* result = globals[index];

    while (result != NULL && strcmp(result->name, name)) //while the student hasn't been found and the we aren't at the end of the list.
    {
        result = result->next;
    }
    return result->value;
}

void delete_var(const char *name)
{
    unsigned long index = hash(name);
    
    var* prev = NULL;
    var* p = globals[index];

    while (p != NULL && strcmp(p->name, name) != 0)
    {
        prev = p;
        p = p->next;
    }
    if (p == NULL){
        fprintf(stderr, "No var with name %s exists\n", name);
        return;
    }
    if (prev == NULL) // remove the first student
    {
        globals[index] = p->next;
    }
    else{
        prev->next = p->next;
    }
    free((char*)p->name);
    free(p->value);
    free(p);
}

void free_table()
{

}