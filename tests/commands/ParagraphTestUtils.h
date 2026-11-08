#pragma once

#include "../TestUtils.h"

#include "document/DocumentSession.h"
#include "commands/ParagraphController.h"
#include "document/MarkdownNode.h"
#include "edit/UndoStack.h"
#include "editor/BrushQueue.h"
#include "editor/EditorContext.h"
#include "editor/SelectionController.h"

#include <QApplication>

#include <cstdlib>
#include <iostream>

inline mufpen::EditTransaction requireTextDeltaCommand(mufpen::UndoStack& stack, const char* message) {
  require(stack.canUndo(), "expected undo command");
  mufpen::EditTransaction transaction = stack.takeUndo();
  require(transaction.isTextDeltaCommand(), message);
  require(transaction.textDeltaCommand().isValid(), "text delta command should be valid");
  return transaction;
}

inline mufpen::MarkdownNode* blockAt(const mufpen::DocumentSession& session, qsizetype index) {
  const auto& children = session.document().root().children();
  require(index >= 0 && index < static_cast<qsizetype>(children.size()), "block index out of range");
  return children.at(static_cast<size_t>(index)).get();
}

// Returns the node the selection cursor currently sits on, or nullptr if none/invalid.
inline mufpen::MarkdownNode* cursorBlock(const mufpen::DocumentSession& session,
                                         const mufpen::SelectionController& selection) {
  const mufpen::NodeId id = selection.cursorPosition().blockId;
  return id.isValid() ? session.document().node(id) : nullptr;
}

inline mufpen::MarkdownNode* firstChildOfType(mufpen::MarkdownNode* node, mufpen::BlockType type) {
  require(node != nullptr, "parent node should exist");
  for (const auto& child : node->children()) {
    if (child->type() == type) {
      return child.get();
    }
  }
  require(false, "child type not found");
  return nullptr;
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
                               qsizetype anchorText, qsizetype anchorSource, qsizetype focusText,
                               qsizetype focusSource) {
  mufpen::SelectionRange range;
  range.anchor.blockId = block->id();
  range.anchor.text.nodeId = block->id();
  range.anchor.text.textOffset = anchorText;
  range.anchor.text.sourceOffset = anchorSource;
  range.focus.blockId = block->id();
  range.focus.text.nodeId = block->id();
  range.focus.text.textOffset = focusText;
  range.focus.text.sourceOffset = focusSource;
  selection.setSelection(range);
}

inline void wireParagraph(mufpen::ParagraphController& paragraph, mufpen::DocumentSession& session,
                          mufpen::SelectionController& selection, mufpen::UndoStack& undoStack,
                          mufpen::BrushQueue& brushQueue) {
  mufpen::EditorContext ctx;
  ctx.session = &session;
  ctx.selection = &selection;
  ctx.undoStack = &undoStack;
  ctx.brushQueue = &brushQueue;
  paragraph.setContext(ctx);
}
