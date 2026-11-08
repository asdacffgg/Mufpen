#pragma once

class QPainter;

namespace mufpen::mermaid {
struct MermaidPaintOptions;
}

namespace mufpen::mermaid::gantt {

struct GanttScene;

void paintGanttScene(const GanttScene& scene, QPainter& painter, const MermaidPaintOptions& options);

} // namespace mufpen::mermaid::gantt
