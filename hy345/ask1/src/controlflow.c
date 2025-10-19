#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <IO.h>
#include <execute.h>

typedef enum {
    EXPR_FALSE = 0,
    EXPR_TRUE  = 1,
    EXPR_ERROR = -1
} expr_result_t;

expr_result_t evaluate_expr(const char *left, const char *op, const char *right) {
    if (!left || !op || !right)
        return EXPR_ERROR;

    int a = atoi(left);
    int b = atoi(right);

    if (strcmp(op, "==") == 0) return a == b ? EXPR_TRUE : EXPR_FALSE;
    if (strcmp(op, "!=") == 0) return a != b ? EXPR_TRUE : EXPR_FALSE;
    if (strcmp(op, "<") == 0)  return a <  b ? EXPR_TRUE : EXPR_FALSE;
    if (strcmp(op, "<=") == 0) return a <= b ? EXPR_TRUE : EXPR_FALSE;
    if (strcmp(op, ">") == 0)  return a >  b ? EXPR_TRUE : EXPR_FALSE;
    if (strcmp(op, ">=") == 0) return a >= b ? EXPR_TRUE : EXPR_FALSE;

    return EXPR_ERROR;
}

cmdnode* handle_if(cmdnode* p)
{
    preprocess_variables(p);

    char *left = NULL, *op = NULL, *right = NULL;

    for (int i = 0; p->argv[i]; i++)
    {
        if (p->argv[i][0] == '['){
            //parse expression
            left = p->argv[i+1];
            op = p->argv[i+2];
            right = p->argv[i+3];
            if (strchr(left, '[')){
                fprintf(stderr, "IF: syntax error near unexpected token `%s'\n", p->argv[i]);
                exit(1);
            }
            if (strchr(right, ']')){
                fprintf(stderr, "IF: syntax error near unexpected token `%s'\n", right);
                exit(1);
            }
            break;
        }
    }
    expr_result_t result = evaluate_expr(left, op, right);
    if (result == EXPR_FALSE){ //false
        cmdnode* prev = p;
        p=p->next;
        while (p!= NULL) //skip commands until fi and return command after if block
        {
            for (int i = 0; p->argv[i]; i++)
            {
                if (strstr(p->argv[i], "fi") != NULL){
                    return p->next;
                }
            }
            prev = p;
            p=p->next;
        }
        fprintf(stderr, "IF: syntax error near unexpected token `%s'\n", prev->argv[0]);
    }
    else if (result == EXPR_ERROR){
        fprintf(stderr, "IF: syntax error near unexpected token `%s'\n", p->argv[2]);
        exit(1);
    }
    //true
    return p->next;

}