#include <stdio.h>
#include <assert.h>

void *cprintln(void**data, unsigned int argc)
{
    assert(argc == 1 && "Invalid argument count");
    char* msg = (char*)data[0];
    printf("%s\n", msg);
    return NULL;
}

void *cprint(void**data, unsigned int argc)
{
    assert(argc == 1 && "Invalid argument count");
    char* msg = (char*)data[0];
    printf("%s", msg);
    return NULL;
}