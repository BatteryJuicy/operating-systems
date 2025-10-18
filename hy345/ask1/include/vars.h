/*
    HASH TABLE WITH SDBM HASH FUNCTION AND SLL TO HANDLE COLLISIONS,
    TO IMPLEMENT GLOBAL VARIABLES. EACH VAR ALLOCATES NAME AND VALUE.
*/

#ifndef VARS_H
#define VARS_H

#define TABLE_SIZE 211

typedef struct var{
    const char* name;
    char* value;
    struct var* next;
}var;

extern var* globals[TABLE_SIZE];

// create var (call strdup on name, value), append to the SLL of the appropriate globals index
void set_var(const char *name, char *value);
// return pointer to value of var if name is invalid returns NULL
char* get_var(const char *name);
// removed var from SLL
void delete_var(const char *name);
void free_table();

#endif