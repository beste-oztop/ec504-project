#ifndef VIOLATIONHEAP_H
#define VIOLATIONHEAP_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct V_heap_node *node_v;
typedef struct V_heap *heap_v;

// create a new heap
heap_v create_heap();

// insert a node
node_v insert(heap_v heaps, int value, int ver);

// merges two heaps
heap_v union_heaps( heap_v heap1, heap_v heap2);

// extracting the minimum element
int extract_min(heap_v heap1);

// decrease the key of the node
void decrease_key(heap_v heap1, node_v node, int value);

// returns the vertax value of the min of the heap
int v_heap_min(heap_v heap1);

// prints the heap
void print_heap(heap_v heap);

// free memory
void free_heap(heap_v *heap);

int get_key(node_v node);

int get_num_elements(heap_v heap);

#endif // VIOLATIONHEAP_H
