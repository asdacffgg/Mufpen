#pragma once

#include "mermaid/MermaidPaintOptions.h"

class QPainter;

namespace mufpen::mermaid::info {

struct InfoScene;

void paintInfoScene(QPainter& painter, const InfoScene& scene, const MermaidPaintOptions& options = {});

} // namespace mufpen::mermaid::info
