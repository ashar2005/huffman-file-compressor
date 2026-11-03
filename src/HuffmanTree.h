#pragma once
#include <cstdint>
#include "MinHeap.h"

inline Node* buildTree(const uint64_t freq[256]) {
    MinHeap heap;

    // TODO 1: one leaf per used byte. order = the byte value itself
    for (int i = 0; i < 256; i++)
        if (freq[i] > 0)
            heap.push(new Node{freq[i], i, i});

    if (heap.size() == 0) return nullptr;            // empty file

    int nextOrder = 256;
    while (heap.size() > 1) {
        // TODO 2: take the two smallest, merge, put the parent back
        Node* a = heap.pop();                        // smallest  -> left
        Node* b = heap.pop();                        // 2nd smallest -> right
        Node* parent = new Node{a->freq + b->freq, -1, nextOrder++};
        parent->left = a;
        parent->right = b;
        heap.push(parent);
    }
    return heap.pop();                               // the root
}

inline void computeLengths(const Node* node, int depth, int lengths[256]) {
    if (!node) return;
    // TODO 3: leaf -> record depth; internal node -> go deeper
    if (node->symbol >= 0) {
        lengths[node->symbol] = depth;
    } else {
        computeLengths(node->left,  depth + 1, lengths);
        computeLengths(node->right, depth + 1, lengths);
    }
}