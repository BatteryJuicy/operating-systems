#ifndef _D_PARAMS_
#define _D_PARAMS_

#define __NR_set_proc_info 341
#define __NR_get_proc_info 342

struct d_params{
    int deadline; /* the process's deadline, in seconds from now */
    int est_runtime; /* the expected execution time, in milliseconds */
};
 
#endif