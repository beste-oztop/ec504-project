#include "ViolationHeap.h"
#include <unordered_map>
#include <iostream>

int main() {
    const unsigned int N = 1000;
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
