#pragma once
#include <vector>
#include <cstdint>
#include <stdexcept>
#include "HuffmanTree.h"
#include "Canonical.h"
#include "BitIO.h"

// ---------- helpers ----------

// append a 64-bit number as 8 bytes, least significant byte first
inline void writeU64(std::vector<uint8_t>& out, uint64_t v) {
    for (int i = 0; i < 8; i++)
        out.push_back((uint8_t)((v >> (8 * i)) & 0xFF));
}

// read 8 bytes back into a 64-bit number (same byte order as writeU64)
inline uint64_t readU64(const uint8_t* p) {
    uint64_t v = 0;
    for (int i = 7; i >= 0; i--) v = (v << 8) | p[i];
    return v;
}

// ---------- compress ----------
// File format:
//   4 bytes   magic "HUF1"
//   8 bytes   original size
//   256 bytes code length of every byte value (0 = unused)
//   ...       packed bits, last byte zero-padded

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

// ---------- decompress ----------

inline std::vector<uint8_t> decompressBytes(const std::vector<uint8_t>& in) {
    const size_t HEADER = 4 + 8 + 256;
    if (in.size() < HEADER || in[0] != 'H' || in[1] != 'U' || in[2] != 'F' || in[3] != '1')
        throw std::runtime_error("Not a valid HUF1 file");

    uint64_t origSize = readU64(&in[4]);
    int lengths[256];
    for (int i = 0; i < 256; i++) lengths[i] = in[12 + i];

    std::vector<uint8_t> out;
    if (origSize == 0) return out;                    // empty original

    // build the decoding trie from the canonical codes
    struct TNode { int child[2]; int symbol; };
    std::vector<TNode> trie(1, TNode{{-1, -1}, -1});
    for (const CodeEntry& e : buildCanonical(lengths)) {
        int cur = 0;
        for (int b = e.len - 1; b >= 0; b--) {
            int bit = (int)((e.code >> b) & 1);
            if (trie[cur].child[bit] == -1) {
                trie[cur].child[bit] = (int)trie.size();
                trie.push_back(TNode{{-1, -1}, -1});
            }
            cur = trie[cur].child[bit];
        }
        trie[cur].symbol = e.symbol;
    }

    // decode bit by bit, stop at exactly origSize bytes
    BitReader r(in.data() + HEADER, in.size() - HEADER);
    out.reserve((size_t)origSize);
    int cur = 0;
    while (out.size() < origSize) {
        if (!r.hasMore()) throw std::runtime_error("Corrupt file: data ended early");
        cur = trie[cur].child[r.readBit()];
        if (cur == -1) throw std::runtime_error("Corrupt file: invalid code");
        if (trie[cur].symbol >= 0) {
            out.push_back((uint8_t)trie[cur].symbol);
            cur = 0;                                  // back to the root
        }
    }
    return out;
}