#pragma once

#include "mermaid/MermaidPaintOptions.h"

class QPainter;

namespace mufpen::mermaid::architecture {

struct ArchitectureScene;
void paintArchitectureScene(const ArchitectureScene& scene, QPainter& painter, const MermaidPaintOptions& options = {});

} // namespace mufpen::mermaid::architecture
