#include <pthread.h>
#include <stdio.h>

#define STR_SIZE 256
#define MSG "Hello from thread %d\n"
#define THREAD_CREATED "Thread %d is created\n"

typedef struct {
    pthread_t id;
    int i;
    char message[STR_SIZE];
} Thread;

void* thread_function(void* arg) {
    Thread* thread_info = (Thread*) arg;
    printf(thread_info->message, thread_info->i);
}

int main() {
    int n;
    scanf("%d", &n);

    Thread threads[n];
    
    for (int i = 0; i < n; ++i) {
        threads[i].i = i;
        sprintf(threads[i].message, MSG, i);
  
        pthread_create(&threads[i].id, NULL, &thread_function, &threads[i]);
        printf(THREAD_CREATED, i);
        pthread_join(threads[i].id, NULL);
    }
}
