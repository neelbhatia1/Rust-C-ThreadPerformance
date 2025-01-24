/*
    Neel Bhatia
    neelb384@gmail.com
    linkedin.com/in/neel-bhatia-5849021a6
    github.com/neelbhatia1
*/

use std::sync::{Arc, Mutex};
use std::thread;
use std::time::Instant;
use num_cpus;

const PRIME_CALC: usize = 100000; // Upper Limit for finding primes
const NUM_ITERATIONS: usize = 5;      // Number of iterations for averaging

fn get_time_diff(start: Instant, end: Instant) -> f64 {
    end.duration_since(start).as_secs_f64()
}

fn sieve_part(_start: usize, end: usize, prime: Arc<Mutex<Vec<bool>>>, _thread_id: usize) {
    //println!("Thread ID: {:?} started processing numbers from {} to {}", thread_id, start, end);
    let mut prime = prime.lock().unwrap();  // Lock only when modifying the range
    
    for p in 2..=((end as f64).sqrt() as usize) {
        if prime[p] {  // If prime[p] is true, proceed to mark multiples
            for i in (p * p..=end).step_by(p) {
                prime[i] = false;
            }
        }
    }
    //println!("Thread ID: {:?} finished processing numbers from {} to {}", thread_id, start, end);
}

fn main() {
    let num_cores = 4;//num_cpus::get(); // Get the number of CPU cores
    println!("Number of CPU cores: {}", num_cores);

    let mut total_time = 0.0;

    for _ in 0..2 {
        let prime = Arc::new(Mutex::new(vec![true; PRIME_CALC + 1])); // Initialize the prime vector

        let mut handles = vec![];
        let range_size = PRIME_CALC / num_cores;

        for j in 0..num_cores {
            let start = j * range_size + 2;
            let end = if j == num_cores - 1 {
                PRIME_CALC
            } else {
                (j + 1) * range_size + 1
            };

            let prime_clone = Arc::clone(&prime); // Clone Arc once
            let handle = thread::spawn(move || {
                sieve_part(start, end, prime_clone, j);  // Perform sieve task
            });
            handles.push(handle); // Store thread handle
        }

        // Wait for warm-up threads to finish
        for handle in handles {
            handle.join().unwrap();
        }
    }

    for i in 1..=NUM_ITERATIONS {
        let prime = Arc::new(Mutex::new(vec![true; PRIME_CALC + 1])); // Initialize the prime vector

        let mut handles = vec![];
        let range_size = PRIME_CALC / num_cores;

        for j in 0..num_cores {
            let start = j * range_size + 2;
            let end = if j == num_cores - 1 {
                PRIME_CALC
            } else {
                (j + 1) * range_size + 1
            };

            let prime_clone = Arc::clone(&prime); // Clone Arc once
            let handle = thread::spawn(move || {
                sieve_part(start, end, prime_clone, j);  // Perform sieve task
            });
            handles.push(handle); // Store thread handle
        }

        let start = Instant::now();  // Record start time

        // Wait for threads to finish
        for handle in handles {
            handle.join().unwrap();
        }

        let end = Instant::now();  // Record end time
        let cur_duration = get_time_diff(start, end);
        total_time += cur_duration;

        println!("Time for iteration {}: {:.6} seconds", i, cur_duration);
        // No need to manually drop 'prime', it's dropped when the Arc goes out of scope
    }

    println!("Average time per thread execution (Sieve of Eratosthenes): {:.6} seconds", total_time / NUM_ITERATIONS as f64);
}
