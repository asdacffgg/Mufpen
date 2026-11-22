#include "app/WorkspaceRootDelegate.h"

#include "app/FileTreeModel.h"

#include <QFontMetrics>
#include <QPainter>

namespace {

constexpr int kBandInsetX = 4;   // breathing room left/right of the tinted band
constexpr int kBandInsetY = 1;
constexpr qreal kBandRadius = 5.0;
constexpr int kAnchorWidth = 3;  // accent bar marking the start of a workspace
constexpr int kLineInset = 10;   // separator stops short of the band's corners
constexpr int kPillPadding = 7;  // horizontal text padding inside the label pill
constexpr int kPillInset = 8;    // gap between the pill and the band's right edge

// Straight alpha-free RGB blend; the result is opaque so it stays readable over
// any surface. `amount` is how much of `tint` is mixed into `base`.
QColor blend(const QColor& base, const QColor& tint, qreal amount) {
  const qreal keep = 1.0 - amount;
  return QColor::fromRgbF(base.redF() * keep + tint.redF() * amount,
                          base.greenF() * keep + tint.greenF() * amount,
                          base.blueF() * keep + tint.blueF() * amount);
}

} // namespace

mufpen::WorkspaceRootDelegate::WorkspaceRootDelegate(FileTreeModel* model, QObject* parent)
    : QStyledItemDelegate(parent), model_(model) {}

void mufpen::WorkspaceRootDelegate::setThemeColors(const QColor& text, const QColor& surface) {
  if (themeText_ == text && themeSurface_ == surface)
    return;
  themeText_ = text;
  themeSurface_ = surface;
}

void mufpen::WorkspaceRootDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                          const QModelIndex& index) const {
  if (!model_ || !model_->isWorkspaceRoot(index)) {
    QStyledItemDelegate::paint(painter, option, index);
    return;
  }

  const QColor accent = model_->data(index, mufpen::FileTreeModel::WorkspaceColorRole).value<QColor>();
  const QString label = model_->data(index, mufpen::FileTreeModel::WorkspaceLabelRole).toString();
  const bool customLabel = model_->data(index, mufpen::FileTreeModel::WorkspaceHasLabelRole).toBool();
  if (!accent.isValid()) {
    QStyledItemDelegate::paint(painter, option, index);
    return;
  }

  // The presets are pale on purpose: on a light surface the very light ones need
  // a touch more density to stay visible, while dark surfaces take them as-is.
  const QColor surface = themeSurface_.isValid() ? themeSurface_ : option.palette.color(QPalette::Window);
  const bool lightSurface = surface.lightness() > 128;
  const QColor textColor = themeText_.isValid() ? themeText_ : option.palette.color(QPalette::Text);

  // Decorative tones derive from the accent: `wash` is the band behind the row,
  // `stroke` the anchor bar, separator and pill outline. The palest presets (mist
  // on white) need a denser neutral, otherwise the section disappears.
  const bool paleAccent = accent.lightness() > 205;
  const QColor stroke = (lightSurface && paleAccent) ? accent.darker(145) : accent;
  const QColor wash = (lightSurface && paleAccent) ? stroke : accent;

  const QRect row = option.rect;
  const QRect band(row.left() + kBandInsetX, row.top() + kBandInsetY, row.width() - 2 * kBandInsetX,
                   row.height() - 2 * kBandInsetY - 1);

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing, true);

  // 1. Section wash: a translucent, rounded accent band behind the whole row.
  if (band.width() > 8 && band.height() > 4) {
    QColor fill = wash;
    fill.setAlphaF(lightSurface ? 0.16 : 0.11);
    painter->setPen(Qt::NoPen);
    painter->setBrush(fill);
    painter->drawRoundedRect(QRectF(band), kBandRadius, kBandRadius);

    // 2. Anchor bar: a slim rounded accent strip at the band's left edge, so the
    //    workspace colour is unmistakable without shouting. It sits just left of
    //    the branch arrow, so the tree's own chevron stays clear.
    QColor anchor = stroke;
    anchor.setAlphaF(lightSurface ? 0.7 : 0.75);
    painter->setBrush(anchor);
    painter->drawRoundedRect(QRectF(band.left(), band.top() + 3.0, kAnchorWidth, band.height() - 6.0),
                             kAnchorWidth / 2.0, kAnchorWidth / 2.0);

    // 3. Separator: hairline above every workspace except the first, with soft
    //    rounded ends so it reads as a divider rather than a table rule.
    if (index.row() > 0) {
      QColor separator = stroke;
      separator.setAlphaF(lightSurface ? 0.38 : 0.55);
      painter->setPen(QPen(separator, 1.0, Qt::SolidLine, Qt::RoundCap));
      painter->drawLine(QPointF(band.left() + kLineInset, band.top()),
                        QPointF(band.right() - kLineInset, band.top()));
    }
  }

  painter->restore();
  painter->save();

  // 4. Row content (chevron, icon, folder name, hover/selection background).
  //    The workspace title reads as a title: same size, slightly heavier weight.
  QStyleOptionViewItem titleOption(option);
  titleOption.font.setWeight(QFont::DemiBold);
  QStyledItemDelegate::paint(painter, titleOption, index);

  // 5. Custom workspace label: a quiet pill on the right, tinted by the accent.
  if (customLabel && !label.isEmpty()) {
    QFont pillFont = option.font;
    if (pillFont.pointSizeF() > 1.0)
      pillFont.setPointSizeF(pillFont.pointSizeF() - 1.0);
    pillFont.setWeight(QFont::DemiBold);
    const QFontMetrics fm(pillFont);
    const int available = band.width() - 2 * kPillInset - kLineInset;
    const int textWidth = qMin(fm.horizontalAdvance(label), qMax(0, available - 2 * kPillPadding));
    if (textWidth > 0) {
      const int pillHeight = qMin(fm.height() + 4, band.height() + 2);
      const QRectF pill(band.right() - kPillInset - textWidth - 2 * kPillPadding,
                        band.center().y() - pillHeight / 2.0 + 0.5, textWidth + 2 * kPillPadding, pillHeight);

      QColor pillFill = accent;
      pillFill.setAlphaF(lightSurface ? 0.32 : 0.22);
      QColor pillStroke = stroke;
      pillStroke.setAlphaF(lightSurface ? 0.45 : 0.42);

      painter->setRenderHint(QPainter::Antialiasing, true);
      painter->setPen(QPen(pillStroke, 1.0));
      painter->setBrush(pillFill);
      painter->drawRoundedRect(pill, pillHeight / 2.0, pillHeight / 2.0);

      painter->setFont(pillFont);
      painter->setPen(blend(textColor, accent, 0.4));
      painter->drawText(pill, Qt::AlignCenter, fm.elidedText(label, Qt::ElideRight, textWidth));
    }
  }

  painter->restore();
}
