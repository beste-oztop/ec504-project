#ifndef VIOLATION
#define VIOLATION

// implementation build on top of: https://violationheap.weebly.com

// main heap and node structs
typedef struct v_node *v_node_T;
typedef struct v_heap *v_heap_T;

// for debugging purposes
void validate_heap_structure(v_heap *heap);

// additional functionality for testbenching on Dijkstra's algorithm
int get_key(v_node_T node);
int get_num_elements(v_heap_T heap);

// creates a new heap
v_heap_T v_heap_new();

// insert a node
v_node_T v_heap_insert(v_heap_T heaps, int value, int ver);

// decrease a key
void v_heap_decrease_key(v_heap_T heap1, v_node_T node, int value);

// removes the minimum element
int v_extract_min(v_heap_T heap1);

// merges two heaps
v_heap_T merge_v_heaps( v_heap_T heap1, v_heap_T heap2);

// returns the key value of the heap->min
int v_heap_min(v_heap_T heap1);

// frees memory
void v_heap_free(v_heap_T *heap);

#endif // VIOLATION