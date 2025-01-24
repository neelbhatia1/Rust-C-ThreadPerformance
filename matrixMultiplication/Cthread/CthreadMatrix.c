/*
    Neel Bhatia
    neelb384@gmail.com
    linkedin.com/in/neel-bhatia-5849021a6
    github.com/neelbhatia1
*/

/*
    CthreadMatrix.c
    This code was based on GeeksforGeeks' 
    implementation of Matrix Multiplication, found at
    https://www.geeksforgeeks.org/c-matrix-multiplication/
*/

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h> // For sysconf (Unix/Linux)

#define MATRIX_SIZE 10000       // Size of the square matrix
#define NUM_ITERATIONS 30     // Number of iterations for averaging

// Structure to hold the row range for each thread
typedef struct {
    int start_row;
    int end_row;
    int N;
    int **A;
    int **B;
    int **C;
} ThreadData;

void initialize_matrix(int N, int **matrix) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = rand() % 100; // Random values between 0 and 99
        }
    }
}

void matrix_multiply_part(ThreadData* data) {
    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < data->N; j++) {
            data->C[i][j] = 0;
            for (int k = 0; k < data->N; k++) {
                data->C[i][j] += data->A[i][k] * data->B[k][j];
            }
        }
    }
}

// High-resolution timing function
double get_time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

void* thread_function(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    matrix_multiply_part(data);
    return NULL;
}

int main() {
    pthread_t *threads;
    struct timespec start, end;
    double total_time = 0;

    // Get the number of CPU cores (using sysconf for Linux)
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN); // _SC_NPROCESSORS_ONLN gives the number of processors (cores) available
    if (num_cores < 1) {
        num_cores = 1; // Fallback in case sysconf fails (in case of single-core systems)
    }
    printf("Number of CPU cores: %d\n", num_cores);
    printf("Matrix Size: %d\n",MATRIX_SIZE);

    // Allocate memory for the matrix and threads
    threads = (pthread_t*)malloc(num_cores * sizeof(pthread_t));

    int **A = (int**)malloc(MATRIX_SIZE * sizeof(int*));
    int **B = (int**)malloc(MATRIX_SIZE * sizeof(int*));
    int **C = (int**)malloc(MATRIX_SIZE * sizeof(int*));
    for (int i = 0; i < MATRIX_SIZE; i++) {
        A[i] = (int*)malloc(MATRIX_SIZE * sizeof(int));
        B[i] = (int*)malloc(MATRIX_SIZE * sizeof(int));
        C[i] = (int*)malloc(MATRIX_SIZE * sizeof(int));
    }

    printf("Stabilizing System.\n");

    // Warm-up phase (run some initial iterations to let the system stabilize)
    for (int i = 0; i < 1; i++) {
        initialize_matrix(MATRIX_SIZE, A);
        initialize_matrix(MATRIX_SIZE, B);

        // Create threads to perform matrix multiplication
        ThreadData *thread_data = (ThreadData*)malloc(num_cores * sizeof(ThreadData));

        for (int t = 0; t < num_cores; t++) {
            thread_data[t].start_row = t * (MATRIX_SIZE / num_cores);
            thread_data[t].end_row = (t + 1) * (MATRIX_SIZE / num_cores);
            thread_data[t].N = MATRIX_SIZE;
            thread_data[t].A = A;
            thread_data[t].B = B;
            thread_data[t].C = C;

            pthread_create(&threads[t], NULL, thread_function, (void*)&thread_data[t]);
        }

        for (int t = 0; t < num_cores; t++) {
            pthread_join(threads[t], NULL);
        }

        free(thread_data); // Free the dynamically allocated thread data
    }

    printf("Stabilization Complete.\n");

    // Measure the time for multiple iterations to average out noise
    for (int i = 1; i < NUM_ITERATIONS + 1; i++) {
        initialize_matrix(MATRIX_SIZE, A);
        initialize_matrix(MATRIX_SIZE, B);

        clock_gettime(CLOCK_MONOTONIC, &start);

        // Create threads to perform matrix multiplication
        ThreadData *thread_data = (ThreadData*)malloc(num_cores * sizeof(ThreadData));

        for (int t = 0; t < num_cores; t++) {
            thread_data[t].start_row = t * (MATRIX_SIZE / num_cores);
            thread_data[t].end_row = (t + 1) * (MATRIX_SIZE / num_cores);
            thread_data[t].N = MATRIX_SIZE;
            thread_data[t].A = A;
            thread_data[t].B = B;
            thread_data[t].C = C;

            pthread_create(&threads[t], NULL, thread_function, (void*)&thread_data[t]);
        }

        for (int t = 0; t < num_cores; t++) {
            pthread_join(threads[t], NULL);
        }

        clock_gettime(CLOCK_MONOTONIC, &end);
        double curDuration = get_time_diff(start, end);
        total_time += curDuration;

        printf("Time for iteration %d: %.6f seconds\n", i, curDuration);

        free(thread_data); // Free the dynamically allocated thread data
    }

    // Calculate and print average time taken by each thread
    printf("Average time per thread execution (matrix multiplication): %f seconds\n", total_time / NUM_ITERATIONS);

    // Free the allocated memory
    for (int i = 0; i < MATRIX_SIZE; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);
    free(threads); // Free the dynamically allocated thread array

    return 0;
}