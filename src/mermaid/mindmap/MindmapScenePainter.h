#pragma once

#include "mermaid/MermaidPaintOptions.h"

class QPainter;

namespace mufpen::mermaid::mindmap {
struct MindmapScene;
void paintMindmapScene(const MindmapScene& scene, QPainter& painter, const MermaidPaintOptions& options = {});
} // namespace mufpen::mermaid::mindmap
