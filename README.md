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