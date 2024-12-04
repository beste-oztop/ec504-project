#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ViolationHeap.h"
#include <unordered_map>
#include <iostream>

// testbench credit: sina karimi 

int main() {
    const unsigned int N = 10;
    v_heap_T pq = v_heap_new();
    std::unordered_map<int, v_node_T> entries;
    entries.reserve(N);

    try {
        int i = 1;
        int n = 0;

        for (int x = 1; x < 10; x++) {
            n = N;
            std::cout << "\ninserting " << n << " ...";
            std::cout.flush();
            for (unsigned long i = x; i < N; i++) {
                entries[i] = v_heap_insert(pq, (i + i) * x, i * 0.09);
            }

            n = N / x;
            i = 1;
            std::cout << "\ndecreasing " << n << " ...";
            std::cout.flush();
            for (auto it = entries.begin(); it != entries.end() && i < n; it++) {
                v_heap_decrease_key(pq, it->second, (get_key(it->second) / 2));
                ++i;
            }

            n = N / x;
            i = 1;
            std::cout << "\ndeleting-min " << n << " ..." << std::endl;
            std::cout.flush();
            for (auto it = entries.begin(); it != entries.end() && i < n; it++) {  
                v_extract_min(pq);
                ++i;
            }

        }

        std::cout << "\ndeleting-min remaining " << get_num_elements(pq) << "...";
        std::cout.flush();
        i = 1;
        while (get_num_elements(pq) > 0) {
            ++i;
            // printf("heap->num_elem = %d\n", get_num_elements(pq));
            int min = v_extract_min(pq);
            // validate_heap_structure(pq);
            std::cout << min << "\n";
            std::cout.flush();
        }
    } catch (...) {
        std::cout << "An error occurred.\n";
    }

    v_heap_free(&pq);
    std::cout << "\nGoodbye\n";
    return 0;
}