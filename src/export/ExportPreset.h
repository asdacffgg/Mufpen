#pragma once

#include "Export.h"
#include "export/ExportFormat.h"
#include <QVariantMap>
#include <QVector>

namespace mufpen {

// A value snapshot: exporting never temporarily rewrites global preferences.
struct MUFPEN_UI_EXPORT ExportPreset {
  QString id;
  QString name;
  ExportFormat format = ExportFormat::Pdf;
  QVariantMap options;
};

MUFPEN_UI_EXPORT QString exportExtension(ExportFormat format);
MUFPEN_UI_EXPORT QVariantMap defaultExportValues(ExportFormat format);
MUFPEN_UI_EXPORT ExportPreset builtInExportPreset(ExportFormat format);
MUFPEN_UI_EXPORT QVector<ExportPreset> exportPresets();
MUFPEN_UI_EXPORT void saveExportPresets(const QVector<ExportPreset>& presets);
// Custom commands receive individual arguments, expanded after tokenization.
// ${outputPath} always names the staged file, not the existing destination.
MUFPEN_UI_EXPORT bool exportCommandArguments(const ExportPreset& preset, const QVariantMap& variables, QString* program,
                                             QStringList* arguments);
MUFPEN_UI_EXPORT QString exportPresetExtension(const ExportPreset& preset);

} // namespace mufpen
