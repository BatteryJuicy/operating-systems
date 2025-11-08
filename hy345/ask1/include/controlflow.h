#include <IO.h>

#define MAX_FOR_ARGS 20
#if MAX_FOR_ARGS > (MAX_ARG_SIZE - 3)
#undef MAX_FOR_ARGS
#define MAX_FOR_ARGS (MAX_ARG_SIZE - 3)
#endif

//handles if and return pointer to commmand after fi if false and to the command after then
cmdnode* handle_if(cmdnode* p);

void handle_for(cmdnode* p, char*** arrayptr, const char** iterator_name);