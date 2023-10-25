
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>

namespace lemlib {
namespace PathFileSystem {

struct StaticWaypoint {
        uint8_t flags;
        int16_t x; // Signed Integer, 0.5mm/bit, range: -16384mm ~ +16383.5mm
        int16_t y; // Signed Integer, 0.5mm/bit, range: -16384mm ~ +16383.5mm
        int16_t speed; // Signed Integer, mm/s/bit, range: -32768mm/s ~ +32767mm/s
};
struct Waypoint : StaticWaypoint {
        uint16_t heading; // Unsigned Integer , 0.0001rad/bit, range: 0rad ~ 6.2832rad
        int16_t lookahead; // Signed Integer, 0.5mm/bit, range: -16384mm ~ +16383.5mm
        bool isHeadingAvailable : 1;
        bool isLookaheadAvailable : 1;
        
        Waypoint(StaticWaypoint sWaypt);
};

class Path {
    public:
        std::string name;
        std::vector<Waypoint> waypoints;

        Path() = default;
};

class PathFile {
    public:
        std::vector<Path> paths;

        PathFile() = default;
};

bool decode(const uint8_t* fileBuffer, const size_t fileSize, PathFile& output);

bool encode(const PathFile& input, uint8_t* fileBuffer, size_t& fileSize);

} // namespace PathFileSystem
} // namespace lemlib
