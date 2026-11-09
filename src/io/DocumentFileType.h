#pragma once

#include <QFileInfo>
#include <QString>
#include <QStringList>

namespace mufpen {

// MarkText-compatible Markdown extension whitelist. Only these files are
// opened as documents; everything else is ignored by the open path.
inline const QStringList& markdownExtensions() {
  static const QStringList extensions{QStringLiteral("markdown"), QStringLiteral("mdown"),  QStringLiteral("mkdn"),
                                      QStringLiteral("md"),       QStringLiteral("mkd"),    QStringLiteral("mdwn"),
                                      QStringLiteral("mdtxt"),    QStringLiteral("mdtext"), QStringLiteral("mdx"),
                                      QStringLiteral("text"),     QStringLiteral("txt")};
  return extensions;
}

// Image formats shown in the file tree. Image preview is a later phase; for
// now these rows are visible but not openable.
inline const QStringList& imageExtensions() {
  static const QStringList extensions{QStringLiteral("jpeg"), QStringLiteral("jpg"), QStringLiteral("png"),
                                      QStringLiteral("gif"),  QStringLiteral("svg"), QStringLiteral("webp")};
  return extensions;
}

inline bool hasDocumentExtension(const QString& path, const QStringList& extensions) {
  const QString suffix = QFileInfo(path).suffix();
  if (suffix.isEmpty())
    return false;
  for (const QString& extension : extensions) {
    if (suffix.compare(extension, Qt::CaseInsensitive) == 0)
      return true;
  }
  return false;
}

inline bool isMarkdownDocumentPath(const QString& path) {
  return hasDocumentExtension(path, markdownExtensions());
}

inline bool isImageDocumentPath(const QString& path) {
  return hasDocumentExtension(path, imageExtensions());
}

inline bool isOpenableDocumentPath(const QString& path) {
  return isMarkdownDocumentPath(path) || isImageDocumentPath(path);
}

inline QString markdownFileDialogFilter() {
  QStringList patterns;
  patterns.reserve(markdownExtensions().size());
  for (const QString& extension : markdownExtensions())
    patterns.append(QStringLiteral("*.") + extension);
  return QStringLiteral("Markdown (%1)").arg(patterns.join(QLatin1Char(' ')));
}

inline QString openDocumentFileDialogFilter() {
  QStringList patterns;
  patterns.reserve(markdownExtensions().size() + imageExtensions().size());
  for (const QString& extension : markdownExtensions())
    patterns.append(QStringLiteral("*.") + extension);
  for (const QString& extension : imageExtensions())
    patterns.append(QStringLiteral("*.") + extension);
  return QStringLiteral("Markdown and images (%1)").arg(patterns.join(QLatin1Char(' ')));
}

} // namespace mufpen
