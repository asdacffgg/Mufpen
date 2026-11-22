#pragma once

#include <QColor>
#include <QString>
#include <QVector>

// Preset accents for workspace section headers. All tones are light, bright and
// slightly muted so they sit naturally on both dark and light chrome surfaces
// without shouting over the document. The UI applies them translucently.
namespace mufpen::workspace_style {

struct Accent {
  const char* key;
  const char* label; // English source text; the menu wraps it in tr().
  QColor color;
};

// Light, bright, harmonious tones: mist white, sky blue, mint, lilac, peach,
// honey, rose and aqua. Chosen to stay legible on ayu/dark and whitey/light.
inline const QVector<Accent>& accents() {
  static const QVector<Accent> list{
      {"mist", "Mist", QColor::fromRgb(0xE6, 0xEA, 0xF2)},
      {"sky", "Sky", QColor::fromRgb(0x8A, 0xB4, 0xF8)},
      {"mint", "Mint", QColor::fromRgb(0x86, 0xCC, 0xA0)},
      {"lilac", "Lilac", QColor::fromRgb(0xC4, 0x9B, 0xEC)},
      {"peach", "Peach", QColor::fromRgb(0xF4, 0xB3, 0x86)},
      {"honey", "Honey", QColor::fromRgb(0xE9, 0xC6, 0x6E)},
      {"rose", "Rose", QColor::fromRgb(0xEE, 0x9A, 0xAC)},
      {"aqua", "Aqua", QColor::fromRgb(0x7B, 0xD2, 0xD4)},
  };
  return list;
}

inline QString defaultKey() {
  return QStringLiteral("mist");
}

inline QColor colorForKey(const QString& key) {
  for (const Accent& accent : accents()) {
    if (key.compare(QLatin1String(accent.key), Qt::CaseInsensitive) == 0)
      return accent.color;
  }
  return accents().first().color;
}

inline bool isValidKey(const QString& key) {
  for (const Accent& accent : accents()) {
    if (key.compare(QLatin1String(accent.key), Qt::CaseInsensitive) == 0)
      return true;
  }
  return false;
}

} // namespace mufpen::workspace_style
