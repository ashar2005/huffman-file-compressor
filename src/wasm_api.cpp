#include <emscripten/emscripten.h>
#include <vector>
#include <cstdint>
#include "Codec.h"

static std::vector<uint8_t> g_out;

extern "C" {

EMSCRIPTEN_KEEPALIVE
int hc_compress(const uint8_t* data, int len) {
    g_out = compressBytes(std::vector<uint8_t>(data, data + len));
    return (int)g_out.size();
}

EMSCRIPTEN_KEEPALIVE
int hc_decompress(const uint8_t* data, int len) {
    try {
        g_out = decompressBytes(std::vector<uint8_t>(data, data + len));
        return (int)g_out.size();
    } catch (...) {
        g_out.clear();
        return -1;
    }
}

EMSCRIPTEN_KEEPALIVE
const uint8_t* hc_result() {
    return g_out.data();
}

}