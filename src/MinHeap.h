#pragma once
#include <vector>
#include <cstdint>
#include <utility>   // std::swap

struct Node {
    uint64_t freq;
    int symbol;          // 0-255 for leaves, -1 for internal nodes
    int order;           // creation order, breaks ties
    Node *left = nullptr, *right = nullptr;
};

class MinHeap {
    std::vector<Node*> a;

    // true if x should sit ABOVE y in the heap
    static bool less(const Node* x, const Node* y) {
        if (x->freq != y->freq) return x->freq < y->freq;
        return x->order < y->order;          // tie-break
    }

    void siftUp(size_t i) {
        while (i > 0) {
            size_t p = (i - 1) / 2;
            if (less(a[i], a[p])) {
                std::swap(a[i], a[p]);
                i = p;
            } else break;
        }
    }

    void siftDown(size_t i) {
        while (true) {
            size_t l = 2 * i + 1, r = 2 * i + 2, smallest = i;
            if (l < a.size() && less(a[l], a[smallest])) smallest = l;
            if (r < a.size() && less(a[r], a[smallest])) smallest = r;
            if (smallest == i) break;
            std::swap(a[i], a[smallest]);
            i = smallest;
        }
    }

public:
    void push(Node* n) {
        a.push_back(n);
        siftUp(a.size() - 1);
    }

    Node* pop() {
        Node* top = a[0];
        a[0] = a.back();
        a.pop_back();
        if (!a.empty()) siftDown(0);
        return top;
    }

    size_t size() const { return a.size(); }
};