#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <IO.h>
#include <vars.h>

void preprocess_variables(cmdnode* p)
{
    for (int i = 0; p->argv[i] != NULL; i++)
    {
        if(p->argv[i] == NULL) break;

        //if arg starts with $ replace the whole string with the value of the var in globals with name argv[i] + 1
        if (p->argv[i][0] == '$'){
            const char* name = p->argv[i] + 1;
            char* value = get_var(name);
            p->argv[i] = value; // replace with var value or NULL if var doesn't exist
        }    
    }
}

void define_variable(cmdnode* p)
{
    //split cmd between =
    const char* name = strtok((p->argv[0]), "=");
    char* value = strtok(NULL, " ");

    if (value != NULL)
    {
        //create/overrite variable
        set_var(name, value);
    }
}

int check_builin(cmdnode* p, cmdnode* command_list)
{
    //built-in: cd
    if(strcmp(p->argv[0], "cd") == 0){
        if (p->argv[1] == NULL) {
            fprintf(stderr, "cd: no directory provided\n");
        } 
        else if (chdir(p->argv[1]) == -1){
            perror("cd");
        }
        return 1; // flag to go to next command skipping this one
    }
    //built-in; exit
    else if (strcmp(p->argv[0], "exit") == 0){
        free_commands(command_list);
        free_table();
        exit(0);   
    }
    return 0;
}

void execute_pipeline()
{

}

int main(){
    int status;
    while(1)
    {
        type_prompt();
        
        cmdnode* command_list = read_command(); // reads command(s) and appends each command to a SLL

        cmdnode* p = command_list;
        while(p != NULL){

            //-------------preprocess variables-------------

            preprocess_variables(p);
            
            //-------------check for variable declaration-------------

            if (strchr(p->argv[0], '=') != NULL){ //if it contains =
                define_variable(p);
                //going to the next command and continuing to not execute the other if-then's
                p=p->next;
                continue;
            }

            //-------------check for builtins-------------

            if (check_builin(p, command_list) == 1)
            {
                //going to the next command and continuing to not execute the other if-then's
                p=p->next;
                continue;
            }

            //-------------execute command-------------

            pid_t pid = fork();
            if (pid > 0){ //parent wait
                waitpid(pid, &status, 0);
            }
            else if (pid == 0) { //child proccess
                execvp((p->argv)[0], p->argv);

                //terminating child process in case execvp fails
                fprintf(stderr, "%s: command not found\n", p->argv[0]);
                _exit(127); // exiting child process without calling atexit functions of the parent possibly freeing freed memory (same code as bash)
            }
            else{
                perror("fork");
            }

            p=p->next; //process the next command
        }
        // parent frees the command list
        free_commands(command_list);
    }
    return 0;
}