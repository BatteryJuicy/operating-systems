/*
    HASH TABLE IMPLEMENTS GLOBAL VARIABLES.
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

void set_var(const char *name, char *value);
char* get_var(const char *name);
void delete_var(const char *name);
void free_table();

#endif