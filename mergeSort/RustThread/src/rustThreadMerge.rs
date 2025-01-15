/*
    Neel Bhatia
    CSCI 440
    Fall 2024
*/

use rand::Rng;
use std::thread;
use std::time::Instant;
use num_cpus;

const ARRAY_SIZE: usize = 100000;

fn generate_random_array(size: usize) -> Vec<i32> {
    let mut rng = rand::thread_rng();
    (0..size).map(|_| rng.gen_range(0..1000)).collect()
}

fn compare(a: &i32, b: &i32) -> std::cmp::Ordering {
    a.cmp(b)
}

fn sort_chunk(arr: &mut Vec<i32>, start_idx: usize, end_idx: usize, _thread_id: usize) {
    //println!("Thread {} is now handling indices {}-{}", thread_id, start_idx, end_idx);
    let chunk = &mut arr[start_idx..=end_idx];
    chunk.sort_by(compare);
    //println!("Thread {} has finished sorting indices {}-{}", thread_id, start_idx, end_idx);
}

fn merge(arr: &mut Vec<i32>, start: usize, mid: usize, end: usize) {
    let left = arr[start..=mid].to_vec();
    let right = arr[mid + 1..=end].to_vec();
    let (mut i, mut j, mut k) = (0, 0, start);

    while i < left.len() && j < right.len() {
        if left[i] <= right[j] {
            arr[k] = left[i];
            i += 1;
        } else {
            arr[k] = right[j];
            j += 1;
        }
        k += 1;
    }

    while i < left.len() {
        arr[k] = left[i];
        i += 1;
        k += 1;
    }

    while j < right.len() {
        arr[k] = right[j];
        j += 1;
        k += 1;
    }
}

fn sort_parallel(arr: &mut Vec<i32>) {
    let num_cores = 4;//num_cpus::get(); // Get the number of available CPU cores
    let chunk_size = arr.len() / num_cores; // Divide the work into chunks

    let mut handles = vec![];

    // Spawn threads to sort each chunk
    for thread_index in 0..num_cores {
        let start_idx = thread_index * chunk_size;
        let end_idx = if thread_index == num_cores - 1 {
            arr.len() - 1
        } else {
            (thread_index + 1) * chunk_size - 1
        };

        let mut arr_clone = arr.clone(); // Clone the array to move into the thread
        let handle = thread::spawn(move || {
            sort_chunk(&mut arr_clone, start_idx, end_idx, thread_index);
            arr_clone
        });

        handles.push(handle);
    }

    // Collect results from threads
    let mut sorted_chunks = vec![];
    for handle in handles {
        let partial_result = handle.join().unwrap();
        sorted_chunks.push(partial_result);
    }

    // After sorting, merge all sorted chunks into one sorted array
    for sorted_chunk in sorted_chunks {
        merge(arr, 0, sorted_chunk.len() / 2, sorted_chunk.len() - 1);
    }
}

fn main() {
    let array_size = ARRAY_SIZE; // Example array size
    let mut arr = generate_random_array(array_size); // Generate random array
    
    let num_iterations = 5; // Define how many iterations you want

    let mut total_time = 0.0;

    println!("Number of CPU cores: {}", num_cpus::get());

    // Warm-up phase: run a few initial iterations without timing
    for _ in 0..3 {
        // Perform sorting (we don't need the result here)
        sort_parallel(&mut arr);
    }

    // Measure the time for multiple iterations to average out noise
    for iteration in 1..num_iterations + 1 {
        let start_time = Instant::now(); // Record start time

        // Perform sorting
        sort_parallel(&mut arr);  // We don't need to use the result here

        let duration = start_time.elapsed(); // Get the elapsed time
        let duration_in_seconds = duration.as_secs_f64(); // Convert duration to seconds (as floating point)

        total_time += duration_in_seconds;

        // Print the time for the current iteration
        println!("Time for iteration {}: {:.6} seconds", iteration, duration_in_seconds);
    }

    // Calculate and print the average time per iteration
    println!("Average time per iteration: {:.6} seconds", total_time / num_iterations as f64);
}
