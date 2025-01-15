/*
    Neel Bhatia
    CSCI 440
    Fall 2024
*/

/*
    CthreadSieve.c
    This code was based on Aditya Kumar's (adityakumar129) 
    Sieve of Eratosthenes implementation, found on
    https://www.geeksforgeeks.org/sieve-of-eratosthenes/
*/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>  // For sysconf (Linux)

#define PRIME_CALC 1000000       // Upper Limit for finding primes
#define NUM_ITERATIONS 30        // Number of iterations for averaging

double get_time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

typedef struct {
    int start;
    int end;
    bool *prime;
    int thread_id;  // Add thread_id to identify each thread
} ThreadData;

pthread_mutex_t lock;

void* sieve_part(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    // Uncomment the next line if you want to see thread processing
    // printf("Thread %d is processing numbers from %d to %d\n", data->thread_id, data->start, data->end);

    for (int p = 2; p * p <= data->end; p++) {
        if (data->prime[p]) {
            for (int i = p * p; i <= data->end; i += p) {
                if (i >= data->start) {
                    pthread_mutex_lock(&lock);
                    data->prime[i] = false;
                    pthread_mutex_unlock(&lock);
                }
            }
        }
    }

    // Uncomment the next line if you want to see when the thread finishes processing
    // printf("Thread %d finished processing numbers from %d to %d\n", data->thread_id, data->start, data->end);

    return NULL;
}

int main() {
    struct timespec start, end;
    double total_time = 0;
    double curDuration;

    // Get the number of CPU cores at runtime
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);  // Get the number of processors
    if (num_cores < 1) {
        num_cores = 1;  // Fallback in case sysconf fails
    }

    // Allocate memory for threads once outside the loop
    pthread_t *threads = (pthread_t*)malloc(num_cores * sizeof(pthread_t));
    if (threads == NULL) {
        printf("Memory allocation for threads failed!\n");
        return 1;
    }

    printf("Number of CPU cores: %d\n", num_cores);

    pthread_mutex_init(&lock,NULL);

    for (int i = 1; i <= 3; i++) {
        bool *prime = (bool *)malloc((PRIME_CALC + 1) * sizeof(bool));
        if (prime == NULL) {
            printf("Memory allocation for primes failed!\n");
            free(threads);
            return 1;
        }

        memset(prime, true, (PRIME_CALC + 1) * sizeof(bool));

        ThreadData *thread_data = (ThreadData*)malloc(num_cores * sizeof(ThreadData));
        if (thread_data == NULL) {
            printf("Memory allocation for thread_data failed!\n");
            free(prime);
            free(threads);
            return 1;
        }

        int range_size = PRIME_CALC / num_cores;
        
        // Set the range for each thread and create threads
        for (int j = 0; j < num_cores; j++) {
            thread_data[j].start = j * range_size + 2;
            thread_data[j].end = (j == num_cores - 1) ? PRIME_CALC : (j + 1) * range_size + 1;
            thread_data[j].prime = prime;
            thread_data[j].thread_id = j;  // Assign a unique thread ID

            pthread_create(&threads[j], NULL, sieve_part, (void*)&thread_data[j]);
        };

        // Wait for all threads to finish
        for (int j = 0; j < num_cores; j++) {
            pthread_join(threads[j], NULL);
        }

        // Free memory allocated in this iteration
        free(thread_data);
        free(prime);
    }

    // Run multiple iterations
    for (int i = 1; i <= NUM_ITERATIONS; i++) {
        bool *prime = (bool *)malloc((PRIME_CALC + 1) * sizeof(bool));
        if (prime == NULL) {
            printf("Memory allocation for primes failed!\n");
            free(threads);
            return 1;
        }

        memset(prime, true, (PRIME_CALC + 1) * sizeof(bool));

        ThreadData *thread_data = (ThreadData*)malloc(num_cores * sizeof(ThreadData));
        if (thread_data == NULL) {
            printf("Memory allocation for thread_data failed!\n");
            free(prime);
            free(threads);
            return 1;
        }

        int range_size = PRIME_CALC / num_cores;
        
        // Set the range for each thread and create threads
        for (int j = 0; j < num_cores; j++) {
            thread_data[j].start = j * range_size + 2;
            thread_data[j].end = (j == num_cores - 1) ? PRIME_CALC : (j + 1) * range_size + 1;
            thread_data[j].prime = prime;
            thread_data[j].thread_id = j;  // Assign a unique thread ID

            pthread_create(&threads[j], NULL, sieve_part, (void*)&thread_data[j]);
        }

        // Measure time for each iteration
        clock_gettime(CLOCK_MONOTONIC, &start);

        // Wait for all threads to finish
        for (int j = 0; j < num_cores; j++) {
            pthread_join(threads[j], NULL);
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        curDuration = get_time_diff(start, end);
        total_time += curDuration;

        printf("Time for iteration %d: %.6f seconds\n", i, curDuration);

        // Free memory allocated in this iteration
        free(thread_data);
        free(prime);
    }

    // Free memory for threads after all iterations
    free(threads);

    printf("Average time per thread execution (Sieve of Eratosthenes): %.6f seconds\n", total_time / NUM_ITERATIONS);

    return 0;
}
