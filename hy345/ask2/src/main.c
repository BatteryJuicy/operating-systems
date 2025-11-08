/*============================================================================
    Author:      Andreas Ektoras Thanopoulos
    AM:          5401
============================================================================*/

#include <stdio.h>
#include <stdlib.h>

static int _N, _G, _X;
const int *const N = &_N; //Nubmer of Tables
const int *const G = &_G; //Number of Groups
const int *const X = &_X; //Capacity of each Table and cap of Group Size

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Invalid number of parameters.\n");
        return 1;
    }

    _N = atoi(argv[1]);
    _G = atoi(argv[2]);
    _X = atoi(argv[3]);

    if (*N < 1 || *G < 1 || *X < 1)
    {
        fprintf(stderr, "Invalid Parameter types or values.\n");
        return 1;
    }
    printf(":%d, %d, %d:\n", *N, *G, *X);

    return 0;
}