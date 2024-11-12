#include "ViolationHeap.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Forward declarations for helper functions
void free_level(f_node *node);  // Declares free_level so it can be used in free_f_heap
void initialize_node(f_node *node, int value, int ver);
f_node* merge_nodes(f_node* node1, f_node* node2);  // Used within correct_heap

// Creates a new Fibonacci heap
f_heap* create_f_heap() {
    f_heap* heap = (f_heap*)malloc(sizeof(f_heap));
    heap->min = NULL;
    heap->num_elem = 0;
    return heap;
}

// Frees all nodes in the heap and sets the heap pointer to NULL
void free_f_heap(f_heap **heap) {
    if (*heap == NULL) return;
    free_level((*heap)->min);  // Calls helper function to free all nodes
    free(*heap);
    *heap = NULL;
}

// Frees all nodes recursively in the heap
void free_level(f_node *node) {
    if (!node) return;

    f_node *current = node;
    do {
        // Free children of the current node
        if (current->down) {
            free_level(current->down);
        }
        f_node *next = current->right;
        free(current);
        current = next;
    } while (current != node);
}

// Inserts a node with a given value and vertex into the heap
f_node* insert_into_f_heap(f_heap *heap, int value, int ver) {
    f_node* new_node = (f_node*)malloc(sizeof(f_node));
    initialize_node(new_node, value, ver);

    if (!heap->min) {
        new_node->left = new_node;
        new_node->right = new_node;
        heap->min = new_node;
    } else {
        f_node* temp = heap->min->left;
        temp->right = new_node;
        new_node->left = temp;
        new_node->right = heap->min;
        heap->min->left = new_node;
        if (heap->min->data > value) {
            heap->min = new_node;
        }
    }
    heap->num_elem++;
    return new_node;
}

// Initializes a node with default values
void initialize_node(f_node *node, int value, int ver) {
    node->up = NULL;
    node->down = NULL;
    node->left = NULL;
    node->right = NULL;
    node->data = value;
    node->colored = 0;
    node->vertex = ver;
    node->children = 0;
}

// Prints all nodes in the root list of the heap
void print_root_list(f_heap *heap) {
    if (!heap || !heap->min) {
        printf("Heap is empty\n");
        return;
    }
    f_node *current = heap->min;
    do {
        printf("%d -> ", current->data);
        current = current->right;
    } while (current != heap->min);
    printf("END\n");
}

// Returns the minimum value in the heap
int get_f_heap_min(f_heap *heap) {
    if (!heap || !heap->min) return INT_MAX;
    return heap->min->data;
}

// Prints the entire heap (for testing/debugging purposes)
void print_f_heap(f_heap *heap) {
    if (!heap || !heap->min) {
        printf("Heap is empty\n");
        return;
    }
    printf("Heap elements:\n");
    print_root_list(heap);
}

// Extracts the minimum node from the heap and returns its value
int extract_f_heap_min(f_heap *heap) {
    if (!heap->min) return INT_MAX;  // Heap is empty

    f_node *minNode = heap->min;
    int minValue = minNode->data;

    // Handle children of the minimum node
    f_node *child = minNode->down;
    if (child) {
        f_node *temp = child;
        do {
            temp->up = NULL;  // Set parent to NULL for all children
            temp = temp->right;
        } while (temp != child);
        
        // Add children to root list
        f_node *minLeft = minNode->left;
        f_node *childLeft = child->left;
        minLeft->right = child;
        child->left = minLeft;
        childLeft->right = minNode;
        minNode->left = childLeft;
    }

    // Remove minNode from the root list
    if (minNode->right == minNode) {
        heap->min = NULL;
    } else {
        minNode->left->right = minNode->right;
        minNode->right->left = minNode->left;
        heap->min = minNode->right;  // Set a new min
    }
    free(minNode);
    heap->num_elem--;

    return minValue;
}

// Decreases the key of a given node to the specified value
void decrease_key_in_f_heap(f_heap *heap, f_node *node, int newValue) {
    if (!node || newValue > node->data) return;  // New value must be smaller
    node->data = newValue;

    f_node *parent = node->up;
    if (parent && node->data < parent->data) {
        // Cut the node and move it to the root list
        if (node->right != node) {
            node->right->left = node->left;
            node->left->right = node->right;
        }
        if (parent->down == node) {
            parent->down = (node->right == node) ? NULL : node->right;
        }
        node->up = NULL;
        node->left = heap->min->left;
        node->right = heap->min;
        heap->min->left->right = node;
        heap->min->left = node;
    }
}
