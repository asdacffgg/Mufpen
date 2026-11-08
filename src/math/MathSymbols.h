#pragma once

#include "math/MathParseNode.h"

#include <QString>

namespace mufpen::math {

struct MathSymbolInfo {
  QString replacement;
  MathNodeType type = MathNodeType::Ord;
  QString fontClass = QStringLiteral("mathnormal");
  bool known = false;
};

struct WideCharMapping {
  QString baseChar;
  QString fontClass;
};

MathSymbolInfo lookupSymbol(const QString& token);
WideCharMapping wideCharacterFont(uint codePoint);

} // namespace mufpen::math
