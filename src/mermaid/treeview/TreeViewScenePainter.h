#pragma once

class QPainter;

namespace mufpen::mermaid {
struct MermaidPaintOptions;
}

namespace mufpen::mermaid::treeview {

struct TreeViewScene;

void paintTreeViewScene(QPainter& painter, const TreeViewScene& scene, const MermaidPaintOptions& options);

} // namespace mufpen::mermaid::treeview
