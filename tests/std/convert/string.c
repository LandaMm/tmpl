#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

// ---------- String -------------

void* cstrlen(void**data, unsigned int argc) {
    assert(argc == 1 && "Invalid argument amount.");
    char* str = (char*)data[0];
    int len = strlen(str);
    int* res = (int*)malloc(sizeof(int));
    *res = len;
    return (void*)res;
}