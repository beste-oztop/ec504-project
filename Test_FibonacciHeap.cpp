#include "FibonacciHeap.cpp"
#include <iostream>
#include <string>

int main() {
    // Create a Fibonacci Heap instance
    FibonacciHeap<int, std::string> fibHeap;

    // Create and insert entries
    auto entry1 = FibonacciHeap<int, std::string>::makeEntry(10, "Ten");
    auto entry2 = FibonacciHeap<int, std::string>::makeEntry(5, "Five");
    auto entry3 = FibonacciHeap<int, std::string>::makeEntry(20, "Twenty");
    auto entry4 = FibonacciHeap<int, std::string>::makeEntry(15, "Fifteen");

    fibHeap.insert(entry1);
    fibHeap.insert(entry2);
    fibHeap.insert(entry3);
    fibHeap.insert(entry4);

    std::cout << "Minimum key: " << fibHeap.minimum()->key() << std::endl;

    // Extract the minimum element
    auto minEntry = fibHeap.extractMin();
    std::cout << "Extracted min: " << minEntry->key() << " -> " << minEntry->value() << std::endl;

    std::cout << "New minimum key: " << fibHeap.minimum()->key() << std::endl;

    // Decrease a key
    fibHeap.decreaseKey(entry3, 1);
    std::cout << "After decreasing key, new minimum key: " << fibHeap.minimum()->key() << std::endl;

    // Test union operation
    FibonacciHeap<int, std::string> fibHeap2;
    auto entry5 = FibonacciHeap<int, std::string>::makeEntry(30, "Thirty");
    auto entry6 = FibonacciHeap<int, std::string>::makeEntry(25, "Twenty Five");

    fibHeap2.insert(entry5);
    fibHeap2.insert(entry6);

    fibHeap.unionHeap(fibHeap2);

    std::cout << "Minimum key after union: " << fibHeap.minimum()->key() << std::endl;

    // Extract all elements to clean up the heap
    while (fibHeap.size() > 0) {
        auto min = fibHeap.extractMin();
        std::cout << "Extracted: " << min->key() << " -> " << min->value() << std::endl;
    }

    return 0;
}
