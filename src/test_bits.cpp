#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include "BitIO.h"

int main() {
    // Test 1: aaabbc with a=0, b=10, c=11  -> expect "15 80" and 000101011
    {
        BitWriter w;
        w.writeBits(0, 1); w.writeBits(0, 1); w.writeBits(0, 1);
        w.writeBits(2, 2); w.writeBits(2, 2);
        w.writeBits(3, 2);
        w.flush();

        for (uint8_t b : w.bytes())
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b << " ";
        std::cout << std::dec << "\n";

        BitReader r(w.bytes().data(), w.bytes().size());
        for (int i = 0; i < 9; i++) std::cout << r.readBit();
        std::cout << "\n";
    }

    // Test 2: exactly 8 bits -> exactly 1 byte
    {
        BitWriter w;
        w.writeBits(0xFF, 8);
        w.flush();
        std::cout << "8 bits -> " << w.bytes().size() << " byte(s) (expect 1)\n";
    }

    // Test 3: nothing written -> empty
    {
        BitWriter w;
        w.flush();
        std::cout << "0 bits -> " << w.bytes().size() << " byte(s) (expect 0)\n";
    }

    // Test 4: round trip with 1000 random codes
    {
        std::vector<int> lens;
        std::vector<uint64_t> codes;
        BitWriter w;
        for (int i = 0; i < 1000; i++) {
            int len = 1 + rand() % 20;
            uint64_t code = rand() & ((1u << len) - 1);
            lens.push_back(len);
            codes.push_back(code);
            w.writeBits(code, len);
        }
        w.flush();

        BitReader r(w.bytes().data(), w.bytes().size());
        bool ok = true;
        for (int i = 0; i < 1000 && ok; i++) {
            uint64_t got = 0;
            for (int b = 0; b < lens[i]; b++) got = (got << 1) | r.readBit();
            if (got != codes[i]) { ok = false; std::cout << "MISMATCH at " << i << "\n"; }
        }
        std::cout << (ok ? "round trip OK\n" : "round trip FAILED\n");
    }
}