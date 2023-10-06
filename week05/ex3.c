#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

bool is_prime(int n) {
    if (n <= 1) {
        return false;
    }
    for (int i = 2; i * i <= n; ++i) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

int primes_count(int a, int b) {
    int count = 0;
    for (int i = a; i < b; ++i) {
        count += (is_prime(i) ? 1 : 0);
    }
    return count;
}

typedef struct {
    int a, b;
} prime_request;

void* prime_counter(void* arg) {
    prime_request* request = (prime_request*) arg;
    int* result = malloc(sizeof(int));
    *result = primes_count(request->a, request->b);
    return result;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        return EXIT_FAILURE;
    }
    int n = atoi(argv[1]), m = atoi(argv[2]);
    pthread_t threads[m];
    prime_request requests[m];
    int len = n / m;
    for (int idx = 0; idx < m - 1; ++idx) {
        requests[idx].a = idx * len;
        requests[idx].b = (idx + 1) * len;
        pthread_create(&threads[idx], NULL, &prime_counter, &requests[idx]);
    }
    requests[m - 1].a = (m - 1) * len;
    requests[m - 1].b = n;
    pthread_create(&threads[m - 1], NULL, &prime_counter, &requests[m - 1]);
    int ans = 0;
    for (int idx = 0; idx < m; ++idx) {
        void* ans_idx;
        pthread_join(threads[idx], &ans_idx);
        ans += *((int*) ans_idx);
        free(ans_idx);
    }
    printf("Answer is %d\n", ans);
}  
