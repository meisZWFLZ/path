
#include "pathFileSystem.hpp"
#include "byteBuffer.hpp"
#include <iostream>
#include <istream>
#include <streambuf>
#include <string>

// copied from stack overflow
struct membuf : std::streambuf {
        membuf(char* begin, char* end) { this->setg(begin, begin, end); }
};

template <class T> T getIn(T& item, std::istream& in) {
    in.read(reinterpret_cast<char*>(&item), sizeof(T));
    return item;
}

template <class T> T getIn(T &item, std::istream& in, size_t size) {
    in.read(reinterpret_cast<char*>(&item), size);
    return item;
}

std::string getNTBS(size_t maxSize, std::istream &in) {
    std::string rtn = "";
    char now;
    for (size_t i = 0; i < maxSize; i++) {
        getIn(now, in);
        if (now == (char)0x00) break;
        rtn += now;
    }
    return rtn;
}

namespace lemlib {
namespace PathFileSystem {

using namespace lemlib;

bool decode(const uint8_t* fileBuffer, const size_t fileSize, PathFile& output) {
    try {
        uint8_t metadata[256];
        uint8_t metadataSize;
        uint16_t pathCount;
        uint32_t waypointCount;
        uint8_t flag;
        uint16_t ignored;

        membuf sbuf((char*)fileBuffer, (char*)(fileBuffer + fileSize));
        std::istream in(&sbuf);

        // ByteBuffer buf = ByteBuffer::wrap(fileSize, (char*)fileBuffer);

        // Start reading metadata
        getIn(metadataSize, in);
        getIn(metadata[0], in, metadataSize);
        // buf.get((char*)metadata, metadataSize);

        // End reading metadata

        // pathCount = buf.get<uint16_t>();
        getIn(pathCount, in);
        if (pathCount > 0) {
            for (int i = 0; i < pathCount; i++) {
                Path p;

                // p.name = buf.getNTBS();
                p.name = getNTBS(1024, in);

                // Start reading metadata
                getIn(metadataSize, in);
                // metadataSize = buf.get<uint8_t>();
                getIn(metadata[0], in, metadataSize);
                // buf.get((char*)metadata, metadataSize);

                // End reading metadata
                getIn(waypointCount, in);
                // waypointCount = buf.get<uint32_t>();

                for (int j = 0; j < waypointCount; j++) {
                    StaticWaypoint sw;
                    getIn(sw, in);
                    Waypoint w(sw);

                    if ((w.isHeadingAvailable = (flag & 0x01) != 0)) getIn(w.heading, in);
                    if ((w.isLookaheadAvailable = (flag & 0x02) != 0)) getIn(w.lookahead, in);
                    if ((flag & 0x04) != 0) getIn(ignored, in);
                    if ((flag & 0x08) != 0) getIn(ignored, in);
                    if ((flag & 0x10) != 0) getIn(ignored, in);
                    if ((flag & 0x20) != 0) getIn(ignored, in);
                    if ((flag & 0x40) != 0) getIn(ignored, in);
                    if ((flag & 0x80) != 0) getIn(ignored, in);

                    p.waypoints.push_back(w);
                }

                output.paths.push_back(p);
            }
        }

        return true;
    } catch (std::exception& e) { return false; }
}

bool encode(const PathFile& input, uint8_t* fileBuffer, size_t& fileSize) {
    try {
        uint8_t metadata[256];
        uint8_t metadataSize;
        uint16_t pathCount;
        uint32_t waypointCount;
        uint8_t flag;

        ByteBuffer buf = ByteBuffer::wrap(fileSize, (char*)fileBuffer);

        // Start writing metadata
        metadataSize = 0;
        buf.put(metadataSize);
        buf.put((char*)metadata, metadataSize);

        // End writing metadata

        pathCount = input.paths.size();
        buf.put(pathCount);

        for (int i = 0; i < pathCount; i++) {
            const Path& p = input.paths[i];

            buf.putNTBS(p.name);

            // Start writing metadata
            metadataSize = 0;
            buf.put(metadataSize);
            buf.put((char*)metadata, metadataSize);

            // End writing metadata

            waypointCount = p.waypoints.size();
            buf.put(waypointCount);

            for (int j = 0; j < waypointCount; j++) {
                const Waypoint& w = p.waypoints[j];

                flag = 0;
                if (w.isHeadingAvailable) flag |= 0x01;
                if (w.isLookaheadAvailable) flag |= 0x02;
                buf.put(flag);
                buf.put(w.x);
                buf.put(w.y);
                buf.put(w.speed);
                if (w.isHeadingAvailable) buf.put(w.heading);
                if (w.isLookaheadAvailable) buf.put(w.lookahead);
                if ((flag & 0x04) != 0) buf.put<uint16_t>(0);
                if ((flag & 0x08) != 0) buf.put<uint16_t>(0);
                if ((flag & 0x10) != 0) buf.put<uint16_t>(0);
                if ((flag & 0x20) != 0) buf.put<uint16_t>(0);
                if ((flag & 0x40) != 0) buf.put<uint16_t>(0);
                if ((flag & 0x80) != 0) buf.put<uint16_t>(0);
            }
        }

        fileSize = buf.position();

        return true;
    } catch (std::exception& e) { return false; }
}

Waypoint::Waypoint(StaticWaypoint sWaypt) : StaticWaypoint(sWaypt) {}

} // namespace PathFileSystem
} // namespace lemlib