#pragma once
#include "mermaid/flowchart/SwimlaneLayout.h"

namespace mufpen::mermaid::flowchart {
void applyLineJumps(FlowLayoutResult& result, const FlowchartData& data, const SwimlaneLayoutOptions& options);
}
