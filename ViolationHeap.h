#ifndef VIOLATIONHEAP
#define VIOLATIONHEAP


typedef struct f_node *f_node_T;
typedef struct f_heap *f_heap_T;

//frees all memory allocated in the use of the heap the sets *heap to NULL;
void f_heap_free(f_heap_T *heap);
// insert a node into the heap with value and ver specified and returns a 
// pionter to that node;
f_node_T f_heap_insert(f_heap_T heaps, int value, int ver);
// merges the two fibonnaci heaps
void merge_f_heaps( f_heap_T heap1, f_heap_T heap2);
//prints all element is the root list
void print_root_list(f_heap_T heap);
// returns the vertax value of the min of the heap
int f_heap_min(f_heap_T heap1);
//removes the minimum element from the node
int f_extract_min(f_heap_T heap1);
//prints the entire contense of the heap, include what its neighbor is
void print_heap(f_heap_T heap);
//decrease the key of the node the pointer points to to the specified value
void f_heap_decrease_key(f_heap_T heap1, f_node_T node, int value);
//creates a new heap and returns it 
f_heap_T f_heap_new();
#endif // VIOLATIONHEAP