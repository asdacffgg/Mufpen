#pragma once

#include "mermaid/MermaidPaintOptions.h"

class QPainter;

namespace mufpen::mermaid::eventmodeling {

struct EventModelingScene;

void paintEventModelingScene(const EventModelingScene& scene, QPainter& painter, const MermaidPaintOptions& options);

} // namespace mufpen::mermaid::eventmodeling
