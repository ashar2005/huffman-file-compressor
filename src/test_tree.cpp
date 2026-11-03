#include <iostream>
#include "HuffmanTree.h"

int main() {
    uint64_t freq[256] = {0};
    freq['a'] = 3; freq['b'] = 2; freq['c'] = 1;

    Node* root = buildTree(freq);
    int lengths[256] = {0};
    computeLengths(root, 0, lengths);
    if (root && root->symbol >= 0) lengths[root->symbol] = 1;

    for (int i = 0; i < 256; i++)
        if (lengths[i]) std::cout << (char)i << " : " << lengths[i] << " bits\n";
}