
#include "pathFileSystem.hpp"
#include "byteBuffer.hpp"

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

        ByteBuffer buf = ByteBuffer::wrap(fileSize, (char*)fileBuffer);

        // Start reading metadata
        metadataSize = buf.get<uint8_t>();
        buf.get((char*)metadata, metadataSize);

        // End reading metadata

        pathCount = buf.get<uint16_t>();

        for (int i = 0; i < pathCount; i++) {
            Path p;

            p.name = buf.getNTBS();

            // Start reading metadata
            metadataSize = buf.get<uint8_t>();
            buf.get((char*)metadata, metadataSize);

            // End reading metadata

            waypointCount = buf.get<uint32_t>();

            for (int j = 0; j < waypointCount; j++) {
                Waypoint w;

                flag = buf.get<uint8_t>();
                w.x = buf.get<int16_t>();
                w.y = buf.get<int16_t>();
                w.speed = buf.get<int16_t>();

                if (w.isHeadingAvailable = (flag & 0x01) != 0) w.heading = buf.get<uint16_t>();
                if (w.isLookaheadAvailable = (flag & 0x02) != 0) w.lookahead = buf.get<int16_t>();
                if ((flag & 0x04) != 0) buf.get<uint16_t>();
                if ((flag & 0x08) != 0) buf.get<uint16_t>();
                if ((flag & 0x10) != 0) buf.get<uint16_t>();
                if ((flag & 0x20) != 0) buf.get<uint16_t>();
                if ((flag & 0x40) != 0) buf.get<uint16_t>();
                if ((flag & 0x80) != 0) buf.get<uint16_t>();

                p.waypoints.push_back(w);
            }

            output.paths.push_back(p);
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

} // namespace PathFileSystem
} // namespace lemlib