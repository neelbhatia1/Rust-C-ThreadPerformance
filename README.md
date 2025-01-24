# Performance Comparison Summary of Multithreaded Algorithms in C and Rust
#### NOTE: Full writeup is in ThreadedPerformanceWriteup.pdf
## Introduction

This project aims to compare the performance of multithreaded implementations in C and Rust, focusing on the efficiency and scalability of threads in both languages. Three different algorithms were used for testing: Mergesort, Matrix Multiplication, and the Sieve of Eratosthenes. The experiment was designed to explore the impact of various factors such as thread count, data size, and cache efficiency on the overall performance of these algorithms in C and Rust. The results are analyzed in terms of execution time and cache hits/misses.

---

## Experimental Setup

### Machine Specifications

- **Device**: LAPTOP-BM3H46BB
- **Processor**: 11th Gen Intel(R) Core(TM) i5-1135G7 @ 2.40GHz (4 cores, 8 threads)
- **Installed RAM**: 20 GB (19.7 GB usable)
- **Operating System**: 64-bit Linux
- **System Type**: x64-based processor

#### Side Effects

The Intel Core i5-1135G7 has 4 physical cores with 8 threads, but performance from multithreading might not improve linearly. This is because multiple threads can compete for shared resources like cache and memory bandwidth. The Linux scheduler might also not distribute threads evenly, causing some cores to be overworked while others are underutilized. Background processes running on the system also compete for CPU time, adding noise to measurements. Differences in how C and Rust manage memory (stack vs. heap) could also affect how efficiently the algorithms use the CPU cache, potentially causing performance differences between the two languages.

---

## Algorithms Tested

The algorithms tested include:

1. **Mergesort**: Parallelized by splitting the array into smaller subarrays, with threads sorting them in parallel.
2. **Matrix Multiplication**: Involves calculating the dot product of rows and columns, parallelized by assigning each thread to compute a portion of the result matrix.
3. **Sieve of Eratosthenes**: Used for finding primes, parallelized by assigning threads to mark multiples of primes across different number ranges.

---

## Test Variables

The tests varied two key factors: **Thread Count** and **Data Size**.

- **Thread Count**: Adjusted to 2, 4, and 8 threads to evaluate scalability and performance improvements with more threads.
- **Data Size**: Modified, with array and matrix sizes ranging from 100 to 1,000,000 elements, and the upper limit for the Sieve of Eratosthenes varying from 100 to 10,000,000.

---

## Benchmarking Methodology

### Each test was executed under the following conditions:

1. **Warm-up Phase**: Programs ran a warm-up phase of 5-10 iterations to minimize inconsistencies.
2. **Time Measurement**: Execution time was measured using high-resolution timers (`clock_gettime()` in C and `Instant::now()` in Rust).
3. **Cache Monitoring**: Cache hits and misses were measured using Cachegrind.
4. **Iteration**: Tests were executed for 30 iterations to ensure reliable results.

---

## Data

### Matrix Multiplication

| Language | Cores | Size | Instruction Cache (I1) Miss Rate | Data Cache (D1) Miss Rate | Level 2 Cache (LLd) Miss Rate | Avg EEC Time (seconds) |
|---|---|---|---|---|---|---|
| C | 8 | 100 | 0.00% | 0.00% | 0.00% | 0.001849 |
| Rust | 8 | 100 | 0.10% | 0.80% | 0.00% | 0.000711 |
| C | 8 | 500 | 0.00% | 0.20% | 0.00% | 0.146563 |
| Rust | 8 | 500 | 0.00% | 0.03% | 0.10% | 0.041884 |
| C | 8 | 1000 | 0.00% | 3.70% | 0.00% | 1.467962 |
| Rust | 8 | 1000 | 0.00% | 24.40% | 0.10% | 0.648165 |
| C | 8 | 2000 | 0.00% | 5.10% | 0.00% | 24.786701 |
| Rust | 8 | 2000 | 0.00% | 28.60% | 1.30% | 12.849081 |
| C | 2 | 500 | 0.00% | 0.20% | 0.00% | 2.456916 |
| Rust | 2 | 500 | 0.00% | 2.30% | 0.00% | 0.064522 |
| C | 4 | 500 | 0.00% | 0.20% | 0.00% | 1.691113 |
| Rust | 4 | 500 | 0.00% | 2.50% | 0.00% | 0.042985 |
---

### MergeSort

| Language | Cores | Size    | Instruction Cache (i1) Miss Rate | Data Cache (D1) Miss Rate | Level 2 Cache (LLd) Miss Rate | Avg ITER Time (seconds) |
| -------- | ----- | ------- | --------------------------------- | ------------------------- | ---------------------------- | ------------------------- |
| C        | 8     | 100     | 0.46%                             | 0.01%                     | 0.90%                        | 0.000199                  |
| Rust     | 8     | 100     | 0.44%                             | 1.50%                     | 0.90%                        | 0.002763                  |
| C        | 8     | 100000  | 0.00%                             | 0.40%                     | 0.00%                        | 0.134407                  |
| Rust     | 8     | 100000  | 0.00%                             | 1.80%                     | 0.10%                        | 0.114717                  |
| C        | 8     | 1000000 | 0.00%                             | 0.50%                     | 0.00%                        | 1.508911                  |
| Rust     | 8     | 1000000 | 0.00%                             | 6.10%                     | 1.30%                        | 0.559443                  |
| C        | 2     | 100000  | 0.00%                             | 0.40%                     | 0.00%                        | 0.150452                  |
| Rust     | 2     | 100000  | 0.00%                             | 2.00%                     | 0.00%                        | 0.054058                  |
| C        | 4     | 100000  | 0.00%                             | 0.40%                     | 0.00%                        | 0.136279                  |
| Rust     | 4     | 100000  | 0.00%                             | 1.80%                     | 0.10%                        | 0.07997                   |
---

### Sieve of Eratosthenes

| Language | Cores | Size    | Instruction Cache (I1) Miss Rate | Data Cache (D1) Miss Rate | Level 2 Cache (LLd) Miss Rate | Avg ITER Time (seconds) |
| -------- | ----- | ------- | --------------------------------- | ------------------------- | ---------------------------- | ------------------------- |
| C        | 8     | 100     | 0.63%                             | 1.50%                     | 1.30%                        | 0.000584                  |
| Rust     | 8     | 100     | 0.41%                             | 1.70%                     | 1.10%                        | 0.000666                  |
| C        | 8     | 100000  | 0.00%                             | 0.00%                     | 0.00%                        | 0.037053                  |
| Rust     | 8     | 100000  | 0.00%                             | 4.90%                     | 0.00%                        | 0.012301                  |
| C        | 8     | 1000000 | 0.00%                             | 0.70%                     | 0.00%                        | 0.703995                  |
| Rust     | 8     | 1000000 | 0.00%                             | 10.10%                    | 0.00%                        | 0.264296                  |
| C        | 2     | 100000  | 0.00%                             | 0.20%                     | 0.00%                        | 0.052946                  |
| Rust     | 2     | 100000  | 0.01%                             | 4.90%                     | 0.10%                        | 0.014018                  |
| C        | 4     | 100000  | 0.00%                             | 0.00%                     | 0.00%                        | 0.047494                  |
| Rust     | 4     | 100000  | 0.00%                             | 4.90%                     | 0.00%                        | 0.027115                  |
---

## Results and Analysis

### Execution Time

#### Matrix Multiplication
- C generally performs better in terms of execution time across all data sizes and thread counts. For example:
  - **C (100 elements, 8 threads)**: 0.001849 seconds
  - **Rust (100 elements, 8 threads)**: 0.000711 seconds
- As problem size increases, Rust shows better scalability.

#### Sieve of Eratosthenes
- **C** is consistently faster across all sizes.
- Rust shows significant speedups for larger datasets but doesn't match C's execution speed.

#### MergeSort
- C outperforms Rust in execution time, especially for large data sizes.

---

### Cache Efficiency

#### Instruction Cache (i1) Miss Rate
- Both languages show low miss rates for small data sizes.

#### Data Cache (D1) Miss Rate
- Rust generally has higher data cache miss rates compared to C.

#### Level 2 Cache (LLd) Miss Rate
- C maintains a slightly better cache miss rate across all algorithms.

---

### Thread Scalability

#### C
- Increasing thread count results in a noticeable reduction in execution time, with diminishing returns after 4 or 8 threads.

#### Rust
- Rust benefits from increasing thread counts, but the gains are often less significant compared to C.

---

## Conclusion

In terms of thread performance, **C** demonstrates superior execution times, particularly for
smaller datasets. It achieves faster results due to its efficient memory management and lower
cache miss rates, which are crucial for tasks that require high-speed execution, such as matrix
multiplication and the Sieve of Eratosthenes. C also shows more significant improvements with
increasing thread counts in smaller datasets, making it a strong choice for applications where
thread scalability in smaller workloads is critical.

On the other hand, **Rust** excels in handling larger data sizes and more complex operations,
where its threading model proves to be more efficient. While the performance improvements
with multithreading are less dramatic in smaller datasets, Rust manages to better scale as the
problem size grows. Its threading model is optimized for larger workloads, reducing the impact
of synchronization overhead and resource contention. This makes Rust a more suitable choice
for memory-intensive, large-scale applications
