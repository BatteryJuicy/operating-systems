#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <execute.h>
#include <IO.h>

void execute_command(cmdnode* p, int* status)
{
    pid_t pid = fork();
    if (pid > 0){ //parent wait
        waitpid(pid, status, 0);
    }
    else if (pid == 0) { //child proccess
        execvp((p->argv)[0], p->argv);

        //terminating child process in case execvp fails
        fprintf(stderr, "%s: command not found\n", p->argv[0]);
        _exit(127); // exiting child process without calling atexit functions of the parent possibly freeing freed memory (same exit code as bash)
    }
    else{
        perror("fork");
        exit(-1);
    }
}

cmdnode* reprocess_commands(cmdnode* p)
{
    for (int i = 0; p->argv[i+1] != NULL; i++)
    {
        int j = 0;
        while (p->argv[i][j] != '\0')
            j++;
            
        p->argv[i][j] = ' '; //e.g. line: echo\0test|\0grep\0te ---> echo test| grep te
    }
    
    cmdnode* head = NULL;

    char* save_strtok_ptr; //pointer to store state of strtok because the static char* gets overritten when it's called in create_node()

    char* cmd = strtok_r(p->line, "|", &save_strtok_ptr); // splitting commands before each "|"
    while (cmd != NULL)
    {
        char* cmd_copy = strdup(cmd);
        if(cmd_copy == NULL){
            fprintf(stderr, "Couldn't allocate pipe cmd.\n");
            exit(1);
        }
        cmdnode* new_node =  create_node(cmd_copy);
        append(&head, new_node); //creating a node with cmd as data and appending it to the list of commands
        cmd = strtok_r(NULL, "|", &save_strtok_ptr); // go to the next command and do the same
    }
    return head;
}

void execute_pipeline(cmdnode* p)
{
    cmdnode* pipelist = reprocess_commands(p); // adding the pipeline commands to the list (each node is 1 command)

    int cmd_sum = 0;
    for (cmdnode* q = pipelist; q != NULL; q=q->next){        
        cmd_sum++;
    }
    printf("cmdsum %d\n", cmd_sum);
    int pipes[cmd_sum-1][2];

    //create pipes
    for (int i = 0; i < cmd_sum-1; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            exit(1);
        }
    }
    
    int cmd_index = 0;
    for (cmdnode* q = pipelist; q != NULL; q=q->next, cmd_index++)
    {
        pid_t pid = fork();
        if(pid == -1){
            perror("fork");
            exit(-1);
        }
        if(pid == 0) //child proccess
        {
            if (cmd_index > 0){ //not first command redirect stdin to previous command out
                dup2(pipes[cmd_index-1][0],STDIN_FILENO);
            }

            if (cmd_index < cmd_sum-1){ //not last command redirect stdout to next command in
                dup2(pipes[cmd_index][1], STDOUT_FILENO);
            }

            //close all pipe fds
            for (int i = 0; i < cmd_sum - 1; i++) {
                close(pipes[i][0]);
                close(pipes[i][1]);
            }

            execvp(q->argv[0], q->argv);
            perror(q->argv[0]);
            exit(1);
        }
    }

    //parent closes all pipes
    for (int i = 0; i < cmd_sum - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    //parent waits for children to finish
    for (int i = 0; i < cmd_sum; i++) {
        wait(NULL);
    }
    
    free_commands(pipelist);
}