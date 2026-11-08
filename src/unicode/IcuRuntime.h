#pragma once

#include "Export.h"
#include <QString>

namespace mufpen {

// Register only our version-matched data, without changing ICU_DATA or the
// system ICU. Thread-safe; the read-only mapping remains alive until exit.
MUFPEN_UI_EXPORT bool initializeIcuData(QString* error = nullptr);
MUFPEN_UI_EXPORT QString icuDataFilePath();

} // namespace mufpen
