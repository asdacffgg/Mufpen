#pragma once

#include "mermaid/kanban/KanbanScene.h"

class QPainter;

namespace mufpen::mermaid::kanban {

void paintKanbanScene(const KanbanScene& scene, QPainter& painter, const MermaidPaintOptions& options);

} // namespace mufpen::mermaid::kanban
