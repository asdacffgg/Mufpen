#pragma once

#include "mermaid/MermaidPaintOptions.h"

class QPainter;

namespace mufpen::mermaid::cynefin {

struct CynefinScene;

void paintCynefinScene(const CynefinScene& scene, QPainter& painter, const MermaidPaintOptions& options = {});

} // namespace mufpen::mermaid::cynefin
