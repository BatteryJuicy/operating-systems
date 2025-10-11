/*
    LINKED LIST WITH CONST SIZE ARRAY FOR FLAGS/ARGS FOR EACH NODE IMPLEMENTS MULTIPLE COMMANDS.
*/

#ifndef IO_H
#define IO_H

#define STUDENT_AM 5401
#define MAX_PATH_SIZE 4096
#define MAX_ARG_SIZE 64

typedef struct cmdnode{
    struct cmdnode *next;
    char* line; //whole line unformatted of the whole command with flags
    char** argv; //line split into an array so execvp can read it.
}cmdnode;

void type_prompt();

cmdnode* read_command();
void free_commands(cmdnode*);

#endif