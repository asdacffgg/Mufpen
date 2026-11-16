#pragma once

#include "Export.h"

#include <QAbstractItemModel>
#include <QFileInfo>
#include <QHash>
#include <QMetaObject>
#include <QSet>
#include <QStringList>
#include <QVector>

#include <memory>
#include <vector>

class QFileSystemModel;

namespace mufpen {

// Aggregates one QFileSystemModel per opened directory under a single virtual
// root, so a tree view can show several independent workspace folders at once.
// It deliberately exposes the small QFileSystemModel-shaped surface that
// FileTreeModel (the sort/filter proxy) relies on: index(path), filePath,
// fileName, fileInfo, isDir and isDirectoryLoaded.
//
// Items are identified by FILE PATH, not by QModelIndex internalId: QFileSystemModel
// recreates its node objects during async scans and may reuse internalIds, which
// would otherwise silently remap an existing view/persistent index to another file.
// Handles (rootId + path) are owned here and stay alive for the model's lifetime
// (or until a full reset), so QPersistentModelIndex / QTreeView expansion state
// remains valid across incremental add/remove of other roots.
class MUFPEN_UI_EXPORT MultiRootSourceModel final : public QAbstractItemModel {
  Q_OBJECT
public:
  explicit MultiRootSourceModel(QObject* parent = nullptr);
  ~MultiRootSourceModel() override;

  // Returns the new root id (stable for the lifetime of the root), or -1 when
  // the path is invalid or already open (compared via WorkspaceFileIndex::pathKey).
  int addRoot(const QString& path);
  bool removeRoot(const QString& path);
  void setRoots(const QStringList& paths);
  QStringList roots() const;
  int rootCount() const { return roots_.size(); }
  bool isEmpty() const { return roots_.isEmpty(); }

  int rootForPath(const QString& path) const;
  bool contains(const QString& path) const { return rootForPath(path) >= 0; }
  QModelIndex rootIndex(int rootId) const;
  // Underlying filesystem model for a root (used by refresh diagnostics/tests).
  QFileSystemModel* sourceForPath(const QString& path) const;

  // Resolve an absolute filesystem path to an aggregate index (any column 0).
  QModelIndex index(const QString& path) const;
  QString filePath(const QModelIndex& index) const;
  QString fileName(const QModelIndex& index) const;
  QFileInfo fileInfo(const QModelIndex& index) const;
  bool isDir(const QModelIndex& index) const;
  bool isDirectoryLoaded(const QModelIndex& index) const;

  // Recreate all underlying filesystem models, dropping stale watches/caches
  // while preserving the current root list.
  void refresh();

  // Key used to cache stable view indices for a (root, path) pair.
  struct HandleKey {
    int rootId = -1;
    QString path;
    bool operator==(const HandleKey& other) const { return rootId == other.rootId && path == other.path; }
  };

  // QAbstractItemModel interface -------------------------------------------
  QModelIndex index(int row, int column, const QModelIndex& parent = {}) const override;
  QModelIndex parent(const QModelIndex& child) const override;
  int rowCount(const QModelIndex& parent = {}) const override;
  int columnCount(const QModelIndex& parent = {}) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  QMimeData* mimeData(const QModelIndexList& indexes) const override;
  bool canFetchMore(const QModelIndex& parent) const override;
  void fetchMore(const QModelIndex& parent) override;

signals:
  void directoryLoaded(const QString& path);
  void rootsChanged();

private:
  struct Root {
    int id = -1;
    QString path;
    QFileSystemModel* model = nullptr;
    QMetaObject::Connection loadedConn;
    QSet<QString> loadedPaths;
  };
  struct Handle {
    int rootId = -1;
    QString path;
  };

  Root makeRoot(const QString& path);
  void destroyRoots();
  void clearHandles();
  Root* rootById(int id) const;
  int rowOfRoot(int id) const;
  Root* rootForIndex(const QModelIndex& index) const;
  Handle* handleFor(const QModelIndex& index) const;
  QModelIndex makeIndex(int row, int column, int rootId, const QString& path) const;
  quintptr registerHandle(int rootId, const QString& path) const;
  QModelIndex sourceIndex(const Root* root, const QString& path) const;
  QModelIndex mapSourceIndex(int rootId, const QModelIndex& source) const;

  QVector<Root> roots_;
  int nextRootId_ = 1;
  mutable QHash<quintptr, Handle*> handles_;
  mutable QHash<HandleKey, quintptr> handleCache_;
  mutable std::vector<std::unique_ptr<Handle>> handleStore_;
  mutable quintptr nextHandleId_ = 1;
};

inline size_t qHash(const mufpen::MultiRootSourceModel::HandleKey& key, size_t seed = 0) noexcept {
  return qHashMulti(seed, key.rootId, key.path);
}

} // namespace mufpen
