#pragma once

#include "Export.h"

#include <QCollator>
#include <QColor>
#include <QHash>
#include <QPersistentModelIndex>
#include <QSet>
#include <QSortFilterProxyModel>

namespace mufpen {

class MultiRootSourceModel;

// Path-based facade over the asynchronous filesystem models. View indexes always
// belong to this proxy, including drag, rename and saved expansion state.
// Several workspace roots are aggregated under one virtual root by the source
// model, so one tree view can display multiple independent folders at once.
class MUFPEN_UI_EXPORT FileTreeModel final : public QSortFilterProxyModel {
  Q_OBJECT
public:
  enum class SortBy { Name, Modified, Created, Type, Size };
  Q_ENUM(SortBy)

  // Custom model roles consumed by WorkspaceRootDelegate to draw the section header.
  enum WorkspaceRole {
    WorkspaceLabelRole = Qt::UserRole + 100,
    WorkspaceColorRole,      // QColor resolved from the stored accent key
    WorkspaceIsRootRole,
    WorkspaceHasLabelRole,   // true when the label was customised by the user
  };

  explicit FileTreeModel(QObject* parent = nullptr);
  using QSortFilterProxyModel::index;
  QModelIndex index(const QString& path) const;
  QString filePath(const QModelIndex& index) const;
  QString fileName(const QModelIndex& index) const;
  bool isDir(const QModelIndex& index) const;
  bool isDirectoryLoaded(const QModelIndex& index) const;
  void setSortBy(SortBy field, Qt::SortOrder order);
  SortBy sortBy() const { return sortBy_; }

  // Multiple workspace roots ------------------------------------------------
  int addRoot(const QString& path);
  bool removeRoot(const QString& path);
  void setRoots(const QStringList& paths);
  QStringList roots() const;
  int rootCount() const;
  bool isEmpty() const;
  // Id of the root containing `path`, or -1. Useful for per-root UI state.
  int rootForPath(const QString& path) const;
  QModelIndex rootIndex(int rootId) const;
  // True when `index` is one of the level-1 workspace-root rows (the folder
  // itself, not a child), used by the delegate to draw a section header.
  bool isWorkspaceRoot(const QModelIndex& index) const;

  // Per-workspace display label + accent colour, keyed by path key. Persisted
  // via QSettings. An empty label means "use the folder name"; the colour is a
  // WorkspaceStyle key (see WorkspaceStyle.h) resolved to a QColor by the delegate.
  // Keys are cheap normalised paths (see workspaceKey()) so these getters stay
  // safe to call from paint/sizeHint (no canonicalFilePath()).
  QString workspaceLabel(const QString& path) const;
  QString workspaceColorKey(const QString& path) const;
  void setWorkspaceLabel(const QString& path, const QString& label);
  void setWorkspaceColorKey(const QString& path, const QString& key);

  // Replace stale filesystem caches; consumers restore only saved branches.
  void refresh();

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

signals:
  void directoryLoaded(QString path);
  void rootsChanged();

protected:
  bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
  bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
  void replaceSource();
  void loadWorkspaceStyles();
  void saveWorkspaceStyles();
  MultiRootSourceModel* files_ = nullptr;
  QCollator collator_;
  SortBy sortBy_ = SortBy::Name;
  QHash<QString, QString> workspaceLabels_;    // path key -> custom label
  QHash<QString, QString> workspaceColors_;    // path key -> accent key
};

} // namespace mufpen
