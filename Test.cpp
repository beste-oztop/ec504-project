#include "ViolationHeap.h"
#include <iostream>

int main() {
    ViolationHeap heap;

    // Inserting elements
    heap.Insert(10);
    heap.Insert(5);
    heap.Insert(20);
    heap.Insert(1);
    heap.Insert(50);
    heap.Insert(200);
    heap.Insert(3);
    heap.Insert(4);
    heap.Insert(40);
    heap.Insert(130);
    heap.Insert(2);
    heap.Insert(21);

    // Extracting the minimum element
    int minValue = heap.ExtractMin();
    std::cout << "Extracted Min: " << minValue << std::endl;

    // Decreasing a key
    heap.DecreaseKey(20, 1);
    std::cout << "Decreased key from 20 to 1." << std::endl;

    minValue = heap.ExtractMin();
    std::cout << "Extracted Min after DecreaseKey: " << minValue << std::endl;

    // Deleting a key
    heap.Delete(10);
    std::cout << "Deleted key 10." << std::endl;

    // Extracting the next minimum
    minValue = heap.ExtractMin();
    std::cout << "Extracted Min after Delete: " << minValue << std::endl;

    return 0;
}
