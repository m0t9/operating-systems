#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <stdio.h>

#define max(a, b) ((a) > (b) ? (a) : (b))

typedef unsigned long int size_t;

void* add_int(const void* a, const void* b) {
    int* result = (int*) malloc(sizeof(int));
    *result = *((int*) a) + *((int*) b);
    return result;
}

void* mult_int(const void* a, const void* b) {
    int* result = (int*) malloc(sizeof(int));
    *result = *((int*) a) * *((int*) b);
    return result;
}

void* max_int(const void* a, const void* b) {
    int* result = (int*) malloc(sizeof(int));
    *result = max(*((int*) a), *((int*) b));
    return result;
}

void* add_double(const void* a, const void* b) {
    double* result = (double*) malloc(sizeof(double));
    *result = *((double*) a) + *((double*) b);
    return result;
}

void* mult_double(const void* a, const void* b) {
    double* result = (double*) malloc(sizeof(double));
    *result = *((double*) a) * *((double*) b);
    return result;
}

void* max_double(const void* a, const void* b) {
    double* result = (double*) malloc(sizeof(double));
    *result = max(*((double*) a), *((double*) b));
    return result;
}

void* aggregate(
        void* base, 
        size_t size, 
        int n, 
        void* initial_value,
        void* (*opr)(const void*, const void*)) {
    if (size == sizeof(int)) {
        int* value = (int*)  malloc (sizeof(int));
        *value = *((int*) initial_value);
        for (size_t idx = 0; idx < n; ++idx) {
            int* temp = opr(value, (int*) base + idx);
            *value = *temp;
            free(temp);
        }
        return value;
    } else {
        double* value = (double*) malloc (sizeof(int));
        *value = *((double*) initial_value);
        for (size_t idx = 0; idx < n; ++idx) {
            double* temp = opr(value, (double*) base + idx);
            *value = *temp;
            free(temp);
        }
        return value;
    }
}

#define ARR_SIZE 5

int main() {
    int int_arr[ARR_SIZE] = {2, -1, 4, 3, -7};
    int int_init_sum = 0, int_init_mult = 1, int_init_max = INT_MIN;
    int *int_arr_sum = aggregate(int_arr, sizeof(int), ARR_SIZE, &int_init_sum, add_int);
    int *int_arr_mult = aggregate(int_arr, sizeof(int), ARR_SIZE, &int_init_mult, mult_int);
    int *int_arr_max = aggregate(int_arr, sizeof(int), ARR_SIZE, &int_init_max, max_int);
    
    printf("Content of integer array:\n");
    for (size_t idx = 0; idx < ARR_SIZE; ++idx) {
        printf("%d ", int_arr[idx]);
    }
    printf("\n");
    printf("Answers for integer array: sum = %d, product = %d, max value = %d\n\n", 
            *int_arr_sum, *int_arr_mult, *int_arr_max);
    free(int_arr_sum);
    free(int_arr_mult);
    free(int_arr_max);
    
    double dbl_arr[ARR_SIZE] = {3.0, -1.0, -1.0, 12.4, 7.2};
    double dbl_init_sum = 0., dbl_init_mult = 1., dbl_init_max = DBL_MIN;
    double *dbl_arr_sum = aggregate(dbl_arr, sizeof(double), ARR_SIZE, &dbl_init_sum, add_double);
    double *dbl_arr_mult = aggregate(dbl_arr, sizeof(double), ARR_SIZE, &dbl_init_mult, mult_double);
    double *dbl_arr_max = aggregate(dbl_arr, sizeof(double), ARR_SIZE, &dbl_init_max, max_double);

    printf("Content of doubles array:\n");
    for (size_t idx = 0; idx < ARR_SIZE; ++idx) {
        printf("%lf ", dbl_arr[idx]);
    }
    printf("\n");
    printf("Answers for doubles array: sum = %lf, product = %lf, max value = %lf\n\n", 
            *dbl_arr_sum, *dbl_arr_mult, *dbl_arr_max);
    free(int_arr_sum);
    free(int_arr_mult);
    free(int_arr_max);
    return 0;
}

