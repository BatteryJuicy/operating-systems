#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <assert.h>
#include <IO.h>

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

void append(cmdnode** headptr, cmdnode* cmd)
{
    //iterating the linked list..
    cmdnode* prevp = NULL;
    cmdnode* p = *headptr;
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
    char** argv = calloc(MAX_ARG_SIZE, sizeof(char*));
    if (!argv) { 
        fprintf(stderr, "cannot allocate arguments for command %s\n", command);
        exit(1);
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
cmdnode* create_node(char* cmd)
{
    cmdnode* new_node = (cmdnode*) malloc(sizeof(cmdnode));
    if(new_node == NULL){
        fprintf(stderr, "cannot allocate command node");
        exit(-1);
    }
    new_node->next = NULL;
    new_node->line = cmd;
    new_node->argv = split_command(cmd); //formatting the command

    return new_node;
}

cmdnode* read_commands()
{
    cmdnode* head = NULL;
    
    char *commands = NULL;
    size_t len = 0; // buffer size (gets updated by getline)
    ssize_t n = getline(&commands, &len, stdin); // reads the entire line and stores the number of chars in n

    //check if/for and do a loop with getline and create cmdnodes

    if(n == -1){ // EOF probably
        return NULL;
    }
    if (commands[n-1] == '\n'){
        commands[n-1] = '\0'; //removing newline
    }

    char* save_strtok_ptr; //pointer to store state of strtok because the static char* gets overritten when it's called in create_node()

    char* cmd = strtok_r(commands, ";", &save_strtok_ptr); // splitting commands before each ";"
    while (cmd != NULL)
    {
        char* cmd_copy = strdup(cmd);
        if(cmd_copy == NULL){
            fprintf(stderr, "Couldn't allocate cmd.\n");
            exit(1);
        }
        cmdnode* command_node =  create_node(cmd_copy);
        append(&head, command_node); //creating a node with cmd as data and appending it to the list of commands
        cmd = strtok_r(NULL, ";", &save_strtok_ptr); // go to the next command and do the same
    }
    
    free(commands);

    return head;
}

void free_commands(cmdnode* first)
{
    cmdnode* p = first;
    cmdnode* tmp;
    while (p != NULL) {
        tmp = p->next;
        free(p->argv);
        free(p->line);
        free(p);
        p = tmp;
    }
}