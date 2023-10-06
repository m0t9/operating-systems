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

int n;
pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;
int k = 0, c = 0;

int get_number_to_check() {
    int ret = k;
    if (k != n) {
        ++k;
    }
    return ret;
}

void increment_primes() {
    ++c;
}

void* check_primes(void* arg) {
    while (1) {
        pthread_mutex_lock(&global_lock);
        int next_to_check = get_number_to_check();
        pthread_mutex_unlock(&global_lock);
        if (next_to_check == n) {
            break;
        }
        if (is_prime(next_to_check)) {
            pthread_mutex_lock(&global_lock);
            increment_primes();
            pthread_mutex_unlock(&global_lock);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        return EXIT_FAILURE;
    }

    n = atoi(argv[1]);
    int m = atoi(argv[2]);

    pthread_t threads[m];
    for (int idx = 0; idx < m; ++idx) {
        pthread_create(&threads[idx], NULL, &check_primes, NULL);
    }
    for (int idx = 0; idx < m; ++idx) {
        pthread_join(threads[idx], NULL);
    }
    printf("Primes count on interval [0, %d) is %d\n", n, c);
}  
