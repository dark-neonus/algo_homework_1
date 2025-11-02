# Student Database - Homework 1

Implementation of student database with three different data structure variants. Supports operations with configurable ratios (V1: 5:5:50) and two sorting approaches.

## Task Parameters

- **Variant**: V1
- **Operation Ratio**: A=5, B=5, C=50
- **Sort Task**: S4 (sort by surname, name)

## Build

```bash
mkdir build && cd build
cmake ..
make
```

## Run

```bash
./algo_homework_1
```

Program generates test datasets (100, 1K, 10K, 100K records) and benchmarks:

- Load time and memory usage for each variant
- Operations/10s for ratio 5:5:50
- Sort performance (standard vs radix)

## Operations

1. Find students by (name, surname)
2. Find groups with duplicate (name, surname)
3. Update group by email

## Variants

- **Variant 1**: HashMap - O(1) lookup for operations 1,3
- **Variant 2**: Mixed - minimal memory, linear search
- **Variant 3**: Map/BST - O(log n) sorted access

## Sorting

- **Standard**: std::sort with custom comparator
- **Radix**: MSD radix sort for string keys

## Exampel output

```bash
=== Student Database Benchmark ===

Loading full dataset from students.csv...
Dataset loaded successfully


--- Dataset size: 100 ---
  Testing Variant 1 (HashMap) with 100 records:
      Load time: 0.000s
      Memory usage: 29 KB
      Operations in 10s: 52337689
  Testing Variant 2 (Mixed) with 100 records:
      Load time: 0.000s
      Memory usage: 26 KB
      Operations in 10s: 4988821
  Testing Variant 3 (Map/BST) with 100 records:
      Load time: 0.000s
      Memory usage: 32 KB
      Operations in 10s: 51530132

--- Dataset size: 1000 ---
  Testing Variant 1 (HashMap) with 1000 records:
      Load time: 0.002s
      Memory usage: 248 KB
      Operations in 10s: 1488477
  Testing Variant 2 (Mixed) with 1000 records:
      Load time: 0.001s
      Memory usage: 218 KB
      Operations in 10s: 312907
  Testing Variant 3 (Map/BST) with 1000 records:
      Load time: 0.001s
      Memory usage: 274 KB
      Operations in 10s: 1310757

--- Dataset size: 10000 ---
  Testing Variant 1 (HashMap) with 10000 records:
      Load time: 0.013s
      Memory usage: 3460 KB
      Operations in 10s: 45033
  Testing Variant 2 (Mixed) with 10000 records:
      Load time: 0.009s
      Memory usage: 3295 KB
      Operations in 10s: 21185
  Testing Variant 3 (Map/BST) with 10000 records:
      Load time: 0.012s
      Memory usage: 3535 KB
      Operations in 10s: 42561

--- Dataset size: 100000 ---
  Testing Variant 1 (HashMap) with 100000 records:
      Load time: 0.157s
      Memory usage: 28413 KB
      Operations in 10s: 3345
  Testing Variant 2 (Mixed) with 100000 records:
      Load time: 0.142s
      Memory usage: 27164 KB
      Operations in 10s: 1289
  Testing Variant 3 (Map/BST) with 100000 records:
      Load time: 0.211s
      Memory usage: 28490 KB
      Operations in 10s: 3289


=== Sort Benchmarks ===

Sort benchmark with 100 records:
  Standard sort: 0.0000s
  Radix sort: 0.0002s

Sort benchmark with 1000 records:
  Standard sort: 0.0003s
  Radix sort: 0.0061s

Sort benchmark with 10000 records:
  Standard sort: 0.0035s
  Radix sort: 0.0707s

Sort benchmark with 100000 records:
  Standard sort: 0.0529s
  Radix sort: 0.9619s
```
