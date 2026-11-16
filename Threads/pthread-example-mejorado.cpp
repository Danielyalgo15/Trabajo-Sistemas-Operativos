
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define NUM_THREADS 5

struct ThreadData {
    int index;
    int sleep_time;
};

void *perform_work(void *arguments) {
    ThreadData *data = (ThreadData *)arguments;

    printf("THREAD %d: Started.\n", data->index);
    printf("THREAD %d: Sleeping for %d seconds.\n",
           data->index, data->sleep_time);

    sleep(data->sleep_time);

    printf("THREAD %d: Ended.\n", data->index);
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_args[NUM_THREADS];
    int result_code;
    int created_threads = 0;

    srand((unsigned int)time(NULL));

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i].index = i;
        thread_args[i].sleep_time = 1 + rand() % NUM_THREADS;

        printf("IN MAIN: Creating thread %d.\n", i);

        result_code = pthread_create(
            &threads[i], NULL, perform_work, &thread_args[i]
        );

        if (result_code != 0) {
            printf("ERROR: Could not create thread %d: %s\n",
                   i, strerror(result_code));
            break;
        }

        created_threads++;
    }

    printf("IN MAIN: Waiting for threads to finish.\n");

    for (int i = 0; i < created_threads; i++) {
        result_code = pthread_join(threads[i], NULL);

        if (result_code != 0) {
            printf("ERROR: Could not join thread %d: %s\n",
                   i, strerror(result_code));
        } else {
            printf("IN MAIN: Thread %d has ended.\n", i);
        }
    }

    printf("MAIN: Program has ended.\n");

    return 0;
}