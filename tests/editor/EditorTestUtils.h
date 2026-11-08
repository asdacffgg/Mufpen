#pragma once

#include "../TestUtils.h"

#include "document/DocumentSession.h"
#include "document/MarkdownNode.h"
#include "edit/UndoStack.h"
#include "editor/BrushQueue.h"
#include "editor/ClipboardController.h"
#include "editor/EditorController.h"
#include "editor/EditorView.h"
#include "editor/InputController.h"
#include "editor/SelectionController.h"
#include "projection/SelectionSerializer.h"

#include <QApplication>
#include <QKeyEvent>
#include <QSettings>

#include <iostream>

// SettingsOverride (RAII QSettings key override) is provided by TestUtils.h, included above.

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

inline mufpen::MarkdownNode* maybeFirstChildOfType(mufpen::MarkdownNode* node, mufpen::BlockType type) {
  if (!node) {
    return nullptr;
  }
  for (const auto& child : node->children()) {
    if (child->type() == type) {
      return child.get();
    }
  }
  return nullptr;
}

inline mufpen::MarkdownNode* firstBlockOfType(const mufpen::DocumentSession& session, mufpen::BlockType type) {
  for (const auto& child : session.document().root().children()) {
    if (child->type() == type) {
      return child.get();
    }
  }
  require(false, "block type not found");
  return nullptr;
}

inline int listDepthForItem(const mufpen::MarkdownNode* item) {
  int depth = 0;
  const mufpen::MarkdownNode* node = item;
  while (node) {
    if (node->type() == mufpen::BlockType::ListItem) {
      ++depth;
    }
    node = node->parent();
  }
  return depth;
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

inline mufpen::CursorPosition inlineCursor(mufpen::NodeId blockId, qsizetype textOffset, qsizetype sourceOffset) {
  mufpen::CursorPosition cursor;
  cursor.blockId = blockId;
  cursor.text.nodeId = blockId;
  cursor.text.textOffset = textOffset;
  cursor.text.sourceOffset = sourceOffset;
  return cursor;
}

inline bool pressKey(mufpen::InputController& input, QObject* target, int key,
                     Qt::KeyboardModifiers modifiers = Qt::NoModifier) {
  QKeyEvent event(QEvent::KeyPress, key, modifiers);
  return input.eventFilter(target, &event);
}

inline void sendKey(QWidget* target, int key, Qt::KeyboardModifiers modifiers = Qt::NoModifier) {
  QKeyEvent event(QEvent::KeyPress, key, modifiers);
  QApplication::sendEvent(target, &event);
}

inline void wireInput(mufpen::InputController& input, mufpen::DocumentSession& session,
                      mufpen::SelectionController& selection, mufpen::UndoStack& undoStack,
                      mufpen::BrushQueue& brushQueue, mufpen::EditorView* view = nullptr,
                      QHash<int, mufpen::LiteralBlockController*> literalEditors = {}) {
  input.setContext({&session, &selection, &undoStack, &brushQueue, view, literalEditors});
}

inline void wireStyle(mufpen::StylizeController& stylize, mufpen::DocumentSession& session,
                      mufpen::SelectionController& selection, mufpen::UndoStack& undoStack,
                      mufpen::BrushQueue& brushQueue) {
  stylize.setContext({&session, &selection, &undoStack, &brushQueue});
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

inline void setCrossSelection(mufpen::SelectionController& selection, mufpen::MarkdownNode* anchorBlock,
                              qsizetype anchorOffset, mufpen::MarkdownNode* focusBlock, qsizetype focusOffset) {
  mufpen::SelectionRange range;
  range.anchor.blockId = anchorBlock->id();
  range.anchor.text.nodeId = anchorBlock->id();
  range.anchor.text.textOffset = anchorOffset;
  range.focus.blockId = focusBlock->id();
  range.focus.text.nodeId = focusBlock->id();
  range.focus.text.textOffset = focusOffset;
  selection.setSelection(range);
}

inline QString selectedMarkdown(const mufpen::DocumentSession& session, const mufpen::SelectionController& selection) {
  return mufpen::SelectionSerializer().exportMarkdown(session.document(), selection.selection());
}

inline QString selectedPlainText(const mufpen::DocumentSession& session, const mufpen::SelectionController& selection) {
  return mufpen::SelectionSerializer().exportPlainText(session.document(), selection.selection());
}

inline void wireClipboard(mufpen::ClipboardController& clipboard, mufpen::DocumentSession& session,
                          mufpen::SelectionController& selection, mufpen::InputController& input) {
  clipboard.setContext({&session, &selection, nullptr, nullptr});
  clipboard.setInputController(&input);
}
