#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <execute.h>
#include <IO.h>
#include <vars.h>

void preprocess_variables(cmdnode* p)
{
    for (int i = 0; p->argv[i] != NULL; i++)
    {
        int ampersand_flag = 0;

        //if arg starts with $ replace the whole string with the value of the var in globals with name argv[i] + 1
        char new_arg[1024] = {0};
        int j = 0;
        while (p->argv[i][j] != '\0'){
            if (p->argv[i][j] == '$'){
                ampersand_flag = 1;
                const char* name = &(p->argv[i][j+1]); //start after $
                char* value;

                //replace next $ with \0 so get_var doesn't read the rest of the string
                int EOF_flag = 1;
                int k;
                for (k = j+1; p->argv[i][k] != '\0'; k++)
                {
                    if(p->argv[i][k] == '$'){
                        p->argv[i][k] = '\0';
                        EOF_flag = 0;
                        break;
                    }
                }
                //check if variable is environment var else check if it's defined in this process
                if ((value = getenv(name)) == 0)
                    value = get_var(name);

                if (!EOF_flag)
                    p->argv[i][k] = '$'; //restore string for the next repetition

                if (value != NULL)
                    strcat(new_arg, value); // concatinate the new value
            }
            j++;
        }
        if (ampersand_flag)
            p->argv[i] = strdup(new_arg); //replace old string with substituted version. Memory leak but don't have time to fix
    }
}

void execute_command(cmdnode* p, int* status)
{
    pid_t pid = fork();
    if (pid > 0){ //parent wait
        waitpid(pid, status, 0);
    }
    else if (pid == 0) { //child proccess

        //restore default behavior for cntrl+c
        signal(SIGINT, SIG_DFL);

        //check for I/O redirection
        redirect_io(p);

        preprocess_variables(p);

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

    preprocess_variables(p);

    int cmd_sum = 0;
    for (cmdnode* q = pipelist; q != NULL; q=q->next){
        cmd_sum++;
    }

    if (cmd_sum < 2){fprintf(stderr, "fewer than 2 pipeline commands: %d\n", cmd_sum);exit(1);}
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
            //restore default behavior for cntrl+c
            signal(SIGINT, SIG_DFL);

            if (cmd_index == 0 || cmd_index == cmd_sum - 1){ //first command or last command redirect io
                redirect_io(q);
            }

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

int redirect_io(cmdnode* p)
{
    int fd;
    for (int i = 0; p->argv[i] != NULL; i++)
    {
        if (strcmp(p->argv[i], "<") == 0) { // input redirection
            if (p->argv[i+1] == NULL) {
                fprintf(stderr, "Error: no input file specified\n");
                return -1;
            }
            fd = open(p->argv[i+1], O_RDONLY);
            if (fd == -1) {
                perror("open");
                return -1;
            }
            dup2(fd, STDIN_FILENO); // redirect stdin
            close(fd);

            //remove the redirection from argv so execvp works
            for (int j = i; p->argv[j-1] != NULL; j++) {
                p->argv[j] = p->argv[j+2];
            }
            i--; // adjust index
        }
        else if (strcmp(p->argv[i], ">") == 0) { // output redirection (truncate)
            if (p->argv[i+1] == NULL) {
                fprintf(stderr, "Error: no output file specified\n");
                return -1;
            }
            fd = open(p->argv[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                perror("open");
                return -1;
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);

            for (int j = i; p->argv[j-1] != NULL; j++) {
                p->argv[j] = p->argv[j+2];
            }
            i--;
        }
        else if (strcmp(p->argv[i], ">>") == 0) { // output redirection (append)
            if (p->argv[i+1] == NULL) {
                fprintf(stderr, "Error: no output file specified\n");
                return -1;
            }
            fd = open(p->argv[i+1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd == -1) {
                perror("open");
                return -1;
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);

            for (int j = i; p->argv[j-1] != NULL; j++) {
                p->argv[j] = p->argv[j+2];
            }
            i--;
        }
    }
    return 0;
}