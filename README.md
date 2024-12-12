# Fibonacci Heaps and Modern Heap-Based Data Structures Comparison, Implementation and Analysis

This project focuses on the implementation and performance comparison of Fibonacci Heaps with three other heap-based data structures, as part of the EC504 Advanced Data Structures course term project.

## Project Overview

In the course lecture, we explored Fibonacci Heaps, which provide efficient operations due to their unique structure:

Min-Heap Ordered Property: Allows finding the minimum key in constant time O(1).
Efficient Operations: Operations such as merging two heaps, decreasing a key, or deleting a node are optimized, making Fibonacci Heaps ideal for specific algorithms like Dijkstra’s shortest path.
Node Degree Bound: The degree of any node is limited to O(log n), maintaining a balanced structure that supports efficient performance.
Given the advantages of Fibonacci Heaps, our project aims to implement this data structure and compare it with other heap-based alternatives by examining their runtime performance in practical scenarios.

## Key Operations to be Compared
We will compare the following fundamental operations across the data structures:

* Insert
* Delete
* Extract-Min
* Decrease-Key
* Objectives

## Main Goal
The primary objective of this project is to:

* Implement Fibonacci Heaps and three additional heap-based data structures.
* Develop a comprehensive testbench to benchmark and compare the performance of all implemented data structures.
* Analyze runtime performance for each data structure under uniform test conditions, focusing on time complexity for the key operations listed above.

We plan to identify the most efficient data structure based on the performance metrics. Once determined, we will apply the chosen structure to a real-world application, which will be finalized after the benchmarking stage.


## Fibonacci Heap
Fibonacci Heaps provide an efficient data structure for priority queue operations with the following amortized time complexities: O(1) for insert, find-min, and decrease-key, and O(log n) for delete and extract-min. The primary advantage of Fibonacci Heaps lies in their ability to perform decrease-key operations very efficiently, making them suitable for algorithms like Dijkstra's and Prim's.
Fibonacci Heaps consist of a collection of heap-ordered trees, with each tree adhering to the min-heap property. These trees are linked together using a circular doubly linked list. The structure is designed to perform certain operations more efficiently than other heap variants by utilizing a lazy approach to restructuring. For instance, Fibonacci Heaps postpone the consolidation of trees until it is necessary, reducing the time complexity for many operations.
* We implemented Fibonacci Heap in FibonacciHeap.h and FibonacciHeap.cpp
* We have tested the implementation by using main.cpp for an example on a Fibonacci heap (from lectures).

## Rank-Pairing Heap
Rank-pairing Heaps support the same operations with similar amortized time complexity: O(1) for insert, find-min, and decrease-key, and O(log n) for delete and extract-min as Fibonacci Heaps.
Unlike Fibonacci Heaps, Rank-pairing Heaps do not require complex adjustments like cascading cuts. Instead, they rely on a simpler rank-based system that adjusts the ranks of nodes dynamically during the operations, resulting in a more flexible tree structure that avoids costly restructuring. This makes Rank-pairing Heaps simpler and easier to implement yet highly efficient.
* We implemented Rank-Pairing Heap in RankPairingHeap.h
* We have tested the implementation by using Test.cpp for a couple of operations on a Rank-Pairing heap


## Violation Heap
Violation Heaps are a specialized variant of heap data structures designed to handle specific constraints on heap operations, particularly for cases where violations (or exceptions to heap properties) are allowed temporarily but need to be corrected efficiently. These heaps support similar operations to other heap structures but allow for certain violations in the tree structure.
The operations in Violation Heaps are similar in terms of time complexity to Fibonacci Heaps.
* We implemented Violation Heap in ViolationHeap.h and ViolationHeap.cpp


## Quake Heap
Quake heap is another variation of Fibonacci heap where it utilizes tournament trees instead of heaps. The goal of quake heap is to reduce the complexity of Fibnacci heap and perform lazy insert and delete. During the extract min operation, it would reconstruct the data structure by making sure all the trees are in reasonable size and performs quaking for the trees with irregular and inefficient structure which can be caused by delete or decrease key operation. The implementation exists in quake_heap.hpp
* We implemented quake heap in quake_heap.hpp
* We teseted the implementation of it using main_QH.cpp for differnt graph sizes
