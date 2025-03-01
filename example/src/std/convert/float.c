#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// --------- Float -------------

// Float to Double
void* f_tod(void** data, unsigned int argc) {
    assert(argc == 1 && "Invalid argument amount.");
    float* arg = (float*)data[0];
    double* res = (double*)malloc(sizeof(double));
    *res = *arg;
    return (void*)res;
}

// Float to Integer
void* f_toi(void** data, unsigned int argc) {
    assert(argc == 1 && "Invalid argument amount.");
    float* arg = (float*)data[0];
    int* res = (int*)malloc(sizeof(int));
    *res = *arg;
    return (void*)res;
}

// Float to String
void* f_tos(void** data, unsigned int argc) {
    assert(argc == 1 && "Invalid argument amount.");
    float* arg = (float*)data[0];
    char* res = (char*)malloc(sizeof(char));
    sprintf(res, "%f", *arg);
    return (void*)res;
}