#include "editor/SourceEditorWidget.h"

#include "editor/VirtualSourceEdit.h"
#include "theme/FontRendering.h"
#include "theme/RenderTheme.h"

#include <QCoreApplication>
#include <QEvent>
#include <QFont>
#include <QHBoxLayout>

mufpen::SourceEditorWidget::SourceEditorWidget(QWidget* parent) : QWidget(parent) {
  setAttribute(Qt::WA_StyledBackground, true);
  auto* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 16, 0, 0);
  layout->setSpacing(0);

  editor_ = new VirtualSourceEdit(this);
  editor_->setMinimumWidth(0);
  setFocusProxy(editor_);
  // Source editing uses the available workspace, independently of the
  // rendered document's reading width. Keep its scrollbar at the outer edge.
  layout->addWidget(editor_, 1);

  connect(editor_, &VirtualSourceEdit::cursorPositionChanged, this, &SourceEditorWidget::cursorPositionChanged);
  connect(editor_, &VirtualSourceEdit::editApplied, this, &SourceEditorWidget::editApplied);
  setupStyle();
  retranslateUi();
}

void mufpen::SourceEditorWidget::bindSession(DocumentSession* session) {
  editor_->bindSession(session);
}

void mufpen::SourceEditorWidget::syncFromSession(bool preserveCursor) {
  editor_->syncFromSession(preserveCursor);
}

void mufpen::SourceEditorWidget::notifyDocumentChanged() {
  editor_->notifyDocumentChanged();
}

QString mufpen::SourceEditorWidget::text() const {
  return editor_->text();
}
void mufpen::SourceEditorWidget::setText(const QString& text) {
  editor_->setStandaloneText(text);
}
qsizetype mufpen::SourceEditorWidget::cursorPosition() const {
  return editor_->cursorPosition();
}
qsizetype mufpen::SourceEditorWidget::anchorPosition() const {
  return editor_->anchorPosition();
}
qsizetype mufpen::SourceEditorWidget::selectionStart() const {
  return editor_->selectionStart();
}
qsizetype mufpen::SourceEditorWidget::selectionEnd() const {
  return editor_->selectionEnd();
}
bool mufpen::SourceEditorWidget::hasSelection() const {
  return editor_->hasSelection();
}
void mufpen::SourceEditorWidget::toggleInlineMarkup(const QString& prefix, const QString& suffix) {
  editor_->toggleInlineMarkup(prefix, suffix);
}

QString mufpen::SourceEditorWidget::selectedText() const {
  return editor_->selectedText();
}
void mufpen::SourceEditorWidget::setCursorPosition(qsizetype position, bool keepAnchor) {
  editor_->setCursorPosition(position, keepAnchor);
}
void mufpen::SourceEditorWidget::setSelection(qsizetype start, qsizetype end) {
  editor_->setSelection(start, end);
}
void mufpen::SourceEditorWidget::selectAll() {
  editor_->selectAll();
}
void mufpen::SourceEditorWidget::selectLine() {
  editor_->selectLine();
}
void mufpen::SourceEditorWidget::selectWord() {
  editor_->selectWord();
}
void mufpen::SourceEditorWidget::insertText(const QString& text) {
  editor_->insertText(text);
}
void mufpen::SourceEditorWidget::replaceAllWithUndo(const QString& text) {
  editor_->replaceAllWithUndo(text);
}
void mufpen::SourceEditorWidget::replaceSelection(const QString& text) {
  editor_->replaceSelection(text);
}
void mufpen::SourceEditorWidget::deleteForward() {
  editor_->deleteForward();
}
void mufpen::SourceEditorWidget::deleteBackward() {
  editor_->deleteBackward();
}
void mufpen::SourceEditorWidget::deleteWord() {
  editor_->deleteWord();
}
void mufpen::SourceEditorWidget::deleteLineContent() {
  editor_->deleteLineContent();
}
void mufpen::SourceEditorWidget::deleteWholeLine() {
  editor_->deleteWholeLine();
}
void mufpen::SourceEditorWidget::moveCurrentLineUp() {
  editor_->moveCurrentLineUp();
}
void mufpen::SourceEditorWidget::moveCurrentLineDown() {
  editor_->moveCurrentLineDown();
}
bool mufpen::SourceEditorWidget::canUndo() const {
  return editor_->canUndo();
}
bool mufpen::SourceEditorWidget::canRedo() const {
  return editor_->canRedo();
}
void mufpen::SourceEditorWidget::undo() {
  editor_->undo();
}
void mufpen::SourceEditorWidget::redo() {
  editor_->redo();
}
void mufpen::SourceEditorWidget::moveDocumentStart() {
  editor_->moveDocumentStart();
}
void mufpen::SourceEditorWidget::moveDocumentEnd() {
  editor_->moveDocumentEnd();
}
void mufpen::SourceEditorWidget::moveLineStart() {
  editor_->moveLineStart();
}
void mufpen::SourceEditorWidget::moveLineEnd() {
  editor_->moveLineEnd();
}
void mufpen::SourceEditorWidget::moveLineVertical(int delta) {
  editor_->moveLineVertical(delta);
}
void mufpen::SourceEditorWidget::selectNextOccurrence() {
  editor_->selectNextOccurrence();
}
qsizetype mufpen::SourceEditorWidget::findText(QStringView text, qsizetype from) const {
  return editor_->findText(text, from);
}
qsizetype mufpen::SourceEditorWidget::findTextBackward(QStringView text, qsizetype from) const {
  return editor_->findTextBackward(text, from);
}
void mufpen::SourceEditorWidget::ensureCursorVisible() {
  editor_->ensureCursorVisible();
}
void mufpen::SourceEditorWidget::centerCursor() {
  editor_->centerCursor();
}

void mufpen::SourceEditorWidget::setFocusMode(bool enabled) {
  editor_->setFocusMode(enabled);
}

void mufpen::SourceEditorWidget::setTypewriterMode(bool enabled, bool cursorMiddle) {
  editor_->setTypewriterMode(enabled, cursorMiddle);
}
mufpen::ViewportPosition mufpen::SourceEditorWidget::viewportPosition() const {
  return editor_->viewportPosition();
}
void mufpen::SourceEditorWidget::restoreViewportPosition(const ViewportPosition& position) {
  editor_->restoreViewportPosition(position);
}
int mufpen::SourceEditorWidget::cursorLine() const {
  return editor_->cursorLine();
}
int mufpen::SourceEditorWidget::cursorColumn() const {
  return editor_->cursorColumn();
}

void mufpen::SourceEditorWidget::setDocumentPath(const QString& path) {
  editor_->setDocumentPath(path);
}

void mufpen::SourceEditorWidget::setReadOnly(bool readOnly) {
  editor_->setReadOnly(readOnly);
}

bool mufpen::SourceEditorWidget::isReadOnly() const {
  return editor_->isReadOnly();
}

void mufpen::SourceEditorWidget::setWordWrapEnabled(bool enabled) {
  editor_->setWordWrapEnabled(enabled);
}

void mufpen::SourceEditorWidget::setZoomPercent(int percent) {
  zoomPercent_ = qBound(60, percent, 200);
  applyFontSize();
}

void mufpen::SourceEditorWidget::setFontSizePx(int px) {
  fontSizePx_ = qBound(12, px, 24);
  applyFontSize();
}

void mufpen::SourceEditorWidget::setTheme(const RenderTheme& theme) {
  const SourceEditorColors colors = SourceEditorColors::fromTheme(theme);
  editor_->setColors(colors);
  setStyleSheet(
      QStringLiteral("mufpen--SourceEditorWidget { background:%1; }").arg(colors.background.name(QColor::HexRgb)));
}

void mufpen::SourceEditorWidget::changeEvent(QEvent* event) {
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();
  QWidget::changeEvent(event);
}

void mufpen::SourceEditorWidget::setupStyle() {
  applyFontSize();
  setTheme(RenderTheme::github());
}

void mufpen::SourceEditorWidget::retranslateUi() {
  editor_->setPlaceholderText(QCoreApplication::translate("mufpen::SourceEditorWidget", "Start writing..."));
}

void mufpen::SourceEditorWidget::applyFontSize() {
  QFont font = font_rendering::sourceEditorFont();
  const qreal scale = static_cast<qreal>(zoomPercent_) / 100.0 * static_cast<qreal>(fontSizePx_) / 16.0;
  font.setPointSizeF(qMax(8.0, 13.0 * scale));
  editor_->setSourceFont(font);
}
