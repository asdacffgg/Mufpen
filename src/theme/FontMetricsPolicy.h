#pragma once
#include <QFont>

namespace mufpen::font_rendering {
// DirectWrite adds a uniform glyph advance when it synthesizes bold from a
// static regular face. CSS emboldening preserves advances. Return only a
// verified simulation delta; real bold and variable faces keep their metrics.
qreal syntheticBoldAdvanceCorrection(const QFont& font);
} // namespace mufpen::font_rendering
