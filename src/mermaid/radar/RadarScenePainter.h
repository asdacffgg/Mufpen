#pragma once

class QPainter;

namespace mufpen::mermaid {
struct MermaidPaintOptions;
}

namespace mufpen::mermaid::radar {

struct RadarScene;

void paintRadarScene(const RadarScene& scene, QPainter& painter, const MermaidPaintOptions& options);

} // namespace mufpen::mermaid::radar
