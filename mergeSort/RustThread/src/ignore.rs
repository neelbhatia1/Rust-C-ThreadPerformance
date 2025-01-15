
/*use std::thread;
use std::sync::{Arc, Mutex};
use std::time::Instant;
use rand::Rng;

const ARRAY_SIZE: usize = 10000;

fn generate_random_array(size: usize) -> Vec<i32> {
    let mut rng = rand::thread_rng();
    (0..size).map(|_| rng.gen_range(0..1000)).collect()
}

fn compare(a: &i32, b: &i32) -> std::cmp::Ordering {
    a.cmp(b)
}

fn sort_chunk(arr: Arc<Mutex<Vec<i32>>>, start_idx: usize, end_idx: usize, thread_id: usize) {
    let mut arr = arr.lock().unwrap();
    println!("Thread {} is now handling indices {}-{}", thread_id, start_idx, end_idx);
    let chunk = &mut arr[start_idx..=end_idx];
    chunk.sort_by(compare);
    println!("Thread {} has finished sorting indices {}-{}", thread_id, start_idx, end_idx);
}

fn merge(arr: &mut Vec<i32>, start: usize, mid: usize, end: usize) {
    let  left = arr[start..=mid].to_vec();
    let  right = arr[mid + 1..=end].to_vec();
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

fn merge_chunks(arr: &mut Vec<i32>, chunk_size: usize) {
    let size = arr.len();
    let mut current_chunk_size = chunk_size;

    while current_chunk_size < size {
        for start in (0..size).step_by(2 * current_chunk_size) {
            let mid = start + current_chunk_size - 1;
            let end = std::cmp::min(start + 2 * current_chunk_size - 1, size - 1);
            if mid < end {
                merge(arr, start, mid, end);
            }
        }
        current_chunk_size *= 2;
    }
}

fn main() {
    let num_cores = num_cpus::get(); // Get the number of available CPU cores
    let chunk_size = ARRAY_SIZE / num_cores;

    let arr = generate_random_array(ARRAY_SIZE);
    let arr = Arc::new(Mutex::new(arr));

    let mut threads = vec![];

    let start_time = Instant::now();

    for i in 0..num_cores {
        let arr_clone = Arc::clone(&arr);
        let start_idx = i * chunk_size;
        let end_idx = if i == num_cores - 1 {
            ARRAY_SIZE - 1
        } else {
            (i + 1) * chunk_size - 1
        };

        let thread = thread::spawn(move || {
            sort_chunk(arr_clone, start_idx, end_idx, i);
        });

        threads.push(thread);
    }

    // Wait for all threads to finish sorting
    for t in threads {
        t.join().unwrap();
    }

    // Merge all sorted chunks into one sorted array
    let mut arr = arr.lock().unwrap();
    merge_chunks(&mut arr, chunk_size);

    let duration = start_time.elapsed();
    println!("\nArray after sorting:");
    for num in arr.iter() {
        print!("{} ", num);
    }
    println!();

    println!("\nTime taken for sorting: {:?}", duration.as_millis());
}
*/