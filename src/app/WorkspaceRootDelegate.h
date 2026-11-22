#pragma once

#include <QColor>
#include <QStyledItemDelegate>

namespace mufpen {

class FileTreeModel;

// Paints top-level workspace-root rows as a soft "section header": the normal
// folder row (chevron, icon, folder name, hover/selection) keeps its standard
// look, and the row is wrapped in a translucent accent band with a hairline
// separator above it. A custom workspace label appears as a small pill on the
// right, only when the user renamed the workspace (otherwise the folder name is
// already the title, and repeating it would just be noise).
//
// Deliberately keeps the standard row height: a taller root row would require
// QTreeView::setUniformRowHeights(false), which makes expanding a large folder
// an order of magnitude slower (measured: ~20s vs ~2s for 2000 entries).
class WorkspaceRootDelegate final : public QStyledItemDelegate {
  Q_OBJECT
public:
  explicit WorkspaceRootDelegate(FileTreeModel* model, QObject* parent = nullptr);

  // Theme colours the delegate needs for accents. Qt style sheets colour the
  // items without updating the palette, so the palette alone is not reliable.
  void setThemeColors(const QColor& text, const QColor& surface);

  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
  FileTreeModel* model_ = nullptr;
  QColor themeText_;
  QColor themeSurface_;
};

} // namespace mufpen
