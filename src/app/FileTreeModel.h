#pragma once

#include "Export.h"

#include <QCollator>
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

  // Replace stale filesystem caches; consumers restore only saved branches.
  void refresh();

signals:
  void directoryLoaded(QString path);
  void rootsChanged();

protected:
  bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
  bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
  void replaceSource();
  MultiRootSourceModel* files_ = nullptr;
  QCollator collator_;
  SortBy sortBy_ = SortBy::Name;
};

} // namespace mufpen
