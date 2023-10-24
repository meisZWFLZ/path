
#include "pathFileSystem.hpp"

namespace lemlib {
namespace PathFile {

bool decode(const uint8_t* fileBuffer, const size_t fileSize, PathFile& output) {
    try {
        size_t i = 0;

        // Read uint16_t in little endian
        uint16_t numWaypoints = fileBuffer[i++];

        return true;
    } catch (...) { return false; }
}

} // namespace PathFile
} // namespace lemlib