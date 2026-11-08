#pragma once

#include "mermaid/MermaidScene.h"

class QPainter;

namespace mufpen::mermaid::ishikawa {

struct IshikawaScene;

void paintIshikawaScene(const IshikawaScene& scene, QPainter& painter, const MermaidPaintOptions& options);

} // namespace mufpen::mermaid::ishikawa
