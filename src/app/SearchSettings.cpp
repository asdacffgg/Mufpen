#include "app/SearchSettings.h"
#include "app/FileOpenProtection.h"
#include <QSettings>

QString mufpen::SearchSettings::defaultExtensions() {
  return file_open_protection::defaultTextExtensions();
}

mufpen::SearchSettings mufpen::SearchSettings::load() {
  QSettings settings;
  SearchSettings result;
  result.extensions = file_open_protection::normalizedTextExtensions(
      settings.value("search/extensions", defaultExtensions()).toString());
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
