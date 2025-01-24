/*
    Neel Bhatia
    neelb384@gmail.com
    linkedin.com/in/neel-bhatia-5849021a6
    github.com/neelbhatia1
*/

use std::thread;
use std::time::Instant;
use num_cpus;

fn multiply_matrices(a: &Vec<Vec<i32>>, b: &Vec<Vec<i32>>) -> Vec<Vec<i32>> {
    let n = a.len();
    let mut result = vec![vec![0; n]; n];

    // Number of threads should be based on the number of CPU cores
    let num_threads = num_cpus::get(); // Automatically get the number of CPU cores
    let chunk_size = n / num_threads; // Divide the work into chunks

    // Create a vector to hold the thread handles
    let mut handles = vec![];

    // Spawn threads to compute parts of the matrix multiplication
    for thread_index in 0..num_threads {
        let a = a.clone(); // Clone a matrix to move into the thread
        let b = b.clone(); // Clone b matrix to move into the thread
        let start_row = thread_index * chunk_size;
        let end_row = if thread_index == num_threads - 1 { n } else { (thread_index + 1) * chunk_size };

        let handle = thread::spawn(move || {
            let mut partial_result = vec![vec![0; n]; n];

            for i in start_row..end_row {
                for j in 0..n {
                    partial_result[i][j] = (0..n).map(|k| a[i][k] * b[k][j]).sum();
                }
            }

            partial_result
        });

        handles.push(handle);
    }

    // Collect results from threads
    for (i, handle) in handles.into_iter().enumerate() {
        let partial_result = handle.join().unwrap();

        // Insert the partial result into the correct part of the result matrix
        let start_row = i * chunk_size;
        let end_row = if i == num_threads - 1 { n } else { (i + 1) * chunk_size };

        for i in start_row..end_row {
            result[i] = partial_result[i].clone();
        }
    }

    result
}

fn main() {
    let n = 2000; // Example matrix size
    let mut a = vec![vec![0; n]; n];
    let mut b = vec![vec![0; n]; n];
    
    let num_iterations = 2; // Define how many iterations you want

    let mut total_time = 0.0;

    // Initialize matrices with random values
    for i in 0..n {
        for j in 0..n {
            a[i][j] = rand::random::<i32>() % 100;
            b[i][j] = rand::random::<i32>() % 100;
        }
    }

    println!("Number of CPU cores: {}", num_cpus::get());

    // Warm-up phase: run a few initial iterations without timing
    for _ in 0..0 {
        // Perform matrix multiplication (we don't need the result here)
        let _ = multiply_matrices(&a, &b);
    }

    // Measure the time for multiple iterations to average out noise
    for iteration in 1..num_iterations + 1 {
        // Measure the time taken for the matrix multiplication
        let start_time = Instant::now(); // Record start time

        // Perform matrix multiplication
        let _result = multiply_matrices(&a, &b);  // We don't need to use the result here

        let duration = start_time.elapsed(); // Get the elapsed time
        let duration_in_seconds = duration.as_secs_f64(); // Convert duration to seconds (as floating point)

        total_time += duration_in_seconds;

        // Print the time for the current iteration
        println!("Time for iteration {}: {:.6} seconds", iteration, duration_in_seconds);
    }

    // Calculate and print the average time per iteration
    println!("Average time per iteration: {:.6} seconds", total_time / num_iterations as f64);
}