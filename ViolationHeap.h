#ifndef VIOL
#define VIOL

typedef struct v_node *v_node_T;
typedef struct v_heap *v_heap_T;

void validate_heap_structure(v_heap *heap);
//frees all memory allocated in the use of the heap the sets *heap to NULL;
void v_heap_free(v_heap_T *heap);
// insert a node into the heap with value and ver specified and returns a 
// pionter to that node;
v_node_T v_heap_insert(v_heap_T heaps, int value, int ver);
// merges the two fibonnaci heaps
v_heap_T merge_v_heaps( v_heap_T heap1, v_heap_T heap2);
// returns the vertax value of the min of the heap
int v_heap_min(v_heap_T heap1);
//removes the minimum element from the node
int v_extract_min(v_heap_T heap1);
//prints the entire contense of the heap, include what its neighbor is
void v_print_heap(v_heap_T heap);
//decrease the key of the node the pointer points to to the specified value
void v_heap_decrease_key(v_heap_T heap1, v_node_T node, int value);
//creates a new heap and returns it 
v_heap_T v_heap_new();

int get_key(v_node_T node);
int get_num_elements(v_heap_T heap);

#endif
