#pragma once

#include "Export.h"
#include <QString>

namespace mufpen {

// Initialize before creating any application settings/services. A marker beside
// the executable opts this copy into an independent, relocatable user profile.
class MUFPEN_CORE_EXPORT ApplicationPaths {
public:
  static bool initialize(const QString& executablePath, QString* error);
  static bool isPortable();
  static QString dataDirectory();
  static QString localDataDirectory();
  static QString temporaryDirectory();
};

} // namespace mufpen
