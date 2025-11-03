# Student Database - Homework 1

Implementation of student database with three different data structure variants. Supports operations with configurable ratios (V1: 5:5:50) and two sorting approaches.

## Task Parameters

- **Variant**: V1
- **Operation Ratio**: A=5, B=5, C=50
- **Sort Task**: S4 (sort by surname, name)

## Build & Run

```bash
mkdir build && cd build
cmake ..
make
./algo_homework_1
```

Program generates test datasets (100, 1K, 10K, 100K records) and benchmarks all three variants plus sorting algorithms. Results are saved to `benchmark_results.csv` and `sort_results.csv`.

## Visualization

After running benchmarks, generate plots:

```bash
python -m venv .venv
source .venv/bin/activate
python -m pip install matplotlib pandas
python3 plot_results.py
```

This creates `benchmark_comparison.png` and `sort_comparison.png` showing performance metrics.

## Operations

1. **Find students by (name, surname)** - searches for matching records
2. **Find groups with duplicate (name, surname)** - identifies groups containing duplicate students
3. **Update group by email** - modifies group assignment for a student

## Three Implementation Variants

### Variant 1: HashMap (unordered_map) - optimal

- **Data structures**: `unordered_map<string, vector<size_t>>` for name/surname/email indices
- **Complexity**: O(1) average for operations 1 and 3; O(n) for operation 2
- **Memory**: Moderate (~28 MB for 100K records)
- **Performance**: **BEST** - highest operations/10s across all dataset sizes

### Variant 2: Mixed (vector + minimal hash)

- **Data structures**: `vector<Student>` + `unordered_map<string, size_t>` for email only
- **Complexity**: O(n) for operations 1,2; O(1) for operation 3
- **Memory**: **BEST** - lowest memory usage (~27 MB for 100K records)
- **Performance**: Worst - dominated by linear searches (operations 1,2 occur 10% of the time each)

### Variant 3: Map/BST (std::map)

- **Data structures**: `map<string, vector<size_t>>` for name/surname/email indices
- **Complexity**: O(log n) for operations 1,3; O(n) for operation 2
- **Memory**: Highest (~28.5 MB for 100K records)
- **Performance**: Second best - slower than HashMap but maintains sorted order

## Experimental Results & Proof of Optimality

**Test Configuration**: Operations ratio A:B:C = 5:5:50 (5% op1, 5% op2, 90% op3)

### Performance Comparison (Operations completed in 10 seconds)

| Dataset Size | Variant 1 (HashMap) | Variant 2 (Mixed) | Variant 3 (Map/BST) |
|--------------|---------------------|-------------------|---------------------|
| 100          | ~52M ops            | ~5M ops           | ~51M ops            |
| 1,000        | ~1.5M ops           | ~312K ops         | ~1.3M ops           |
| 10,000       | ~45K ops            | ~21K ops          | ~42K ops            |
| 100,000      | ~3.3K ops           | ~1.3K ops         | ~3.3K ops           |

### Why Variant 1 (HashMap) is Optimal

**1. Performance Analysis**

- Consistently achieves **highest operations/10s** across all dataset sizes
- At 100K records: faster than Variant 2 and than Variant 3
- O(1) average lookup dominates performance for operations 1 and 3

**2. Memory Trade-off Justification**

- Small tradoff of memmory for high performance vs Variant2

**3. Operation Ratio Impact**

- Given ratio 5:5:50, operation 3 (update by email) is the biggest part of computatiosn
- HashMap provides O(1) email lookup, directly optimizing the dominant operation
- Operations 1,2 also benefit from O(1) name/surname indexing

**4. Scalability**

- HashMap maintains **constant-time average** complexity as dataset grows
- Variant 2 degrades linearly: 10K→100K = 10x slowdown observed
- Variant 3 degrades logarithmically but with higher constant factors than HashMap

### Conclusion

**Variant 1 (HashMap with unordered_map) is the optimal choice** because:

- Best performance on the primary metric (operations/10s)
- O(1) average complexity for 85% of operations (ratio 5:5:50)
- Minimal memory overhead compared to Variant 2
- Excellent scalability with increasing dataset size
- Simple implementation without complex balancing (vs BST)

## Sorting Algorithms

### Standard Sort (std::sort)

- Uses `StudentComparator` for lexicographic ordering by (surname, name)
- O(n log n) comparison-based sort
- **Faster for this task** - optimized sort implementation

### Radix Sort (MSD)

- Custom MSD radix sort treating strings as byte sequences
- Works correctly with UTF-8 Ukrainian text (Cyrillic byte ordering preserved)
- O(n·k) where k = average string length
- Slower in practice: high constant factors, cache-unfriendly memory access

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
