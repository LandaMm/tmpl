#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// --------- Double -------------

// Double to Float
void* d_tof(void** data, unsigned int argc) {
    assert(argc == 1 && "Invalid argument amount.");
    double* arg = (double*)data[0];
    float* res = (float*)malloc(sizeof(float));
    *res = *arg;
    return (void*)res;
}

// Double to Integer
void* d_toi(void** data, unsigned int argc) {
    assert(argc == 1 && "Invalid argument amount.");
    double* arg = (double*)data[0];
    int* res = (int*)malloc(sizeof(int));
    *res = *arg;
    return (void*)res;
}

// Double to String
void* d_tos(void** data, unsigned int argc) {
    assert(argc == 1 && "Invalid argument amount.");
    double* arg = (double*)data[0];
    char* res = (char*)malloc(sizeof(char));
    sprintf(res, "%f", *arg);
    return (void*)res;
}
