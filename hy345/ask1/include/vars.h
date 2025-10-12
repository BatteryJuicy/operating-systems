/*
    HASH TABLE WITH SDBM HASH FUNCTION AND SLL TO HANDLE COLLISIONS IMPLEMENTS GLOBAL VARIABLES. 
    Reusing modified code from HY240 I wrote (https://github.com/BatteryJuicy/data-structures)
*/

#ifndef VARS_H
#define VARS_H

#define TABLE_SIZE 211

typedef struct var{
    const char* name;
    char* value;
    struct var* next;
}var;

extern var* globals[TABLE_SIZE]; //init all buckets to 0/NULL

// create var (call strdup on name, value), append to the SLL of the appropriate globals index
void set_var(const char *name, char *value);
// return pointer to value of var if name is invalid returns NULL
char* get_var(const char *name);
// removed var from SLL
void delete_var(const char *name);
void free_table();

#endif