#ifndef IO_H
#define IO_H

#define STUDENT_AM 5401
#define MAX_PATH_SIZE 4096
#define MAX_ARG_SIZE 64

struct cmdnode{
    struct cmdnode *next;
    char* line; //whole line unformatted of the whole command with flags
    char** argv; //line split into an array so execvp can read it.
};

void type_prompt();

struct cmdnode* read_command();
void free_commands(struct cmdnode*);

#endif