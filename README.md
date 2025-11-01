# Student Database - Homework 1

Implementation of student database with three different data structure variants. Supports operations with configurable ratios (V1: 5:5:50) and two sorting approaches.

## Task Parameters
- **Variant**: V1
- **Operation Ratio**: A=5, B=5, C=50
- **Sort Task**: S4 (sort by surname, name)

## Structure
```
include/
  Student.h      - Student struct with CSV parsing
  Database.h     - Three database implementations
  Sorter.h       - Standard and radix sort
src/
  main.cpp       - Benchmark and demo
```

## Build
```bash
mkdir build && cd build
cmake ..
make
```

## Run
```bash
./student_db
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

Output: `students_sorted.csv` (if students.csv exists)
