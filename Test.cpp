#include "ViolationHeap.h" 
#include <iostream>

int main() {
    v_heap* heap = v_heap_new();
    
    // Insert nodes with values and vertex IDs
    v_heap_insert(heap, 10, 1);
    v_heap_insert(heap, 20, 2);
    v_heap_insert(heap, 15, 3);
    v_heap_insert(heap, 5, 4);
    v_heap_insert(heap, 25, 5);

    std::cout << "Inserted elements into the heap." << std::endl;
    v_print_heap(heap);
    std::cout << "Extracted minimum: " << v_extract_min(heap) << std::endl;
    v_print_heap(heap);
    
    // Free the heap
    v_heap_free(&heap);

    return 0;
}
