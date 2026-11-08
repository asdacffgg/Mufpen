#pragma once

class QPainter;

namespace mufpen::mermaid {
struct MermaidPaintOptions;
}

namespace mufpen::mermaid::wardley {

struct WardleyScene;

void paintWardleyScene(const WardleyScene& scene, QPainter& painter, const MermaidPaintOptions& options);

} // namespace mufpen::mermaid::wardley
