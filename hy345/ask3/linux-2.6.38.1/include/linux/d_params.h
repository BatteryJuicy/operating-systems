#ifndef _D_PARAMS_
#define _D_PARAMS_

struct d_params{
    int deadline; /* the process's deadline, in seconds from now */
    int est_runtime; /* the expected execution time, in milliseconds */
};

#endif