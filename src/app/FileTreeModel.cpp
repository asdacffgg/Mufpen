#include "app/FileTreeModel.h"

#include "app/MultiRootSourceModel.h"
#include "app/WorkspaceFiles.h"
#include "app/WorkspaceStyle.h"
#include "io/DocumentFileType.h"

#include <QDateTime>
#include <QDir>
#include <QLocale>
#include <QSettings>

namespace {

template <typename T> int compare(const T& a, const T& b) {
  return a < b ? -1 : b < a ? 1 : 0;
}

// Normalises a workspace path for label/colour lookups. This runs on every
// paint for the section header, so it must stay pure string work: unlike
// WorkspaceFileIndex::pathKey() it never calls canonicalFilePath(), which costs
// a filesystem round-trip per call.
QString workspaceKey(const QString& path) {
  if (path.isEmpty()) {
    return {};
  }
  QString key = QDir::cleanPath(path);
#ifdef Q_OS_WIN
  key = key.toCaseFolded();
#endif
  return key;
}

} // namespace

mufpen::FileTreeModel::FileTreeModel(QObject* parent) : QSortFilterProxyModel(parent) {
  // The C-locale collator ignores numericMode, even with the ICU backend.
  if (collator_.locale().language() == QLocale::C)
    collator_.setLocale(QLocale(QLocale::English));
  collator_.setNumericMode(true);
  collator_.setCaseSensitivity(Qt::CaseInsensitive);
  setDynamicSortFilter(true);
  loadWorkspaceStyles();
  replaceSource();
  sort(0, Qt::AscendingOrder);
}

void mufpen::FileTreeModel::loadWorkspaceStyles() {
  const QSettings settings;
  // Older settings could be keyed by canonical paths; normalise on load so the
  // in-memory lookups (and future saves) use the same cheap key.
  const QVariantMap labels = settings.value(QStringLiteral("files/workspaceLabels")).toMap();
  for (auto it = labels.cbegin(); it != labels.cend(); ++it) {
    const QString label = it.value().toString();
    if (!label.isEmpty())
      workspaceLabels_.insert(workspaceKey(it.key()), label);
  }
  const QVariantMap colors = settings.value(QStringLiteral("files/workspaceColors")).toMap();
  for (auto it = colors.cbegin(); it != colors.cend(); ++it) {
    const QString key = it.value().toString();
    if (workspace_style::isValidKey(key))
      workspaceColors_.insert(workspaceKey(it.key()), key);
  }
}

void mufpen::FileTreeModel::saveWorkspaceStyles() {
  QSettings settings;
  QVariantMap labels;
  for (auto it = workspaceLabels_.cbegin(); it != workspaceLabels_.cend(); ++it)
    labels.insert(it.key(), it.value());
  settings.setValue(QStringLiteral("files/workspaceLabels"), labels);
  QVariantMap colors;
  for (auto it = workspaceColors_.cbegin(); it != workspaceColors_.cend(); ++it)
    colors.insert(it.key(), it.value());
  settings.setValue(QStringLiteral("files/workspaceColors"), colors);
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

bool mufpen::FileTreeModel::isWorkspaceRoot(const QModelIndex& index) const {
  // The root folder rows are the tree's top level (their parent is the hidden
  // virtual root, i.e. an invalid index). Child files/folders have a valid
  // parent, so this rejects every non-root row before doing any string work.
  if (!index.isValid() || index.parent().isValid())
    return false;
  return files_->isRootRow(mapToSource(index));
}

QString mufpen::FileTreeModel::workspaceLabel(const QString& path) const {
  return workspaceLabels_.value(workspaceKey(path));
}

QString mufpen::FileTreeModel::workspaceColorKey(const QString& path) const {
  return workspaceColors_.value(workspaceKey(path), workspace_style::defaultKey());
}

void mufpen::FileTreeModel::setWorkspaceLabel(const QString& path, const QString& label) {
  if (path.isEmpty())
    return;
  const QString key = workspaceKey(path);
  if (label.isEmpty())
    workspaceLabels_.remove(key);
  else
    workspaceLabels_.insert(key, label);
  saveWorkspaceStyles();
  const QModelIndex root = index(path);
  if (root.isValid())
    emit dataChanged(root, root, {WorkspaceLabelRole, WorkspaceHasLabelRole});
}

void mufpen::FileTreeModel::setWorkspaceColorKey(const QString& path, const QString& colorKey) {
  if (path.isEmpty() || !workspace_style::isValidKey(colorKey))
    return;
  workspaceColors_.insert(workspaceKey(path), colorKey);
  saveWorkspaceStyles();
  const QModelIndex root = index(path);
  if (root.isValid())
    emit dataChanged(root, root, {WorkspaceColorRole});
}

QVariant mufpen::FileTreeModel::data(const QModelIndex& index, int role) const {
  if (role >= WorkspaceLabelRole && role <= WorkspaceHasLabelRole && isWorkspaceRoot(index)) {
    const QString key = workspaceKey(filePath(index));
    switch (role) {
    case WorkspaceLabelRole: {
      const QString custom = workspaceLabels_.value(key);
      return custom.isEmpty() ? files_->fileName(mapToSource(index)) : custom;
    }
    case WorkspaceHasLabelRole:
      return !workspaceLabels_.value(key).isEmpty();
    case WorkspaceColorRole:
      return workspace_style::colorForKey(workspaceColors_.value(key, workspace_style::defaultKey()));
    case WorkspaceIsRootRole:
      return true;
    default:
      return {};
    }
  }
  return QSortFilterProxyModel::data(index, role);
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
