#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <iomanip>

std::vector<uint8_t> readFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);          // binary is a must on Windows
    if (!in) throw std::runtime_error("Cannot open " + path);
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(in), {});
}

int main(int argc, char* argv[]) {
    if (argc < 2) { std::cout << "Usage: huffman <file>\n"; return 1; }

    auto data = readFile(argv[1]);
    uint64_t freq[256] = {0};
    for (uint8_t b : data) freq[b]++;

    std::cout << "Size: " << data.size() << " bytes\n";
    for (int i = 0; i < 256; i++)
        if (freq[i]) std::cout << i << " ('" << (char)(i >= 32 && i < 127 ? i : '.')
                               << "') : " << freq[i] << "\n";
}