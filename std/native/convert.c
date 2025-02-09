#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

void* i_tof(void** data, unsigned int argc) {
    assert(argc == 1 && "Invalid argument amount.");
    int* arg = (int*)data[0];
    float* res = (float*)malloc(sizeof(float));
    *res = *arg;
    return (void*)res;
}

void* f_toi(void** data, unsigned int argc) {
    assert(argc == 1 && "Invalid argument amount.");
    float* arg = (float*)data[0];
    int* res = (int*)malloc(sizeof(int));
    *res = *arg;
    return (void*)res;
}

void* i_tos(void** data, unsigned int argc) {
    assert(argc == 1 && "Invalid argument amount.");
    int* arg = (int*)data[0];
    char* res = (char*)malloc(sizeof(char));
    sprintf(res, "%ld", *arg);
    return (void*)res;
}

void* cstrlen(void**data, unsigned int argc) {
    assert(argc == 1 && "Invalid argument amount.");
    char* str = (char*)data[0];
    int len = strlen(str);
    int* res = (int*)malloc(sizeof(int));
    *res = len;
    return (void*)res;
}
