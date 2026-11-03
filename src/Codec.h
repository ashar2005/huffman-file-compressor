#pragma once
#include <vector>
#include <cstdint>
#include "HuffmanTree.h"
#include "Canonical.h"
#include "BitIO.h"

// append a 64-bit number as 8 bytes, least significant byte first
inline void writeU64(std::vector<uint8_t>& out, uint64_t v) {
    for (int i = 0; i < 8; i++)
        out.push_back((uint8_t)((v >> (8 * i)) & 0xFF));
}

inline std::vector<uint8_t> compressBytes(const std::vector<uint8_t>& data) {
    // 1. count byte frequencies
    uint64_t freq[256] = {0};
    for (uint8_t b : data) freq[b]++;

    // 2. build tree, get code lengths (nullptr root = empty file)
    int lengths[256] = {0};
    Node* root = buildTree(freq);
    computeLengths(root, 0, lengths);
    if (root && root->symbol >= 0) lengths[root->symbol] = 1;   // single-byte-value file

    // 3. canonical codes, then lookup tables indexed by byte value
    std::vector<CodeEntry> entries = buildCanonical(lengths);
    uint64_t codeOf[256] = {0};
    int lenOf[256] = {0};
    for (const CodeEntry& e : entries) {
        codeOf[e.symbol] = e.code;
        lenOf[e.symbol] = e.len;
    }

    // 4. header
    std::vector<uint8_t> out;
    out.push_back('H'); out.push_back('U'); out.push_back('F'); out.push_back('1');
    writeU64(out, data.size());
    for (int i = 0; i < 256; i++) out.push_back((uint8_t)lengths[i]);

    // 5. packed data
    BitWriter w;
    for (uint8_t b : data) w.writeBits(codeOf[b], lenOf[b]);
    w.flush();
    out.insert(out.end(), w.bytes().begin(), w.bytes().end());

    return out;
}