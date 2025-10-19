#include <IO.h>

#ifndef EXECUTE_H
#define EXECUTE_H

void preprocess_variables(cmdnode* p);
void execute_command(cmdnode*, int*);
void execute_pipeline();
int redirect_io(cmdnode* p);
#endif