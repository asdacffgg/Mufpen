#pragma once

class QPainter;

namespace mufpen::mermaid {
struct MermaidPaintOptions;
}

namespace mufpen::mermaid::packet {

struct PacketScene;

void paintPacketScene(const PacketScene& scene, QPainter& painter, const MermaidPaintOptions& options);

} // namespace mufpen::mermaid::packet
