#pragma once

#include "theme/RenderTheme.h"

#include <QFont>
#include <QMarginsF>
#include <QString>

namespace mufpen {

struct MathRenderPlaceholder {
  QString displayText;
  QFont font;
  QMarginsF padding;
};

class MathRenderer final {
public:
  MathRenderPlaceholder renderBlockPlaceholder(const QString& tex, const RenderTheme& theme) const;
};

} // namespace mufpen
