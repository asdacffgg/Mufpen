#include "app/SearchSettings.h"
#include "app/FileOpenProtection.h"
#include "io/DocumentFileType.h"
#include <QSettings>

QString mufpen::SearchSettings::defaultExtensions() {
  // Quick Open and workspace search follow the same Markdown-only document
  // policy as the open path.
  return markdownExtensions().join(QStringLiteral(", "));
}

mufpen::SearchSettings mufpen::SearchSettings::load() {
  QSettings settings;
  SearchSettings result;
  result.extensions = file_open_protection::normalizedTextExtensions(
      settings.value("search/extensions", defaultExtensions()).toString());
  // Saved settings from older versions may include data files; workspace
  // search and Quick Open only cover openable Markdown documents.
  result.extensions.removeIf(
      [](const QString& extension) { return !markdownExtensions().contains(extension, Qt::CaseInsensitive); });
  result.scan.ignorePatterns =
      settings.value("search/ignorePatterns", WorkspaceScanOptions::defaultIgnorePatterns()).toString();
  result.scan.includeHidden = settings.value("search/includeHidden", false).toBool();
  result.scan.readGitIgnore = settings.value("search/readGitIgnore", true).toBool();
  result.scan.readIgnore = settings.value("search/readIgnore", true).toBool();
  result.maxFileBytes = qBound<qint64>(
      1024 * 1024, settings.value("search/maxFileBytes", result.maxFileBytes).toLongLong(), maximumFileBytes);
  result.maxRegexBytes = qBound<qint64>(
      1024 * 1024, settings.value("search/maxRegexBytes", result.maxRegexBytes).toLongLong(), maximumFileBytes);
  return result;
}
