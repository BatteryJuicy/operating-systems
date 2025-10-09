#include <stdio.h>
#include <string.h>
#include <IO.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(){
    int status;
    while(1)
    {
        type_prompt();
        
        struct cmdnode* command_list = read_command();

        struct cmdnode* p = command_list;
        while(p != NULL){
            //check for builtins
            if(strcmp(p->argv[0], "cd") == 0){
                if (p->argv[1] == NULL) {
                    fprintf(stderr, "cd: no directory provided\n");
                } 
                else if (chdir(p->argv[1]) == -1){
                    perror("cd");
                }
            }
            else if (strcmp(p->argv[0], "exit") == 0){
                free_commands(command_list);
                exit(0);   
            }

            pid_t pid = fork();
            if (pid != 0){ //parent wait
                waitpid(pid, &status, 0);
            }
            else{ //child proccess
                execvp((p->argv)[0], p->argv);
            }
            p=p->next; //process the next command
        }
        // parent frees the command list
        free_commands(command_list);
    }
    return 0;
}