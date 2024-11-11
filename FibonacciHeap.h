#ifndef FIBONACCIHEAP_H
#define FIBONACCIHEAP_H

#include <iostream>
#include <vector>

// Fibonacci Heap: Node structure definition
struct Node {
    int key;              // The value of the node
    int degree;           // The degree of the node (number of children)
    bool marked;          // Whether the node is marked for cascading cut
    Node* parent;         // Pointer to the parent node
    Node* child;          // Pointer to the first child node
    Node* right;          // Pointer to the next node in the circular doubly linked list
    Node* left;           // Pointer to the previous node in the circular doubly linked list

    Node(int key) : key(key), degree(0), marked(false), parent(nullptr), child(nullptr), right(this), left(this) {}
};

// ibonacci Heap: FibonacciHeap class definition
class FibonacciHeap {
private:
    Node* min;  // Pointer to the minimum node in the heap
    int nodeCount;          // The number of nodes in the heap

public:
    // Constructor
    FibonacciHeap() : min(nullptr), nodeCount(0) {};

    // Helper function to link the root lists
    void link(Node* y, Node* x) ;
    
    // Helper function to consolidate the heap after extracting the minimum
    void consolidate();

    // Helper function to cut a node from its parent and attach to the root list
    void cut(Node* x, Node* y) ;

    // Helper function for the cascading cut operation
    void cascadingCut(Node* node);

    // Inserts a new node with the specified key into the heap
    void insert(int key);

    // Returns the minimum node in the heap
    Node* minimum() ;

    // Extracts the minimum node from the heap
    Node* extractMin();

    // Merges two Fibonacci heaps
    void unionHeap(FibonacciHeap& other);

    // Decreases the key of a node to the specified value
    void decreaseKey(Node* node, int newKey);

    // Deletes a node from the heap
    void deleteNode(Node* node);

    // Frees the memory used by the heap
    void freeHeap();

    // Helper function to print the heap
    void printHeap(Node* node, int depth = 0);

    // Helper function to free the nodes
    void freeNodes(Node* node);

    // Accessors and mutators for heap properties
    int size();
};

#endif // FIBONACCIHEAP_H
