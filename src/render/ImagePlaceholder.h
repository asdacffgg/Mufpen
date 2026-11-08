#pragma once

#include <QImage>
#include <QSizeF>

class QString;

namespace mufpen::image_placeholder {

/// Render the "loading" placeholder icon at the given logical size.
/// The returned QImage has devicePixelRatio set appropriately.
QImage loading(QSizeF logicalSize);

/// Render the "broken image" placeholder icon at the given logical size.
QImage broken(QSizeF logicalSize);

} // namespace mufpen::image_placeholder
