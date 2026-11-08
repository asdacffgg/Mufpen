#pragma once

// Painter for the requirementDiagram family. Renders requirementBox nodes
// (rounded rect + <<Type>> + bold name + divider + body rows), relationship
// edges (solid/dashed + 2 markers), and centered edge labels. Mirrors the
// classdiagram::classScenePainter / er::ErScenePainter idiom.

class QPainter;

namespace mufpen::mermaid {
struct MermaidPaintOptions;
}

namespace mufpen::mermaid::requirement {

struct RequirementScene;

void paintRequirementScene(const RequirementScene& scene, QPainter& painter, const MermaidPaintOptions& options);

} // namespace mufpen::mermaid::requirement
