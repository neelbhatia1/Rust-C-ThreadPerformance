/*
    Neel Bhatia
    neelb384@gmail.com
    linkedin.com/in/neel-bhatia-5849021a6
    github.com/neelbhatia1
*/

/*
    CthreadMerge.c
    This code was based on GeeksforGeeks' 
    implementation of mergeSort, found at
    https://www.geeksforgeeks.org/c-program-for-merge-sort/
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h> // For sysconf (Unix/Linux)

#define ARRAY_SIZE 100000  // Size of the array
#define NUM_ITERATIONS 30     // Number of iterations for averaging

int *arr;  // Dynamically allocated array to sort
int chunk_size;  // Size of each chunk processed by each thread

// Structure to hold data for each thread
typedef struct {
    int thread_id;
    int start_idx;
    int end_idx;
} ThreadData;

// Function to generate random values in the array
void generate_random_array(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % 1000;  // Random number between 0 and 99
    }
}

// Sorting function for each chunk (using qsort)
int compare(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

// Function to process and sort each chunk by a thread
void *sort_chunk(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int start_idx = data->start_idx;
    int end_idx = data->end_idx;
    int thread_id = data->thread_id;

    //printf("Thread %d is now handling indices %d-%d\n", thread_id, start_idx, end_idx);

    // Sort the chunk using qsort
    qsort(&arr[start_idx], end_idx - start_idx + 1, sizeof(int), compare);

    //printf("Thread %d has finished sorting indices %d-%d\n", thread_id, start_idx, end_idx);

    return NULL;
}

// Function to merge two sorted subarrays
void merge(int *arr, int start, int mid, int end) {
    int n1 = mid - start + 1;
    int n2 = end - mid;

    int *left = malloc(n1 * sizeof(int));
    int *right = malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++) {
        left[i] = arr[start + i];
    }
    for (int i = 0; i < n2; i++) {
        right[i] = arr[mid + 1 + i];
    }

    int i = 0, j = 0, k = start;
    while (i < n1 && j < n2) {
        if (left[i] <= right[j]) {
            arr[k++] = left[i++];
        } else {
            arr[k++] = right[j++];
        }
    }

    while (i < n1) {
        arr[k++] = left[i++];
    }

    while (j < n2) {
        arr[k++] = right[j++];
    }

    free(left);
    free(right);
}

// Merge all the sorted chunks into a single sorted array
void merge_chunks(int *arr, int num_chunks) {
    int size = ARRAY_SIZE;

    // Iterate through chunk sizes, merging adjacent chunks at each step
    for (int current_chunk_size = chunk_size; current_chunk_size < size; current_chunk_size *= 2) {
        for (int start = 0; start < size; start += 2 * current_chunk_size) {
            int mid = start + current_chunk_size - 1;
            int end = (start + 2 * current_chunk_size - 1 < size) ? (start + 2 * current_chunk_size - 1) : (size - 1);

            // Merge adjacent chunks if the mid index is less than end
            if (mid < end) {
                merge(arr, start, mid, end);
            }
        }
    }
}

double get_time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main() {
    srand(time(0));  // Seed the random number generator
    double total_time = 0;

    int num_cores = 4;//sysconf(_SC_NPROCESSORS_ONLN); // _SC_NPROCESSORS_ONLN gives the number of processors (cores) available
    chunk_size = ARRAY_SIZE / num_cores;  // Size of each chunk processed by each thread

    // Dynamically allocate memory for the array
    arr = malloc(ARRAY_SIZE * sizeof(int));
    if (arr == NULL) {
        perror("Failed to allocate memory for the array");
        return 1;
    }

    pthread_t *threads = malloc(num_cores * sizeof(pthread_t));
    if (threads == NULL) {
        perror("Failed to allocate memory for threads");
        free(arr);
        return 1;
    }

    // Stabilize System
    for (int i = 1; i < 3; i++) {
        // Generate the random array
        generate_random_array(arr, ARRAY_SIZE);

        // Dynamically allocate memory for thread data
        ThreadData *thread_data = malloc(num_cores * sizeof(ThreadData));

        // Create threads to sort chunks
        for (int i = 0; i < num_cores; i++) {
            thread_data[i].thread_id = i;
            thread_data[i].start_idx = i * chunk_size;
            thread_data[i].end_idx = (i == num_cores - 1) ? (ARRAY_SIZE - 1) : ((i + 1) * chunk_size - 1);

            pthread_create(&threads[i], NULL, sort_chunk, (void *)&thread_data[i]);
        }

        // Wait for all threads to finish sorting
        for (int i = 0; i < num_cores; i++) {
            pthread_join(threads[i], NULL);
        }
        
        // Merge all sorted chunks into one sorted array
        merge_chunks(arr, num_cores);
        free(thread_data);
    }

    for (int i = 1; i < NUM_ITERATIONS + 1; i++) {
        // Generate the random array
        generate_random_array(arr, ARRAY_SIZE);

        // Dynamically allocate memory for thread data
        ThreadData *thread_data = malloc(num_cores * sizeof(ThreadData));
        if (thread_data == NULL) {
            perror("Failed to allocate memory for thread data");
            free(arr);
            return 1;
        } 

        struct timespec start, end;

        // Start the timer
        clock_gettime(CLOCK_MONOTONIC, &start);

        // Create threads to sort chunks
        for (int i = 0; i < num_cores; i++) {
            thread_data[i].thread_id = i;
            thread_data[i].start_idx = i * chunk_size;
            thread_data[i].end_idx = (i == num_cores - 1) ? (ARRAY_SIZE - 1) : ((i + 1) * chunk_size - 1);

            pthread_create(&threads[i], NULL, sort_chunk, (void *)&thread_data[i]);
        }

        // Wait for all threads to finish sorting
        for (int i = 0; i < num_cores; i++) {
            pthread_join(threads[i], NULL);
        }
        
        clock_gettime(CLOCK_MONOTONIC, &end);

        // Merge all sorted chunks into one sorted array
        merge_chunks(arr, num_cores);

        // End the timer
        double curDuration = get_time_diff(start, end);
        total_time += curDuration;

        // Print performance timing
        printf("Iteration %d: Time taken for sorting: %.6f seconds\n", i, curDuration);

        // Free dynamically allocated memory

        free(thread_data);
    }

    printf("Average time for all iterations: %.6f seconds\n", total_time / NUM_ITERATIONS);

    free(arr);
    free(threads);

    return 0;
}
