#ifndef VIOLATIONHEAP
#define VIOLATIONHEAP

#include <limits.h>

// Define structures for f_node and f_heap
struct f_node {
    struct f_node *up;
    struct f_node *down;
    struct f_node *left;
    struct f_node *right;
    int vertex;
    int data;
    int children;
    char colored;
};

struct f_heap {
    f_node *min;
    int num_elem;
};

// Function prototypes with unique names
f_heap* create_f_heap();  // Renamed from f_heap_new
void free_f_heap(f_heap **heap);  // Renamed from f_heap_free
f_node* insert_into_f_heap(f_heap *heap, int value, int ver);  // Renamed from f_heap_insert
void merge_heaps(f_heap *heap1, f_heap *heap2);  // Merging two heaps
void print_root_list(f_heap *heap);  // Print root list of heap
int get_f_heap_min(f_heap *heap);  // Renamed from f_heap_min
int extract_f_heap_min(f_heap *heap);  // Renamed from f_extract_min
void print_f_heap(f_heap *heap);  // Renamed from print_heap
void decrease_key_in_f_heap(f_heap *heap, f_node *node, int value);  // Renamed from f_heap_decrease_key

class ViolationHeap {
private:
    f_heap *heap;

    f_node* findNode(int value);
    f_node* findNodeRecursive(f_node *node, int value);

public:
    ViolationHeap();
    ~ViolationHeap();

    void Insert(int value, int ver = 0);
    int ExtractMin();
    void DecreaseKey(int oldValue, int newValue);
    void Delete(int value);
    void PrintRootList();
    void PrintHeap();
    int Min() const;
};

#endif // VIOLATIONHEAP
