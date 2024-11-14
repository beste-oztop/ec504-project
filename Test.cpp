#include "ViolationHeap.h" 
#include <iostream>

int main() {
    V_heap* v_heap = create_heap();
    
    // insert nodes with values and keys
    insert(v_heap, 10, 1);
    insert(v_heap, 20, 2);
    insert(v_heap, 15, 3);
    insert(v_heap, 5, 4);
    insert(v_heap, 25, 5);

    std::cout << "Inserted elements into the heap." << std::endl;
    // print_heap(v_heap);
    std::cout << "Extracted minimum: " << extract_min(v_heap) << std::endl;
    // print_heap(v_heap);
    
    // Free the heap
    free(v_heap);

    return 0;
}
