#include <iostream>
#include "HuffmanTree.h"
#include "Canonical.h"

int main() {
    uint64_t freq[256] = {0};
    freq['a'] = 3; freq['b'] = 2; freq['c'] = 1;

    Node* root = buildTree(freq);
    int lengths[256] = {0};
    computeLengths(root, 0, lengths);
    if (root && root->symbol >= 0) lengths[root->symbol] = 1;

    auto codes = buildCanonical(lengths);
    for (auto& e : codes) {
        std::cout << (char)e.symbol << " : ";
        for (int b = e.len - 1; b >= 0; b--) std::cout << ((e.code >> b) & 1);
        std::cout << "\n";
    }
}