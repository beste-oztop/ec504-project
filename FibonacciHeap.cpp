#include "FibonacciHeap.h"
#include <iostream>
#include <unordered_map>
#include <climits> // For INT_MIN
#include <cmath> // For Log operation
#include <vector>


/*
 * ******** MAIN FUNCTION DEFINITIONS ********
 * All of the functions are created following the pseudo-code from CLRS 3rd Edition.
 *
 * MAKE-HEAP
 *   - Creates an empty Fibonacci heap.
 * 
 * INSERT (insert)
 *   - Inserts a new node with a specified value into the Fibonacci heap.
 *   - Adjusts the minimum if necessary.   
 *   - Returns a pointer to the newly inserted node.
 *
 * MINIMUM (minimum)
 *   - Returns the current minimum in the Fibonacci heap.
 *   - Returns NULL if the heap is empty.
 * 
 * EXTRACT-MIN (extractMin)
 *   - Removes the minimum node in the heap.
 *   - Updates the heap structure after removing the minimum by consolidating trees of the same degree.
 *   - Adjusts the minimum pointer.
 * 
 * UNION (unionHeap)
 *   - Merges two Fibonacci heaps by concatenating their circular root lists.
 *   - Updates the min pointer of the merged heap accordingly.
 * 
 * DECREASE-KEY (decreaseKey)
 *   - Decreases the key of a given node to a specified value.
 *   - Applies cut and cascading cut operations if the heap properties are violated.
 *   - Updates the minimum pointer if the decreased key becomes the new minimum.
 *
 * DELETE (deleteNode)
 *   - Decreases the key to a minimum (very little) value, then applies EXTRACT-MIN.
 * 
 * 
 * ********HELPER FUNCTIONS********
 * cut: 
 *  - Detaches a node from its parent and moves the detached node to the root list.
 *  - Updates pointers.
 * 
 * link:
 *   - Links the node as a schild of another node, updates pointers to maintain the heap structure.
 * 
 * consolidate:
 *   - Combines nodes with the same degree to restructure the heap.
 *   - Ensures unique degree for each node of Fib Heap. 
 *
 * cascadingCut:
 *   - Recursively cuts nodes from their parents if they are marked
 *   - Maintains heap properties after a decrease-key operation.
 * 
 * printHeap: 
 *   - Helper function to check the order of the heap.
 * 
 * free: 
 *   - Function to delete the content of heap and free the memory. 
 * 

 */

void FibonacciHeap::link(Node* y, Node* x) {
    y->left->right = y->right;
    y->right->left = y->left;
    y->parent = x;
    if (x->child == nullptr) {
        x->child = y;
        y->left = y;
        y->right = y;
    } else {
        y->right = x->child;
        y->left = x->child->left;
        x->child->left->right = y;
        x->child->left = y;
    }
    x->degree++;
    y->marked = false;
}

void FibonacciHeap::consolidate() {
    int maxDegree = std::log2(nodeCount) + 1;
    std::vector<Node*> degreeArray(maxDegree + 1, nullptr);

    std::vector<Node*> rootList;
    Node* current = min;
    if (current != nullptr) {
        do {
            rootList.push_back(current);
            current = current->right;
        } while (current != min);
    }

    for (Node* w : rootList) {
        Node* x = w;
        int d = x->degree;
        while (degreeArray[d] != nullptr) {
            Node* y = degreeArray[d];
            if (x->key > y->key) std::swap(x, y);
            link(y, x);
            degreeArray[d] = nullptr;
            d++;
        }
        degreeArray[d] = x;
    }

    min = nullptr;
    for (Node* y : degreeArray) {
        if (y != nullptr) {
            if (min == nullptr) {
                min = y;
                min->left = min;
                min->right = min;
            } else {
                y->right = min;
                y->left = min->left;
                min->left->right = y;
                min->left = y;
                if (y->key < min->key) {
                    min = y;
                }
            }
        }
    }
}

void FibonacciHeap::cut(Node* x, Node* y) {
    if (x->right == x) {
        y->child = nullptr;
    } else {
        x->left->right = x->right;
        x->right->left = x->left;
        if (y->child == x) {
            y->child = x->right;
        }
    }
    y->degree--;

    // Add x to the root list
    x->right = min;
    x->left = min->left;
    min->left->right = x;
    min->left = x;

    x->parent = nullptr;
    x->marked = false;
}

void FibonacciHeap::cascadingCut(Node* y) {
    Node* z = y->parent;
    if (z != nullptr) {
        if (!y->marked) {
            y->marked = true;
        } else {
            cut(y, z);
            cascadingCut(z);
        }
    }
}





Node* MAKE_HEAP() {
    return nullptr;
}

void FibonacciHeap::insert(int key) {
    // New elements should be inserted to the right of the current minimum
    Node* node = new Node(key);
    if (min == nullptr) { // empty heap
        min = node;
    } else {
        node->right = min->right;
        node->left = min;
        min->right->left = node;
        min->right = node;

        // Update the minimum node if the new node has a smaller key
        if (node->key < min->key) {
            min = node;
        }
    }
    nodeCount++;
}

Node* FibonacciHeap::minimum() {
    return min;
}

Node* FibonacciHeap::extractMin() {
    Node* z = min;
    if (z != nullptr) {
        if (z->child != nullptr) {
            Node* child = z->child;
            do {
                Node* nextChild = child->right;
                child->left->right = child->right;
                child->right->left = child->left;

                child->right = min;
                child->left = min->left;
                min->left->right = child;
                min->left = child;
                child->parent = nullptr;

                child = nextChild;
            } while (child != z->child);
        }

        z->left->right = z->right;
        z->right->left = z->left;

        if (z == z->right) {
            min = nullptr;
        } else {
            min = z->right;
            consolidate();
        }
        nodeCount--;
    }
    return z;
}

void FibonacciHeap::unionHeap(FibonacciHeap& other) {
    if (other.min == nullptr) return;

    if (min == nullptr) {
        min = other.min;
    } else {
        min->right->left = other.min->left;
        other.min->left->right = min->right;
        min->right = other.min;
        other.min->left = min;

        if (other.min->key < min->key) {
            min = other.min;
        }
    }
    nodeCount += other.nodeCount;
    other.min = nullptr;
    other.nodeCount = 0;
}

void FibonacciHeap::decreaseKey(Node* x, int k) {
    if (k > x->key) {
        std::cerr << "New key is greater than current key" << std::endl;
        return;
    }
    x->key = k;
    Node* y = x->parent;
    if (y != nullptr && x->key < y->key) {
        cut(x, y);
        cascadingCut(y);
    }
    if (x->key < min->key) {
        min = x;
    }
}

void FibonacciHeap::deleteNode(Node* x) {
    decreaseKey(x, INT_MIN);
    extractMin();
}


void FibonacciHeap::printHeap(Node* node, int depth)  {
    if (node == nullptr) return;
    Node* start = node;
    do {
        for (int i = 0; i < depth; i++) std::cout << "  ";
        std::cout << node->key << (node->marked ? "*" : "") << "\n";
        if (node->child != nullptr) {
            printHeap(node->child, depth + 1);
        }
        node = node->right;
    } while (node != start);
}

void FibonacciHeap::freeNodes(Node* node) {
    if (node == nullptr) return;
    Node* start = node;
    do {
        Node* child = node->child;
        if (child != nullptr) {
            freeNodes(child);
        }
        Node* next = node->right;
        delete node;
        node = next;
    } while (node != start);
}

int FibonacciHeap::size() { return nodeCount; }
