#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <IO.h>
#include <vars.h>
#include <execute.h>

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

void define_variable(cmdnode* p)
{
    //split cmd between =
    const char* name = strtok((p->argv[0]), "=");
    char* value = strtok(NULL, " ");
    
    char* nameptr = (char*)name;
    if (!isalpha(*nameptr)){
        fprintf(stderr, "%s: Invalid variable name", name);
        return;
    }
    while (*(++nameptr) != '\0')
    {
        if (!isalpha(*nameptr) && !isdigit(*nameptr)){
            fprintf(stderr, "%s: Invalid variable name\n", name);
            return;
        }
    }

    if (value != NULL)
    {
        if (value[0] == '\"'){
            value = &value[1];

            int i = 0;
            while (value[i] != '\"') i++;
            if(value[i] != '\"'){
                fprintf(stderr, "Invalid variable delcaration\n");
                return;
            }
            else{
                value[i] = '\0';
            }
            
        }

        //create/overrite variable
        set_var(name, value);
    }
}

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
    int status;
    int pipe_flag;
    while(1)
    {
        pipe_flag = 0;

        type_prompt();
        
        cmdnode* command_list = read_commands(); // reads command(s) and appends each command to a SLL

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

            if (check_builtin(p, command_list) == 1)
            {
                //going to the next command and continuing to not execute the other if-then's
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
            if(pipe_flag == 0)
                execute_command(p, &status);

            p=p->next; //process the next command
        }
        // parent frees the command list
        free_commands(command_list);
    }
    return 0;
}