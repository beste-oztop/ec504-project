#include "RankPairingHeap.h"
#include <iostream>

int main() {
    RankPairingHeap<int> heap;

    // Inserting elements and storing node references
    auto* node10 = heap.Insert(1);
    auto* node5 = heap.Insert(5);
    auto* node20 = heap.Insert(20);
    auto* node1 = heap.Insert(1);
    auto* node50 = heap.Insert(50);
    auto* node200 = heap.Insert(200);
    auto* node3 = heap.Insert(3);
    auto* node4 = heap.Insert(4);
    auto* node40 = heap.Insert(40);
    auto* node130 = heap.Insert(130);
    auto* node2 = heap.Insert(2);
    auto* node21 = heap.Insert(21);

    // Extracting the minimum element
    int minValue = heap.ExtractMin();
    std::cout << "Extracted Min: " << minValue << std::endl;

    // Extracting the next minimum element
    minValue = heap.ExtractMin();
    std::cout << "Extracted Min: " << minValue << std::endl;

    // Extracting the next minimum element
    minValue = heap.ExtractMin();
    std::cout << "Extracted Min: " << minValue << std::endl;

    // Decreasing a key using the node reference
    heap.DecreaseKey(node20, 1);
    std::cout << "Decreased key from 20 to 1." << std::endl;

    minValue = heap.ExtractMin();
    std::cout << "Extracted Min after DecreaseKey: " << minValue << std::endl;

    // Deleting specific keys using their node references
    heap.Delete(node200);
    std::cout << "Deleted key 10." << std::endl;

    heap.Delete(node4);
    std::cout << "Deleted key 4." << std::endl;

    heap.Delete(node5);
    std::cout << "Deleted key 5." << std::endl;

    // Extracting the next minimum
    minValue = heap.ExtractMin();
    std::cout << "Extracted Min after Delete: " << minValue << std::endl;

    minValue = heap.ExtractMin();
    std::cout << "Extracted Min after Delete: " << minValue << std::endl;

    minValue = heap.ExtractMin();
    std::cout << "Extracted Min after Delete: " << minValue << std::endl;

    return 0;
}
