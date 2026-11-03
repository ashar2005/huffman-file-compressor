#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
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

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cout << "Usage:\n"
                  << "  huffman compress <input> <output>\n"
                  << "  huffman decompress <input> <output>\n";
        return 1;
    }
    try {
        std::string mode = argv[1];
        if (mode == "compress") {
            auto data = readFile(argv[2]);
            auto packed = compressBytes(data);
            writeFile(argv[3], packed);
            std::cout << "Original:   " << data.size()   << " bytes\n";
            std::cout << "Compressed: " << packed.size() << " bytes\n";
            if (!data.empty())
                std::cout << "Ratio:      " << (100.0 * packed.size() / data.size()) << " %\n";
        } else if (mode == "decompress") {
            auto packed = readFile(argv[2]);
            auto data = decompressBytes(packed);
            writeFile(argv[3], data);
            std::cout << "Restored " << data.size() << " bytes\n";
        } else {
            std::cout << "Unknown mode: " << mode << "\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}