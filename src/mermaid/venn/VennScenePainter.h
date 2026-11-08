#pragma once

#include "mermaid/MermaidPaintOptions.h"

class QPainter;

namespace mufpen::mermaid::venn {

struct VennScene;

void paintVennScene(const VennScene& scene, QPainter& painter, const MermaidPaintOptions& options = {});

} // namespace mufpen::mermaid::venn
