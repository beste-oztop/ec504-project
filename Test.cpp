#include "ViolationHeap.h"
#include <unordered_map>
#include <iostream>

// credit: prof. sina karimi 

int main() {
    const unsigned int N = 20;
    heap_v pq = create_heap();
    std::unordered_map<int, node_v> entries;
    entries.reserve(N);

    try {
        int i = 0;
        int n = 0;

        for (int x = 1; x < 10; x++) {
            n = N;
            std::cout << "\ninserting " << n << " ...";
            std::cout.flush();
            for (unsigned long i = x; i < N; i++) {
                entries[i] = insert(pq, (i + i) * x, i * 0.09);
                validate_heap_structure(pq);
            }

            n = N / x;
            i = 0;
            std::cout << "\ndecreasing " << n << " ...";
            std::cout.flush();
            for (auto it = entries.begin(); it != entries.end() && i < n; it++) {
                decrease_key(pq, it->second, get_key(it->second) / 2);

                ++i;
            }

            n = N / x;
            i = 0;
            std::cout << "\ndeleting-min " << n << " ...";
            std::cout.flush();
            for (auto it = entries.begin(); it != entries.end() && i < n; it++) {
                extract_min(pq);
                ++i;
            }

            n = N / 4 * x;
            i = 0;
            std::cout << "\ndecreasing " << n << " ...";
            std::cout.flush();
            for (auto it = entries.begin(); it != entries.end() && i < n; it++) {
                decrease_key(pq, it->second, get_key(it->second) / 2);
                ++i;
            }

            n = N / 2 * x;
            i = 0;
            std::cout << "\ndeleting-min " << n << " ...";
            std::cout.flush();
            for (auto it = entries.begin(); it != entries.end() && i < n; it++) {
                extract_min(pq);
                ++i;
            }
        }

        std::cout << "\ndeleting-min remaining " << get_num_elements(pq) << "...";
        std::cout.flush();
        i = 0;
        while (get_num_elements(pq) > 0) {
            ++i;
            int min = extract_min(pq);
            std::cout << min << "\n";
            std::cout.flush();
        }
    } catch (...) {
        std::cout << "An error occurred.\n";
    }

    free_heap(&pq);
    std::cout << "\nGoodbye\n";
    return 0;
}


// int main() {
//     heap_v heap = create_heap();

//     // Insert elements
//     insert(heap, 5, 1);
//     insert(heap, 3, 2);
//     insert(heap, 7, 3);

//     // Extract the minimum
//     printf("Min key: %d\n", v_heap_min(heap));
//     printf("Extracting min: %d\n", extract_min(heap));

//     // Decrease key of a node
//     node_v node = insert(heap, 10, 4);
//     decrease_key(heap, node, 1);

//     // Validate heap
//     validate_heap(heap, "After operations");

//     // Free heap memory
//     free_heap(&heap);

//     return 0;
// }
