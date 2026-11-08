#pragma once

#include "render/CodeHighlight.h"

#include <QString>
#include <QVector>

namespace mufpen {

class TreeSitterHighlighter {
public:
  QVector<CodeHighlightSpan> highlight(const QString& language, const QString& text) const;
  bool supportsLanguage(const QString& language) const;
};

} // namespace mufpen
