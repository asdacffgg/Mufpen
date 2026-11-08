#pragma once

#include "document/DocumentSession.h"
#include "document/InlineNode.h"
#include "document/MarkdownNode.h"
#include "document/SourceRangeUtil.h"
#include "edit/UndoStack.h"
#include "editor/BrushQueue.h"
#include "editor/EditorContext.h"
#include "editor/EditorController.h"
#include "editor/EditorView.h"
#include "editor/InputController.h"
#include "editor/SelectionController.h"
#include "projection/InlineProjection.h"
#include "projection/SelectionSerializer.h"
#include "render/InlineLayout.h"
#include "theme/RenderTheme.h"

#include "../TestUtils.h"

#include <QApplication>
#include <QClipboard>
#include <QFontMetricsF>
#include <QKeyEvent>
#include <QMouseEvent>

#include <iostream>

using namespace mufpen;

inline mufpen::MarkdownNode* blockAt(const mufpen::DocumentSession& session, qsizetype index) {
  const auto& children = session.document().root().children();
  require(index >= 0 && index < static_cast<qsizetype>(children.size()), "block index out of range");
  return children.at(static_cast<size_t>(index)).get();
}

inline mufpen::MarkdownNode* childAt(mufpen::MarkdownNode* node, qsizetype index) {
  require(node != nullptr, "parent node should exist");
  const auto& children = node->children();
  require(index >= 0 && index < static_cast<qsizetype>(children.size()), "child index out of range");
  return children.at(static_cast<size_t>(index)).get();
}

inline mufpen::MarkdownNode* listItemAt(const mufpen::DocumentSession& session, qsizetype listIndex,
                                        qsizetype itemIndex) {
  return childAt(blockAt(session, listIndex), itemIndex);
}

inline void setCursor(mufpen::SelectionController& selection, mufpen::MarkdownNode* block, qsizetype offset) {
  mufpen::CursorPosition cursor;
  cursor.blockId = block->id();
  cursor.text.nodeId = block->id();
  cursor.text.textOffset = offset;
  selection.setCursorPosition(cursor);
}

inline void setSourceCursor(mufpen::SelectionController& selection, mufpen::MarkdownNode* block,
                            qsizetype visibleOffset, qsizetype sourceOffset) {
  mufpen::CursorPosition cursor;
  cursor.blockId = block->id();
  cursor.text.nodeId = block->id();
  cursor.text.textOffset = visibleOffset;
  cursor.text.sourceOffset = sourceOffset;
  selection.setCursorPosition(cursor);
}

inline bool pressKey(mufpen::InputController& input, QObject* target, int key,
                     Qt::KeyboardModifiers modifiers = Qt::NoModifier) {
  QKeyEvent event(QEvent::KeyPress, key, modifiers);
  return input.eventFilter(target, &event);
}

inline void wireInput(mufpen::InputController& input, mufpen::DocumentSession& session,
                      mufpen::SelectionController& selection, mufpen::UndoStack& undoStack,
                      mufpen::BrushQueue& brushQueue, mufpen::EditorView* view = nullptr,
                      QHash<int, mufpen::LiteralBlockController*> literalEditors = {}) {
  input.setContext({&session, &selection, &undoStack, &brushQueue, view, literalEditors});
}

inline void setSelection(mufpen::SelectionController& selection, mufpen::MarkdownNode* block, qsizetype anchor,
                         qsizetype focus) {
  mufpen::SelectionRange range;
  range.anchor.blockId = block->id();
  range.anchor.text.nodeId = block->id();
  range.anchor.text.textOffset = anchor;
  range.focus.blockId = block->id();
  range.focus.text.nodeId = block->id();
  range.focus.text.textOffset = focus;
  selection.setSelection(range);
}

inline void setSourceSelection(mufpen::SelectionController& selection, mufpen::MarkdownNode* block,
                               mufpen::MarkdownNode* textNode, qsizetype anchorTextOffset, qsizetype anchorSourceOffset,
                               qsizetype focusTextOffset, qsizetype focusSourceOffset) {
  mufpen::SelectionRange range;
  range.anchor.blockId = block->id();
  range.anchor.text.nodeId = textNode->id();
  range.anchor.text.textOffset = anchorTextOffset;
  range.anchor.text.sourceOffset = anchorSourceOffset;
  range.focus.blockId = block->id();
  range.focus.text.nodeId = textNode->id();
  range.focus.text.textOffset = focusTextOffset;
  range.focus.text.sourceOffset = focusSourceOffset;
  selection.setSelection(range);
}

inline QString selectedMarkdown(const mufpen::DocumentSession& session, const mufpen::SelectionController& selection) {
  return mufpen::SelectionSerializer().exportMarkdown(session.document(), selection.selection());
}

inline QString selectedPlainText(const mufpen::DocumentSession& session, const mufpen::SelectionController& selection) {
  return mufpen::SelectionSerializer().exportPlainText(session.document(), selection.selection());
}

struct ExpectedProjectionSpan {
  mufpen::InlineType type = mufpen::InlineType::Unknown;
  mufpen::InlineSpanKind kind = mufpen::InlineSpanKind::Text;
  qsizetype sourceStart = 0;
  qsizetype sourceEnd = 0;
  qsizetype contentSourceStart = 0;
  qsizetype contentSourceEnd = 0;
  qsizetype displayStart = 0;
  qsizetype displayEnd = 0;
  qsizetype visibleStart = 0;
  qsizetype visibleEnd = 0;
};

QVector<mufpen::InlineNode> withInlineTestRanges(QVector<mufpen::InlineNode> inlines, const QString& markdown);

inline void requireSpanContract(const mufpen::InlineProjectionSpan& span, const ExpectedProjectionSpan& expected,
                                qsizetype index, const char* label) {
  const QString prefix = QStringLiteral("%1 span %2").arg(QString::fromUtf8(label)).arg(index);
  require(span.type == expected.type, prefix + QStringLiteral(" type mismatch"));
  require(span.kind == expected.kind, prefix + QStringLiteral(" kind mismatch"));
  require(span.sourceStart == expected.sourceStart, prefix + QStringLiteral(" sourceStart mismatch"));
  require(span.sourceEnd == expected.sourceEnd, prefix + QStringLiteral(" sourceEnd mismatch"));
  require(span.contentSourceStart == expected.contentSourceStart,
          prefix + QStringLiteral(" contentSourceStart mismatch"));
  require(span.contentSourceEnd == expected.contentSourceEnd, prefix + QStringLiteral(" contentSourceEnd mismatch"));
  require(span.displayStart == expected.displayStart, prefix + QStringLiteral(" displayStart mismatch"));
  require(span.displayEnd == expected.displayEnd, prefix + QStringLiteral(" displayEnd mismatch"));
  require(span.visibleStart == expected.visibleStart, prefix + QStringLiteral(" visibleStart mismatch"));
  require(span.visibleEnd == expected.visibleEnd, prefix + QStringLiteral(" visibleEnd mismatch"));
}

inline void requireProjectionSpans(const QVector<mufpen::InlineNode>& inlines, const QString& markdown,
                                   mufpen::InlineProjectionState state, const QVector<ExpectedProjectionSpan>& expected,
                                   const QString& displayText, const QString& visibleText, const char* label) {
  mufpen::InlineProjection projection(withInlineTestRanges(inlines, markdown), markdown, state);
  require(projection.isValid(), label);
  require(projection.displayText() == displayText,
          QStringLiteral("%1 display text mismatch: %2").arg(QString::fromUtf8(label), projection.displayText()));
  require(projection.visibleText() == visibleText,
          QStringLiteral("%1 visible text mismatch: %2").arg(QString::fromUtf8(label), projection.visibleText()));
  require(projection.spans().size() == expected.size(),
          QStringLiteral("%1 span count mismatch: %2").arg(QString::fromUtf8(label)).arg(projection.spans().size()));
  for (qsizetype i = 0; i < expected.size(); ++i) {
    requireSpanContract(projection.spans().at(i), expected.at(i), i, label);
  }
}

inline void requireProjectionRoundTrip(const QVector<mufpen::InlineNode>& inlines, const QString& markdown,
                                       qsizetype cursorSourceOffset, const QString& visibleText,
                                       const QString& displayText, const char* label) {
  mufpen::InlineProjectionState state;
  state.cursorSourceOffset = cursorSourceOffset;
  mufpen::InlineProjection projection(withInlineTestRanges(inlines, markdown), markdown, state);
  require(projection.isValid(), label);
  require(projection.visibleText() == visibleText,
          QStringLiteral("%1 visible text mismatch: %2").arg(QString::fromUtf8(label), projection.visibleText()));
  require(projection.displayText() == displayText,
          QStringLiteral("%1 display text mismatch: %2").arg(QString::fromUtf8(label), projection.displayText()));

  for (qsizetype sourceOffset = 0; sourceOffset <= markdown.size(); ++sourceOffset) {
    qsizetype displayOffset = -1;
    require(projection.displayOffsetForSourceOffset(sourceOffset, displayOffset),
            "projection source->display should succeed");
    qsizetype mappedSourceOffset = -1;
    require(projection.sourceOffsetForDisplayOffset(displayOffset, mappedSourceOffset),
            "projection display->source should succeed");
    require(mappedSourceOffset >= 0 && mappedSourceOffset <= markdown.size(),
            "projection display round-trip should stay in source");
  }

  for (qsizetype visibleOffset = 0; visibleOffset <= visibleText.size(); ++visibleOffset) {
    qsizetype sourceOffset = -1;
    require(projection.sourceOffsetForVisibleOffset(visibleOffset, sourceOffset),
            "projection visible->source should succeed");
    qsizetype mappedVisibleOffset = -1;
    require(projection.visibleOffsetForSourceOffset(sourceOffset, mappedVisibleOffset),
            "projection source->visible should succeed");
    require(mappedVisibleOffset >= 0 && mappedVisibleOffset <= visibleText.size(),
            "projection visible round-trip should stay in visible text");
  }
}

inline void setInlineRanges(mufpen::InlineNode& node, qsizetype sourceStart, qsizetype sourceEnd,
                            qsizetype contentStart, qsizetype contentEnd) {
  mufpen::InlineSourceRanges ranges;
  ranges.source = {sourceStart, sourceEnd};
  ranges.content = {contentStart, contentEnd};
  ranges.openMarker = {sourceStart, contentStart};
  ranges.closeMarker = {contentEnd, sourceEnd};
  node.setSourceRanges(ranges);
}

inline QString testMarkerForInline(const mufpen::InlineNode& node) {
  switch (node.type()) {
  case mufpen::InlineType::Code:
    return QStringLiteral("`");
  case mufpen::InlineType::InlineMath:
    return QStringLiteral("$");
  case mufpen::InlineType::Emphasis:
    return node.marker().isEmpty() ? QStringLiteral("*") : node.marker();
  case mufpen::InlineType::Strong:
    return node.marker().isEmpty() ? QStringLiteral("**") : node.marker();
  case mufpen::InlineType::Strikethrough:
    return QStringLiteral("~~");
  default:
    return {};
  }
}

inline void annotateInlineTestRanges(QVector<mufpen::InlineNode>& inlines, const QString& markdown,
                                     qsizetype sourceStart, qsizetype sourceEnd) {
  qsizetype searchFrom = sourceStart;
  for (mufpen::InlineNode& node : inlines) {
    const QString nodeMarkdown = mufpen::InlineProjection::markdownForInlines({node});
    const qsizetype start = markdown.indexOf(nodeMarkdown, searchFrom);
    if (start < 0 || start + nodeMarkdown.size() > sourceEnd) {
      continue;
    }
    const qsizetype end = start + nodeMarkdown.size();
    const QString marker = testMarkerForInline(node);
    if (!marker.isEmpty() && end - start >= marker.size() * 2 &&
        (node.type() == mufpen::InlineType::Code || node.type() == mufpen::InlineType::InlineMath ||
         node.type() == mufpen::InlineType::Emphasis || node.type() == mufpen::InlineType::Strong ||
         node.type() == mufpen::InlineType::Strikethrough)) {
      setInlineRanges(node, start, end, start + marker.size(), end - marker.size());
      annotateInlineTestRanges(node.children(), markdown, start + marker.size(), end - marker.size());
    } else if (node.type() == mufpen::InlineType::Link) {
      mufpen::InlineSourceRanges ranges;
      ranges.source = {start, end};
      ranges.openMarker = {start, start + 1};
      const qsizetype labelEnd = markdown.indexOf(QLatin1Char(']'), start);
      ranges.content = labelEnd >= 0 && labelEnd <= end ? mufpen::InlineRange{start + 1, labelEnd}
                                                        : mufpen::InlineRange{start + 1, start + 1};
      node.setSourceRanges(ranges);
      annotateInlineTestRanges(node.children(), markdown, ranges.content.start, ranges.content.end);
    } else if (node.type() == mufpen::InlineType::Image) {
      mufpen::InlineSourceRanges ranges;
      ranges.source = {start, end};
      ranges.openMarker = {start, start + 2};
      const qsizetype labelEnd = markdown.indexOf(QLatin1Char(']'), start);
      ranges.content = labelEnd >= 0 && labelEnd <= end ? mufpen::InlineRange{start + 2, labelEnd}
                                                        : mufpen::InlineRange{start + 2, start + 2};
      node.setSourceRanges(ranges);
    } else {
      mufpen::InlineSourceRanges ranges;
      ranges.source = {start, end};
      ranges.content = ranges.source;
      node.setSourceRanges(ranges);
    }
    searchFrom = end;
  }
}

inline QVector<mufpen::InlineNode> withInlineTestRanges(QVector<mufpen::InlineNode> inlines, const QString& markdown) {
  annotateInlineTestRanges(inlines, markdown, 0, markdown.size());
  return inlines;
}

inline const mufpen::BlockLayout* requireViewBlock(mufpen::EditorView& view, mufpen::NodeId blockId,
                                                   const QString& label) {
  const QRectF blockRect = view.nodeRect(blockId);
  require(!blockRect.isEmpty(), label + QStringLiteral(" block rect should exist"));
  const mufpen::BlockLayout* block = view.blockAtViewportPos(blockRect.center());
  require(block != nullptr, label + QStringLiteral(" block layout should exist"));
  return block;
}

inline const mufpen::InlineLayout* requireViewInlineLayout(mufpen::EditorView& view, mufpen::NodeId blockId,
                                                           const QString& label) {
  const mufpen::BlockLayout* block = requireViewBlock(view, blockId, label);
  require(block->inlineLayout() != nullptr, label + QStringLiteral(" inline layout should exist"));
  return block->inlineLayout();
}

inline mufpen::CursorPosition inlineCursor(mufpen::NodeId blockId, qsizetype textOffset, qsizetype sourceOffset) {
  mufpen::CursorPosition cursor;
  cursor.blockId = blockId;
  cursor.text.nodeId = blockId;
  cursor.text.textOffset = textOffset;
  cursor.text.sourceOffset = sourceOffset;
  return cursor;
}

inline mufpen::HitTestResult hitAtTextOffset(mufpen::EditorView& view, mufpen::NodeId blockId, qsizetype textOffset,
                                             const QString& label) {
  const QRectF blockRect = view.nodeRect(blockId);
  const mufpen::InlineLayout* inlineLayout = requireViewInlineLayout(view, blockId, label);
  const QRectF cursor = inlineLayout->cursorRect(textOffset);
  require(!cursor.isEmpty(), label + QStringLiteral(" cursor rect should exist"));
  return view.hitTest(blockRect.topLeft() + QPointF(cursor.left(), cursor.center().y()));
}

inline mufpen::HitTestResult hitAtSourceOffset(mufpen::EditorView& view, mufpen::NodeId blockId, qsizetype sourceOffset,
                                               const QString& label) {
  const QRectF blockRect = view.nodeRect(blockId);
  const mufpen::InlineLayout* inlineLayout = requireViewInlineLayout(view, blockId, label);
  const QRectF cursor = inlineLayout->cursorRectForSourceOffset(sourceOffset);
  require(!cursor.isEmpty(), label + QStringLiteral(" source cursor rect should exist"));
  return view.hitTest(blockRect.topLeft() + QPointF(cursor.left(), cursor.center().y()));
}

inline mufpen::SelectionRange inlineSelection(mufpen::NodeId blockId, qsizetype anchorOffset, qsizetype focusOffset) {
  mufpen::SelectionRange selection;
  selection.anchor = inlineCursor(blockId, anchorOffset, anchorOffset);
  selection.focus = inlineCursor(blockId, focusOffset, focusOffset);
  return selection;
}

struct CursorLineRange {
  qsizetype start = 0;
  qsizetype end = 0;
  qreal y = 0;
};

inline QVector<CursorLineRange> cursorLineRanges(const mufpen::InlineLayout& layout) {
  QVector<CursorLineRange> ranges;
  const qsizetype length = layout.plainText().size();
  for (qsizetype offset = 0; offset <= length; ++offset) {
    const QRectF cursor = layout.cursorRect(offset);
    if (cursor.isEmpty()) {
      continue;
    }
    const qreal y = cursor.center().y();
    if (ranges.isEmpty() || qAbs(ranges.last().y - y) > 0.5) {
      CursorLineRange range;
      range.start = offset;
      range.end = offset;
      range.y = y;
      ranges.push_back(range);
    } else {
      ranges.last().end = offset;
    }
  }
  return ranges;
}
