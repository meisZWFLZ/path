
#include <cstdint>
#include <cstddef>

namespace lemlib {
namespace PathFile {

struct Waypoint {
        int16_t x;
        int16_t y;
        int16_t speed;
        uint16_t heading;
        int16_t lookahead;
        bool isHeadingAvailable : 1;
        bool isLookaheadAvailable : 1;
};

class Path {
    public:
};

class PathFile {
    public:
};

bool decode(const uint8_t* fileBuffer, const size_t fileSize, PathFile& output);

} // namespace PathFile
} // namespace lemlib
