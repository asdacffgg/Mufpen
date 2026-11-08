#pragma once

#include "mermaid/MermaidPaintOptions.h"

class QPainter;

namespace mufpen::mermaid::railroad {

struct RailroadScene;

void paintRailroadScene(const RailroadScene& scene, QPainter& painter, const MermaidPaintOptions& options = {});

} // namespace mufpen::mermaid::railroad
