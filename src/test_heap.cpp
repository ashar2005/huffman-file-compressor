#include <iostream>
#include "MinHeap.h"

int main() {
    MinHeap h;
    int vals[] = {5, 3, 8, 1, 9, 2, 7, 3};
    for (int i = 0; i < 8; i++) h.push(new Node{(uint64_t)vals[i], -1, i});
    while (h.size()) std::cout << h.pop()->freq << " ";
    std::cout << "\n";
}