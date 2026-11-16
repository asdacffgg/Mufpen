#include "app/FileTreeModel.h"

#include "app/MultiRootSourceModel.h"
#include "app/WorkspaceFiles.h"
#include "io/DocumentFileType.h"

#include <QDateTime>
#include <QDir>
#include <QLocale>

namespace {

template <typename T> int compare(const T& a, const T& b) {
  return a < b ? -1 : b < a ? 1 : 0;
}

} // namespace

mufpen::FileTreeModel::FileTreeModel(QObject* parent) : QSortFilterProxyModel(parent) {
  // The C-locale collator ignores numericMode, even with the ICU backend.
  if (collator_.locale().language() == QLocale::C)
    collator_.setLocale(QLocale(QLocale::English));
  collator_.setNumericMode(true);
  collator_.setCaseSensitivity(Qt::CaseInsensitive);
  setDynamicSortFilter(true);
  replaceSource();
  sort(0, Qt::AscendingOrder);
}

void mufpen::FileTreeModel::replaceSource() {
  auto* previous = files_;
  files_ = new MultiRootSourceModel(this);
  connect(files_, &MultiRootSourceModel::directoryLoaded, this, &FileTreeModel::directoryLoaded);
  connect(files_, &MultiRootSourceModel::rootsChanged, this, &FileTreeModel::rootsChanged);
  setSourceModel(files_);
  if (previous) {
    disconnect(previous, nullptr, this, nullptr);
    previous->deleteLater();
  }
}

QModelIndex mufpen::FileTreeModel::index(const QString& path) const {
  if (path.isEmpty()) {
    return {};
  }
  // Walk the visible proxy tree: mapFromSource() guesses the proxy row from
  // the unsorted source row for source indices the proxy has not seen, which
  // points at a different file once a non-default sort is active.
  const QString key = WorkspaceFileIndex::pathKey(path);
  const auto find = [this, &key](const QModelIndex& parent, const auto& self) -> QModelIndex {
    for (int row = 0; row < rowCount(parent); ++row) {
      const QModelIndex child = QSortFilterProxyModel::index(row, 0, parent);
      if (!child.isValid())
        continue;
      if (WorkspaceFileIndex::pathKey(filePath(child)) == key)
        return child;
      if (isDir(child)) {
        const QModelIndex found = self(child, self);
        if (found.isValid())
          return found;
      }
    }
    return {};
  };
  const QModelIndex visible = find({}, find);
  if (visible.isValid())
    return visible;
  const QModelIndex source = files_->index(path);
  if (!source.isValid())
    return {};
  const QString resolved = files_->filePath(source);
  if (!files_->isDir(source) && !isMarkdownDocumentPath(resolved) && !isImageDocumentPath(resolved))
    return {};
  return mapFromSource(source);
}

QString mufpen::FileTreeModel::filePath(const QModelIndex& index) const {
  return files_->filePath(mapToSource(index));
}

QString mufpen::FileTreeModel::fileName(const QModelIndex& index) const {
  return files_->fileName(mapToSource(index));
}

bool mufpen::FileTreeModel::isDir(const QModelIndex& index) const {
  return files_->isDir(mapToSource(index));
}

bool mufpen::FileTreeModel::isDirectoryLoaded(const QModelIndex& index) const {
  return index.isValid() && files_->isDirectoryLoaded(mapToSource(index));
}

void mufpen::FileTreeModel::setSortBy(SortBy field, Qt::SortOrder order) {
  sortBy_ = field;
  // The aggregate source can receive rows asynchronously; force a clean
  // rebuild of the proxy's sorted mapping rather than a no-op sort().
  invalidate();
  sort(0, order);
}

int mufpen::FileTreeModel::addRoot(const QString& path) {
  return files_->addRoot(path);
}

bool mufpen::FileTreeModel::removeRoot(const QString& path) {
  return files_->removeRoot(path);
}

void mufpen::FileTreeModel::setRoots(const QStringList& paths) {
  files_->setRoots(paths);
}

QStringList mufpen::FileTreeModel::roots() const {
  return files_->roots();
}

int mufpen::FileTreeModel::rootCount() const {
  return files_->rootCount();
}

bool mufpen::FileTreeModel::isEmpty() const {
  return files_->isEmpty();
}

int mufpen::FileTreeModel::rootForPath(const QString& path) const {
  return files_->rootForPath(path);
}

QModelIndex mufpen::FileTreeModel::rootIndex(int rootId) const {
  return mapFromSource(files_->rootIndex(rootId));
}

void mufpen::FileTreeModel::refresh() {
  files_->refresh();
}

bool mufpen::FileTreeModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const {
  const QModelIndex index = files_->index(sourceRow, 0, sourceParent);
  if (!index.isValid())
    return false;
  if (files_->isDir(index))
    return true;
  const QString path = files_->filePath(index);
  // MarkText-compatible workspace tree: Markdown documents and image assets.
  // Images are visible for reference but are not openable until preview lands.
  return isMarkdownDocumentPath(path) || isImageDocumentPath(path);
}

bool mufpen::FileTreeModel::lessThan(const QModelIndex& left, const QModelIndex& right) const {
  const auto a = files_->fileInfo(left), b = files_->fileInfo(right);
  // QSortFilterProxyModel reverses the comparison in descending order. Keep
  // directories ahead of files in either direction, on every platform.
  if (a.isDir() != b.isDir())
    return sortOrder() == Qt::AscendingOrder ? a.isDir() : b.isDir();
  int result = 0;
  switch (sortBy_) {
  case SortBy::Name:
    break;
  case SortBy::Modified:
    result = compare(a.lastModified(), b.lastModified());
    break;
  case SortBy::Created: {
    const auto createdA = a.birthTime(), createdB = b.birthTime();
    result =
        compare(createdA.isValid() ? createdA : a.lastModified(), createdB.isValid() ? createdB : b.lastModified());
    break;
  }
  case SortBy::Type:
    result = collator_.compare(a.suffix(), b.suffix());
    break;
  case SortBy::Size:
    result = compare(a.size(), b.size());
    break;
  }
  if (result == 0)
    result = collator_.compare(a.fileName(), b.fileName());
  if (result == 0)
    result = QString::compare(a.fileName(), b.fileName());
  return result < 0;
}
