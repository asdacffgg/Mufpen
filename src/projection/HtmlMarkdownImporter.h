#pragma once
#include <QString>

namespace mufpen {
// Semantic clipboard conversion: sanitize, normalize content/empty wrappers,
// then serialize with explicit block boundaries. No theme or font dependency;
// inline display:none and HTML hidden are omitted, while aria-hidden alone is
// not treated as visual hiding. Active content/URLs use the preview's policy.
QString importHtmlMarkdown(const QString& html);
}
