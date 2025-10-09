#include <IO.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <assert.h>

void type_prompt()
{
    struct passwd* pw = getpwuid(getuid());

    // username
    char* username = pw ? pw->pw_name : "user"; // according to man it could return NULL

    // current working direcotry
    char cwd[MAX_PATH_SIZE];
    assert(getcwd(cwd, MAX_PATH_SIZE));

    printf("%s@%d-hy345sh:%s$", username, STUDENT_AM, cwd);
}

void append(struct cmdnode** headptr, struct cmdnode* cmd)
{
    //iterating the linked list..
    struct cmdnode* prevp = NULL;
    struct cmdnode* p = *headptr;
    while(p != NULL)
    {
        prevp = p;
        p = p->next;
    }
    
    //if list is empty.
    if (prevp == NULL)
    {
        *headptr = cmd;
        return;
    }

    //otherwise.
    cmd->next = NULL;
    prevp->next = cmd;
}

/* Takes in a string and returns an array made of that string split at every space (ignoring double spaces)
   The last element of the array is NULL to abide by the execvp format.
*/
char** split_command(char* command)
{
    char** argv = malloc(MAX_ARG_SIZE*sizeof(char*));
    if (!argv) { 
        fprintf(stderr, "cannot allocate arguments for command %s\n", command); exit(1);
    }

    while (*command == ' ') command++; // skipping spaces

    int argc = 0;
    char *arg = strtok(command, " ");
    while (arg != NULL && argc < MAX_ARG_SIZE) {
        argv[argc++] = arg;  // pointers into command string
        arg = strtok(NULL, " ");
    }
    argv[argc] = NULL; // NULL-terminate for execvp
    return argv;
}

//creates a node
struct cmdnode* create_node(char* cmd)
{
    struct cmdnode* new_node = (struct cmdnode*) malloc(sizeof(struct cmdnode));
    if(new_node == NULL){
        fprintf(stderr, "cannot allocate command node");
        exit(-1);
    }
    new_node->next = NULL;
    new_node->line = cmd;
    new_node->argv = split_command(cmd); //formatting the command

    return new_node;
}

struct cmdnode* read_command()
{
    char *commands = NULL;
    size_t len = 0; // buffer size (gets updated by getline)
    ssize_t n = getline(&commands, &len, stdin); // reads the entire line and stores the number of chars in n
    if(n == -1){ // EOF probably
        return NULL;
    }
    if (commands[n-1] == '\n'){
        commands[n-1] = '\0'; //removing newline
    }

    struct cmdnode* head = NULL;

    printf("commands %s\n", commands);

    char* cmd = strtok(commands, ";"); // splitting commands before each ";"
    while (cmd != NULL)
    {
        char* cmd_copy = strdup(cmd);
        if(cmd_copy == NULL){
            fprintf(stderr, "Couldn't allocate cmd.\n");
            exit(1);
        }
        struct cmdnode* command_node =  create_node(cmd_copy);
        //append(&head, command_node); //creating a node with cmd as data and appending it to the list of commands
        printf("command: %s\n", cmd);
        cmd = strtok(NULL, ";"); // go to the next command and do the same

        printf("command2: %s\n", cmd);
    }
    
    free(commands);

    return head;
}

void free_commands(struct cmdnode* first)
{
    struct cmdnode* p = first;
    struct cmdnode* tmp;
    while (p != NULL) {
        tmp = p->next;
        free(p->argv);
        free(p->line);
        free(p);
        p = tmp;
    }
}