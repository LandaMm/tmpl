#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// --------- Integer -------------

// Integer to Double
void* i_tod(void** data, unsigned int argc) {
    assert(argc == 1 && "Invalid argument amount.");
    int* arg = (int*)data[0];
    double* res = (double*)malloc(sizeof(double));
    *res = *arg;
    return (void*)res;
}

// Integer to Float
void* i_tof(void** data, unsigned int argc) {
    assert(argc == 1 && "Invalid argument amount.");
    int* arg = (int*)data[0];
    float* res = (float*)malloc(sizeof(float));
    *res = *arg;
    return (void*)res;
}

// Integer to String
void* i_tos(void** data, unsigned int argc) {
    assert(argc == 1 && "Invalid argument amount.");
    int* arg = (int*)data[0];
    char* res = (char*)malloc(sizeof(char));
    sprintf(res, "%ld", *arg);
    return (void*)res;
}