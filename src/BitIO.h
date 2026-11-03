#pragma once
#include <vector>
#include <cstdint>
#include <cstddef>

class BitWriter {
    std::vector<uint8_t> buf;
    uint8_t cur = 0;     // byte being built
    int nbits = 0;       // bits in cur so far (0..7)

public:
    // write the lowest `len` bits of `code`, most significant bit first
    void writeBits(uint64_t code, int len) {
        for (int b = len - 1; b >= 0; b--) {
            int bit = (code >> b) & 1;
            cur = (uint8_t)((cur << 1) | bit);
            nbits++;
            if (nbits == 8) {
                buf.push_back(cur);
                cur = 0;
                nbits = 0;
            }
        }
    }

    // call once at the end: pad the last partial byte with zeros on the right
    void flush() {
        if (nbits > 0) {
            cur = (uint8_t)(cur << (8 - nbits));
            buf.push_back(cur);
            cur = 0;
            nbits = 0;
        }
    }

    const std::vector<uint8_t>& bytes() const { return buf; }
};

class BitReader {
    const uint8_t* data;
    size_t size;         // number of bytes
    size_t pos = 0;      // current position, counted in BITS

public:
    BitReader(const uint8_t* d, size_t n) : data(d), size(n) {}

    bool hasMore() const { return pos < size * 8; }

    int readBit() {
        uint8_t byte = data[pos / 8];
        int bit = (byte >> (7 - pos % 8)) & 1;
        pos++;
        return bit;
    }
};