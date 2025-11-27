#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

#define INV_STRUCT_VALUE 123
#define CORRECT_STRUCT_VALUE_1 50
#define CORRECT_STRUCT_VALUE_2 20

#define __NR_set_proc_info 341
#define __NR_get_proc_info 342

struct d_params{
    int deadline;
    int est_runtime;
};

long set_proc_info(int a, int b) {
    return syscall(__NR_set_proc_info, a, b);
}

long get_proc_info(struct d_params *par) {
    return syscall(__NR_get_proc_info, par);
}

int main(void){
    printf("Trap to kernel level\n");
    int return_value;
    struct d_params p;
    p.deadline = INV_STRUCT_VALUE;
    p.est_runtime = INV_STRUCT_VALUE;

    printf("\nTest normal execution...\n\n");

    return_value = set_proc_info(CORRECT_STRUCT_VALUE_1, CORRECT_STRUCT_VALUE_2);
    if (return_value != 0){
        printf("set_proc_info [FAIL]\n");
    }
    else{
        printf("set_proc_info [SUCCES]\n");
    }

    return_value = get_proc_info(&p);
    if (return_value != 0){
        printf("get_proc_info [FAIL]\n");
    }
    else{
        printf("get_proc_info [SUCCES]\n");
    }

    if (p.deadline == INV_STRUCT_VALUE || p.est_runtime == INV_STRUCT_VALUE){
        printf("set_proc_info didn't set at least one of the values of d_params \
        or assigned special value denoted by %d. [FAIL]", INV_STRUCT_VALUE \
        );
    }

    printf("return value from get_proc_info:\n");
    printf("Deadline %d seconds\n", p.deadline);
    printf("est_runtime %d ms\n", p.est_runtime);

    if (p.deadline == CORRECT_STRUCT_VALUE_1 && p.est_runtime == CORRECT_STRUCT_VALUE_2){
        printf("set_proc_info correctly assigned values! [SUCCESS]\n");
    }
    else{
        printf("set_proc_info incorrectly assigned values! [FAIL]\n");
    }
    
    printf("\nTest NULL pass in get_proc_info...\n\n");
    return_value = get_proc_info(NULL);
    if (return_value != 0){
        printf("get_proc_info correctly returned error! [SUCCESS]\n");
    }
    else{
        printf("get_proc_info incorrectly returned error! [FAIL]\n");
    }
    
    printf("\nTest other invalid memory address pass in get_proc_info...\n\n");
    return_value = get_proc_info((struct d_params *)0xF0000000);
    if (return_value != 0){
        printf("get_proc_info correctly returned error! [SUCCESS]\n");
    }
    else{
        printf("get_proc_info incorrectly returned error! [FAIL]\n");
    }

    printf("\nTest non-positive args in set_proc_info...\n\n");
    return_value = set_proc_info(0, -1);
    if (return_value != 0){
        printf("set_proc_info correctly returned error! [SUCCESS]\n");
    }
    else{
        printf("set_proc_info incorrectly returned error! [FAIL\n");
    }

    printf("Back to user level\n");
    return 0;
}