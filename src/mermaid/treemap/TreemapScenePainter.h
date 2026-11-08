#pragma once

#include "mermaid/MermaidScene.h"

class QPainter;

namespace mufpen::mermaid::treemap {
struct TreemapScene;
void paintTreemapScene(const TreemapScene& scene, QPainter& painter, const MermaidPaintOptions& options);
} // namespace mufpen::mermaid::treemap
