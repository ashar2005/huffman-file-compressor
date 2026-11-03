#pragma once
#include <cstdint>
#include <vector>
#include <algorithm>

struct CodeEntry {
    int symbol;
    int len;
    uint64_t code;
};

inline std::vector<CodeEntry> buildCanonical(const int lengths[256]) {
    std::vector<CodeEntry> v;
    for (int i = 0; i < 256; i++)
        if (lengths[i] > 0) v.push_back(CodeEntry{i, lengths[i], 0});

    // shortest length first; same length -> smaller byte value first
    std::sort(v.begin(), v.end(), [](const CodeEntry& x, const CodeEntry& y) {
        if (x.len != y.len) return x.len < y.len;
        return x.symbol < y.symbol;
    });

    // assign codes
    if (!v.empty()) {
        v[0].code = 0;
        for (size_t i = 1; i < v.size(); i++) {
            v[i].code = (v[i - 1].code + 1) << (v[i].len - v[i - 1].len);
        }
    }
    return v;
}