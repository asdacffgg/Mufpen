#include "app/MultiRootSourceModel.h"

#include "app/WorkspaceFiles.h"
#include "io/MufpenMime.h"

#include <QDir>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QMimeData>
#include <QUrl>

namespace {

QString cleanPath(const QString& path) {
  return path.isEmpty() ? QString() : QDir::cleanPath(QFileInfo(path).absoluteFilePath());
}

// Syscall-free path equality for the hot index-creation path. Both strings are
// absolute paths produced by this model, so an exact/case-insensitive compare is
// enough; canonicalFilePath() would add two filesystem round-trips per created
// index, and QTreeView creates an index for every visible row.
bool samePath(const QString& a, const QString& b) {
  if (a.isEmpty() || b.isEmpty() || a.size() != b.size())
    return false;
  if (a == b)
    return true;
#ifdef Q_OS_WIN
  return a.compare(b, Qt::CaseInsensitive) == 0;
#else
  return false;
#endif
}

} // namespace

mufpen::MultiRootSourceModel::MultiRootSourceModel(QObject* parent) : QAbstractItemModel(parent) {}

mufpen::MultiRootSourceModel::~MultiRootSourceModel() {
  destroyRoots();
}

mufpen::MultiRootSourceModel::Root mufpen::MultiRootSourceModel::makeRoot(const QString& path) {
  Root root;
  root.id = nextRootId_++;
  root.path = cleanPath(path);
  root.model = new QFileSystemModel(this);
  root.model->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
  root.model->setRootPath(root.path);
  const int id = root.id;
  root.loadedConn = connect(root.model, &QFileSystemModel::directoryLoaded, this,
                            [this, id](const QString& directory) {
                              Root* r = rootById(id);
                              if (!r) {
                                return;
                              }
                              r->loadedPaths.insert(WorkspaceFileIndex::pathKey(directory));
                              emit directoryLoaded(directory);
                            });
  // QFileSystemModel populates directories asynchronously. Forward its change
  // signals with mapped parents, otherwise the sort/filter proxy above us never
  // learns that rows arrived and the tree stays stuck with the first batch.
  connect(root.model, &QAbstractItemModel::rowsAboutToBeInserted, this,
          [this, id](const QModelIndex& parent, int first, int last) {
            beginInsertRows(mapSourceIndex(id, parent), first, last);
          });
  connect(root.model, &QAbstractItemModel::rowsInserted, this, [this](const QModelIndex&, int, int) { endInsertRows(); });
  connect(root.model, &QAbstractItemModel::rowsAboutToBeRemoved, this,
          [this, id](const QModelIndex& parent, int first, int last) {
            beginRemoveRows(mapSourceIndex(id, parent), first, last);
          });
  connect(root.model, &QAbstractItemModel::rowsRemoved, this, [this](const QModelIndex&, int, int) { endRemoveRows(); });
  connect(root.model, &QAbstractItemModel::dataChanged, this,
          [this, id](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles) {
            emit dataChanged(mapSourceIndex(id, topLeft), mapSourceIndex(id, bottomRight), roles);
          });
  connect(root.model, &QAbstractItemModel::modelAboutToBeReset, this, [this] { beginResetModel(); });
  connect(root.model, &QAbstractItemModel::modelReset, this, [this] { endResetModel(); });
  return root;
}

void mufpen::MultiRootSourceModel::destroyRoots() {
  for (Root& root : roots_) {
    disconnect(root.model, nullptr, this, nullptr);
    delete root.model;
    root.model = nullptr;
  }
  roots_.clear();
}

void mufpen::MultiRootSourceModel::clearHandles() {
  handles_.clear();
  handleCache_.clear();
  handleStore_.clear();
  nextHandleId_ = 1;
}

int mufpen::MultiRootSourceModel::addRoot(const QString& path) {
  const QString cleaned = cleanPath(path);
  if (cleaned.isEmpty() || !QFileInfo(cleaned).isDir()) {
    return -1;
  }
  const QString key = WorkspaceFileIndex::pathKey(cleaned);
  for (const Root& root : roots_) {
    if (WorkspaceFileIndex::pathKey(root.path) == key) {
      return -1;
    }
  }
  const int row = roots_.size();
  beginInsertRows({}, row, row);
  roots_.append(makeRoot(cleaned));
  endInsertRows();
  emit rootsChanged();
  return roots_.last().id;
}

bool mufpen::MultiRootSourceModel::removeRoot(const QString& path) {
  const QString key = WorkspaceFileIndex::pathKey(path);
  int row = -1;
  for (int i = 0; i < roots_.size(); ++i) {
    if (WorkspaceFileIndex::pathKey(roots_.at(i).path) == key) {
      row = i;
      break;
    }
  }
  if (row < 0) {
    return false;
  }
  beginRemoveRows({}, row, row);
  Root root = roots_.takeAt(row);
  disconnect(root.model, nullptr, this, nullptr);
  delete root.model;
  endRemoveRows();
  emit rootsChanged();
  return true;
}

void mufpen::MultiRootSourceModel::setRoots(const QStringList& paths) {
  beginResetModel();
  clearHandles();
  destroyRoots();
  QStringList cleaned;
  for (const QString& path : paths) {
    const QString value = cleanPath(path);
    if (value.isEmpty() || !QFileInfo(value).isDir()) {
      continue;
    }
    const QString key = WorkspaceFileIndex::pathKey(value);
    bool duplicate = false;
    for (const QString& existing : cleaned) {
      if (WorkspaceFileIndex::pathKey(existing) == key) {
        duplicate = true;
        break;
      }
    }
    if (!duplicate) {
      cleaned.append(value);
    }
  }
  for (const QString& path : cleaned) {
    roots_.append(makeRoot(path));
  }
  endResetModel();
  emit rootsChanged();
}

QStringList mufpen::MultiRootSourceModel::roots() const {
  QStringList result;
  for (const Root& root : roots_) {
    result.append(root.path);
  }
  return result;
}

int mufpen::MultiRootSourceModel::rootForPath(const QString& path) const {
  if (path.isEmpty()) {
    return -1;
  }
  // Prefer the deepest matching root when opened directories overlap.
  int best = -1;
  int bestLength = -1;
  for (const Root& root : roots_) {
    if (!WorkspaceFileIndex::contains(root.path, path)) {
      continue;
    }
    const int length = WorkspaceFileIndex::pathKey(root.path).size();
    if (length > bestLength) {
      bestLength = length;
      best = root.id;
    }
  }
  return best;
}

QFileSystemModel* mufpen::MultiRootSourceModel::sourceForPath(const QString& path) const {
  const Root* root = rootById(rootForPath(path));
  return root ? root->model : nullptr;
}

QModelIndex mufpen::MultiRootSourceModel::rootIndex(int rootId) const {
  const int row = rowOfRoot(rootId);
  if (row < 0) {
    return {};
  }
  return makeIndex(row, 0, rootId, roots_.at(row).path);
}

bool mufpen::MultiRootSourceModel::isRootRow(const QModelIndex& index) const {
  const Handle* handle = handleFor(index);
  if (!handle) {
    return false;
  }
  const Root* root = rootById(handle->rootId);
  if (!root) {
    return false;
  }
  // Both paths are already cleaned absolute paths from the same filesystem
  // model, so a plain string compare is enough (and syscall-free).
  return samePath(handle->path, root->path);
}

QModelIndex mufpen::MultiRootSourceModel::index(const QString& path) const {
  const int rootId = rootForPath(path);
  const Root* root = rootById(rootId);
  if (!root) {
    return {};
  }
  const QString cleaned = cleanPath(path);
  if (!sourceIndex(root, cleaned).isValid()) {
    return {};
  }
  return makeIndex(0, 0, rootId, cleaned);
}

QModelIndex mufpen::MultiRootSourceModel::sourceIndex(const Root* root, const QString& path) const {
  return root ? root->model->index(path) : QModelIndex();
}

QString mufpen::MultiRootSourceModel::filePath(const QModelIndex& index) const {
  const Handle* handle = handleFor(index);
  return handle ? handle->path : QString();
}

QString mufpen::MultiRootSourceModel::fileName(const QModelIndex& index) const {
  const Root* root = rootForIndex(index);
  const Handle* handle = handleFor(index);
  if (!root || !handle) {
    return {};
  }
  return root->model->fileName(sourceIndex(root, handle->path));
}

QFileInfo mufpen::MultiRootSourceModel::fileInfo(const QModelIndex& index) const {
  const Handle* handle = handleFor(index);
  return handle ? QFileInfo(handle->path) : QFileInfo();
}

bool mufpen::MultiRootSourceModel::isDir(const QModelIndex& index) const {
  const Handle* handle = handleFor(index);
  return handle ? QFileInfo(handle->path).isDir() : false;
}

bool mufpen::MultiRootSourceModel::isDirectoryLoaded(const QModelIndex& index) const {
  const Handle* handle = handleFor(index);
  const Root* root = rootForIndex(index);
  return handle && root && root->loadedPaths.contains(WorkspaceFileIndex::pathKey(handle->path));
}

void mufpen::MultiRootSourceModel::refresh() {
  const QStringList paths = roots();
  beginResetModel();
  clearHandles();
  destroyRoots();
  for (const QString& path : paths) {
    roots_.append(makeRoot(path));
  }
  endResetModel();
  emit rootsChanged();
}

QModelIndex mufpen::MultiRootSourceModel::index(int row, int column, const QModelIndex& parent) const {
  if (row < 0 || column < 0) {
    return {};
  }
  if (!parent.isValid()) {
    if (row >= roots_.size() || column != 0) {
      return {};
    }
    return makeIndex(row, column, roots_.at(row).id, roots_.at(row).path);
  }
  const Root* root = rootForIndex(parent);
  const Handle* parentHandle = handleFor(parent);
  if (!root || !parentHandle) {
    return {};
  }
  const QModelIndex source = root->model->index(row, column, sourceIndex(root, parentHandle->path));
  if (!source.isValid()) {
    return {};
  }
  return makeIndex(row, column, root->id, root->model->filePath(source));
}

QModelIndex mufpen::MultiRootSourceModel::parent(const QModelIndex& child) const {
  const Root* root = rootForIndex(child);
  const Handle* handle = handleFor(child);
  if (!root || !handle) {
    return {};
  }
  if (samePath(handle->path, root->path)) {
    return {}; // root folder item -> virtual root
  }
  const QModelIndex sourceParent = sourceIndex(root, handle->path).parent();
  if (!sourceParent.isValid()) {
    return {};
  }
  return makeIndex(sourceParent.row(), sourceParent.column(), root->id,
                   root->model->filePath(sourceParent));
}

int mufpen::MultiRootSourceModel::rowCount(const QModelIndex& parent) const {
  if (!parent.isValid()) {
    return roots_.size();
  }
  const Root* root = rootForIndex(parent);
  const Handle* handle = handleFor(parent);
  return root && handle ? root->model->rowCount(sourceIndex(root, handle->path)) : 0;
}

int mufpen::MultiRootSourceModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  // Only the name column is exposed; FileTreeModel/SidebarWidget never use
  // QFileSystemModel's extra size/type/date columns.
  return 1;
}

QVariant mufpen::MultiRootSourceModel::data(const QModelIndex& index, int role) const {
  const Root* root = rootForIndex(index);
  const Handle* handle = handleFor(index);
  return root && handle ? root->model->data(sourceIndex(root, handle->path), role) : QVariant();
}

QVariant mufpen::MultiRootSourceModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (section == 0 && role == Qt::DisplayRole) {
    return tr("Name");
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags mufpen::MultiRootSourceModel::flags(const QModelIndex& index) const {
  const Root* root = rootForIndex(index);
  const Handle* handle = handleFor(index);
  return root && handle ? root->model->flags(sourceIndex(root, handle->path)) : Qt::NoItemFlags;
}

QMimeData* mufpen::MultiRootSourceModel::mimeData(const QModelIndexList& indexes) const {
  QList<QUrl> urls;
  for (const QModelIndex& index : indexes) {
    const QString path = filePath(index);
    if (!path.isEmpty()) {
      urls.append(QUrl::fromLocalFile(path));
    }
  }
  auto* data = new QMimeData;
  data->setUrls(urls);
  data->setData(mufpen::kMufpenFileTreeDragMime, {});
  return data;
}

bool mufpen::MultiRootSourceModel::canFetchMore(const QModelIndex& parent) const {
  const Root* root = rootForIndex(parent);
  const Handle* handle = handleFor(parent);
  return root && handle && root->model->canFetchMore(sourceIndex(root, handle->path));
}

void mufpen::MultiRootSourceModel::fetchMore(const QModelIndex& parent) {
  const Root* root = rootForIndex(parent);
  const Handle* handle = handleFor(parent);
  if (root && handle) {
    root->model->fetchMore(sourceIndex(root, handle->path));
  }
}

mufpen::MultiRootSourceModel::Root* mufpen::MultiRootSourceModel::rootById(int id) const {
  for (const Root& root : roots_) {
    if (root.id == id) {
      return const_cast<Root*>(&root);
    }
  }
  return nullptr;
}

int mufpen::MultiRootSourceModel::rowOfRoot(int id) const {
  for (int i = 0; i < roots_.size(); ++i) {
    if (roots_.at(i).id == id) {
      return i;
    }
  }
  return -1;
}

mufpen::MultiRootSourceModel::Root* mufpen::MultiRootSourceModel::rootForIndex(const QModelIndex& index) const {
  if (!index.isValid() || index.model() != this) {
    return nullptr;
  }
  const Handle* handle = handleFor(index);
  return handle ? rootById(handle->rootId) : nullptr;
}

QModelIndex mufpen::MultiRootSourceModel::mapSourceIndex(int rootId, const QModelIndex& source) const {
  if (!source.isValid()) {
    return {}; // virtual root
  }
  const Root* root = rootById(rootId);
  if (!root) {
    return {};
  }
  return makeIndex(source.row(), source.column(), rootId, root->model->filePath(source));
}

mufpen::MultiRootSourceModel::Handle* mufpen::MultiRootSourceModel::handleFor(const QModelIndex& index) const {
  if (!index.isValid() || index.model() != this) {
    return nullptr;
  }
  return handles_.value(index.internalId(), nullptr);
}

QModelIndex mufpen::MultiRootSourceModel::makeIndex(int row, int column, int rootId, const QString& path) const {
  // A root folder item is row 0 inside its own QFileSystemModel, but sits at
  // `rowOfRoot(rootId)` under this model's virtual root.
  const Root* root = rootById(rootId);
  if (root && samePath(path, root->path)) {
    row = rowOfRoot(rootId);
  }
  return createIndex(row, column, registerHandle(rootId, path));
}

quintptr mufpen::MultiRootSourceModel::registerHandle(int rootId, const QString& path) const {
  const HandleKey key{rootId, path};
  if (const auto it = handleCache_.constFind(key); it != handleCache_.cend()) {
    return it.value();
  }
  const quintptr id = nextHandleId_++;
  auto handle = std::make_unique<Handle>();
  handle->rootId = rootId;
  handle->path = path;
  handles_.insert(id, handle.get());
  handleCache_.insert(key, id);
  handleStore_.push_back(std::move(handle));
  return id;
}
