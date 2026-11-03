#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <iomanip>
#include <stdexcept>
#include "Codec.h"

static std::vector<uint8_t> readFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("Cannot open " + path);
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(in), {});
}

static void writeFile(const std::string& path, const std::vector<uint8_t>& d) {
    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("Cannot write " + path);
    out.write((const char*)d.data(), (std::streamsize)d.size());
}

static void printStats(size_t orig, size_t comp) {
    std::cout << "Original size:   " << orig << " bytes\n";
    std::cout << "Compressed size: " << comp << " bytes\n";
    if (orig > 0) {
        double ratio = 100.0 * comp / orig;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Ratio:           " << ratio << " %\n";
        if (comp < orig)
            std::cout << "Space saved:     " << (100.0 - ratio) << " %\n";
        else
            std::cout << "Note: file got larger (already compressed or too small)\n";
    }
}

static void usage() {
    std::cout << "Usage:\n"
              << "  huffman compress   <input> <output>   (verifies the result automatically)\n"
              << "  huffman decompress <input> <output>\n"
              << "  huffman test       <input>            (compress + decompress in memory, report only)\n";
}

int main(int argc, char* argv[]) {
    if (argc < 3) { usage(); return 1; }
    try {
        std::string mode = argv[1];

        if (mode == "compress" && argc == 4) {
            auto data = readFile(argv[2]);
            auto packed = compressBytes(data);

            // round-trip verification before we trust the output
            if (decompressBytes(packed) != data)
                throw std::runtime_error("Verification FAILED: restored data differs from original");

            writeFile(argv[3], packed);
            printStats(data.size(), packed.size());
            std::cout << "Verified:        round trip OK\n";

        } else if (mode == "decompress" && argc == 4) {
            auto packed = readFile(argv[2]);
            auto data = decompressBytes(packed);
            writeFile(argv[3], data);
            std::cout << "Restored " << data.size() << " bytes\n";

        } else if (mode == "test" && argc == 3) {
            auto data = readFile(argv[2]);
            auto packed = compressBytes(data);
            bool ok = (decompressBytes(packed) == data);
            printStats(data.size(), packed.size());
            std::cout << "Round trip:      " << (ok ? "OK" : "FAILED") << "\n";
            return ok ? 0 : 1;

        } else {
            usage();
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}