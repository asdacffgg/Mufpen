#pragma once

// Native port of dagre-d3-es/src/dagre/order/ (C4 of the flowchart plan):
// compound-aware edge-crossing minimization. Sets node `order` via barycenter
// sweeps that respect subgraph hierarchy and border-node constraints.

#include "mermaid/dagre/DagreLabels.h"

namespace mufpen::mermaid::dagre {

void order(DagreGraph& g);

} // namespace mufpen::mermaid::dagre
