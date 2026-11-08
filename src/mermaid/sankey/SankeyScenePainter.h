#pragma once

#include "mermaid/MermaidPaintOptions.h"

class QPainter;

namespace mufpen::mermaid::sankey {
struct SankeyScene;
void paintSankeyScene(const SankeyScene& scene, QPainter& painter, const MermaidPaintOptions& options = {});
} // namespace mufpen::mermaid::sankey
