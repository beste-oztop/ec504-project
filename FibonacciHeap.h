#ifndef FIBONACCIHEAP_H
#define FIBONACCIHEAP_H

#include <iostream>
#include <vector>

template <typename Key, typename Value>
class FibonacciHeap;


template <typename Key, typename Value>
struct TEntry {
    TEntry() : _node(nullptr) {}
    TEntry(Key k, Value v) : _key(k), _value(v), _node(nullptr) {}
    Key key() const { return _key; }
    Value value() const { return _value; }

private:
    friend class FibonacciHeap<Key, Value>; // Grant access to private members
    Key _key;
    Value _value;
    struct Node* _node; // Internal pointer to the heap node
};


template <typename Key, typename Value>
class FibonacciHeap {// Fibonacci Heap: Node structure definition
    struct Node {
        Key key;                    // The value of the node
        Node* parent;               // Pointer to the parent node
        Node* child;                // Pointer to the first child node
        Node* left;                 // Pointer to the previous node in the circular doubly linked list
        Node* right;                // Pointer to the next node in the circular doubly linked list
        int degree;                 // The degree of the node (number of children)
        bool marked;                // Whether the node is marked for cascading cut
        TEntry<Key, Value>* entry;  // Back-reference to the entry

        Node(Key k) 
            : key(k), parent(nullptr), child(nullptr), 
              left(this), right(this), degree(0), marked(false), entry(nullptr) {}
    };

private:
    Node* min;              // Pointer to the minimum node in the heap
    int nodeCount;          // The number of nodes in the heap

public:
    using Entry = TEntry<Key, Value>*;
    using Exception = std::runtime_error;

    static Entry makeEntry(const Key& key, const Value& value) {
        return new TEntry<Key, Value>(key, value);
    }

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
    void insert(Entry& entry);

    // Returns the minimum node in the heap
    Entry minimum() ;

    // Extracts the minimum node from the heap
    Entry extractMin();

    // Merges two Fibonacci heaps
    void unionHeap(FibonacciHeap& other);

    // Decreases the key of a node to the specified value
    void decreaseKey(Entry& entry, Key newKey);

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
