#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <IO.h>
#include <vars.h>
#include <execute.h>
#include <controlflow.h>

int check_builtin(cmdnode* p, cmdnode* command_list)
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
    //built-in: export
    else if (strcmp(p->argv[0], "export") == 0){
        const char* name = p->argv[1];
        char* value = get_var(name);
        if(value == NULL){
            fprintf(stderr, "export: variable '%s' not found\n", name);
            return 1;
        }
        if (setenv(name, value, 1) != 0)
            perror("export");
        return 1;
    }
    return 0; //flag to continue the procesing of this command (not buiilt-int command)
}

int main(){
    signal(SIGINT, SIG_IGN);  //ignore ctrl+c
    
    int status;
    int pipe_flag;
    while(1)
    {
        type_prompt();
        
        cmdnode* command_list = read_commands(); // reads command(s) and appends each command to a SLL

        cmdnode* p = command_list;
        while(p != NULL){

            pipe_flag = 0;

            //-------------check for variable declaration-------------

            if (strchr(p->argv[0], '=') != NULL){ //if it contains =
                define_variable(p);
                //going to the next command and continuing to not execute the other if-then's
                p=p->next;
                continue;
            }

            //-------------check for builtins-------------

            if (check_builtin(p, command_list) == 1)
            {
                //going to the next command and continuing to not execute the other if-then's
                p=p->next;
                continue;
            }

            //-------------check if-------------

            if(strcmp(p->argv[0], "if") == 0){
                for (int j = 0; p->argv[j] != NULL; j++)
                {
                    if(strstr(p->argv[j], "then") != NULL){
                        fprintf(stderr, "IF: syntax error near unexpected token `%s'\n", p->argv[j]);
                        exit(1);
                    }
                }
                p = handle_if(p);
                //continue to the next command.
                //function set p to first command after if check so no p=p->next is needed
                continue;
            }
            else if (strcmp(p->argv[0], "then") == 0){
                for (int j = 0; p->argv[j] != NULL; j++) {
                    p->argv[j] = p->argv[j+1];
                }
            }
            else if (strcmp(p->argv[0], "fi") == 0){
                p=p->next;
                continue;
            }

            //-------------execute command-------------

            for (int i = 0; p->argv[i] != NULL; i++)
            {
                if(strchr(p->argv[i], '|') != NULL){
                    execute_pipeline(p);
                    pipe_flag = 1;
                    break;
                }
            }
            if(pipe_flag == 0){
                execute_command(p, &status);
            }

            p=p->next; //process the next command
        }
        // parent frees the command list
        free_commands(command_list);
    }
    return 0;
}