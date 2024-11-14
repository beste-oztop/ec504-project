#include "FibonacciHeap.h"
#include <iostream>

int main() {

    // I will follow the example we had in class
    // Create a Fibonacci Heap
    FibonacciHeap heap;

    // Insert elements into the heap
    // New elements should be inserted to the right of the current minimum
    heap.insert(10);
    heap.insert(5);
    heap.insert(7);
    heap.insert(9);
    heap.insert(20);
    heap.insert(15);
    heap.insert(17);


    // Print the heap after insertions
    std::cout << "Heap after insertions:\n";
    heap.printHeap(heap.minimum(), 0);

    // Extract the minimum element
    std::cout << "\nExtracting minimum...\n";
    heap.extractMin();

    // Print the heap after extracting the minimum
    std::cout << "Heap after extracting minimum:\n";
    heap.printHeap(heap.minimum(), 0);

    // Decrease the key of the node with key 7 to 2
    std::cout << "\nDecreasing key of 7 to 2...\n";

    Node* nodeToDecrease = heap.minimum();
    while (nodeToDecrease && nodeToDecrease->key != 7) {
        nodeToDecrease = nodeToDecrease->right;
    }
    if (nodeToDecrease) {
        heap.decreaseKey(nodeToDecrease, 2);
    }

    // Print the heap after decreasing key
    std::cout << "Heap after decreasing key of 7 to 2:\n";
    heap.printHeap(heap.minimum(), 0);


    // Extract the minimum element for the second time
    std::cout << "\nExtracting minimum...\n";
    heap.extractMin();

    // Print the heap after extracting the minimum for the second time
    std::cout << "Heap after extracting minimum:\n";
    heap.printHeap(heap.minimum(), 0);


    return 0;
}
