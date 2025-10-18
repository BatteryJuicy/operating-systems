/*
    LINKED LIST WITH CONST SIZE ARRAY WITH NULL TERMINATED STRINGS FOR 
    FLAGS/ARGS POINTING TO PLACES ON LINE FOR EACH NODE,
    IMPLEMENTS MULTIPLE COMMANDS. EACH NODE ALLOCATES LINE STRING AND ARGV ARRAY.
    ARGV[I] GETS INITIALIZED TO NULL.
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
cmdnode* read_commands();
void free_commands(cmdnode*);

cmdnode* create_node(char* cmd);
void append(cmdnode** headptr, cmdnode* cmd);

#endif