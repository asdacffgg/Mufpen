#include "io/ApplicationPaths.h"
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QTemporaryFile>

namespace {
QString portableRoot;
}

bool mufpen::ApplicationPaths::initialize(const QString& executablePath, QString* error) {
  const QDir application(QFileInfo(executablePath).absolutePath());
  const QFileInfo marker(application.filePath(QStringLiteral("Mufpen.portable")));
  if (!marker.exists())
    return true;
  const QString root = application.filePath(QStringLiteral("Data"));
  QTemporaryFile probe(QDir(root).filePath(QStringLiteral(".write-check-XXXXXX")));
  if (!marker.isFile() || !QDir().mkpath(root) || !probe.open()) {
    if (error)
      *error = QDir::toNativeSeparators(root) + QLatin1Char('\n') + probe.errorString();
    return false; // Never fall back to the registry if the portable drive is read-only.
  }
  portableRoot = QDir(root).canonicalPath();
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, portableRoot + QStringLiteral("/settings"));
  // Prevent fallback reads from a machine-wide INI profile as well.
  QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, portableRoot + QStringLiteral("/settings"));
  return true;
}

bool mufpen::ApplicationPaths::isPortable() {
  return !portableRoot.isEmpty();
}

QString mufpen::ApplicationPaths::dataDirectory() {
  return isPortable() ? portableRoot : QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

QString mufpen::ApplicationPaths::localDataDirectory() {
  return isPortable() ? portableRoot : QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
}

QString mufpen::ApplicationPaths::temporaryDirectory() {
  if (!isPortable())
    return QStandardPaths::writableLocation(QStandardPaths::TempLocation);
  const auto path = portableRoot + QStringLiteral("/temp");
  QDir().mkpath(path);
  return path;
}
