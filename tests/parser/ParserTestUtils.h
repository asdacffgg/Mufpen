#pragma once

#include "../TestUtils.h"

#include "document/LineStartOffsetCache.h"
#include "document/MarkdownDocument.h"
#include "parser/CmarkGfmParser.h"
#include "parser/MarkdownSerializer.h"

#include <QCoreApplication>
#include <QDebug>

inline const mufpen::MarkdownNode& childAt(const mufpen::MarkdownNode& node, qsizetype index) {
  require(index >= 0 && index < node.children().size(), QStringLiteral("Missing child %1").arg(index));
  return *node.children()[index];
}

inline const mufpen::MarkdownNode& definitionByLabel(const mufpen::MarkdownNode& root, mufpen::BlockType type,
                                                     const QString& label) {
  for (const auto& child : root.children()) {
    if (child->type() == type && child->definition().label == label) {
      return *child;
    }
  }
  qCritical().noquote()
      << QStringLiteral("definition block not found: type=%1 label=%2").arg(static_cast<int>(type)).arg(label);
  std::exit(1);
  return root;
}

inline int countInlineMath(const QVector<mufpen::InlineNode>& inlines) {
  int count = 0;
  for (const mufpen::InlineNode& inlineNode : inlines) {
    if (inlineNode.type() == mufpen::InlineType::InlineMath) {
      ++count;
    }
    count += countInlineMath(inlineNode.children());
  }
  return count;
}

inline int countInlineMath(const mufpen::MarkdownNode& node) {
  int count = countInlineMath(node.inlines());
  for (const auto& child : node.children()) {
    count += countInlineMath(*child);
  }
  return count;
}

inline QString sourceTextForNode(const QString& markdown, const mufpen::MarkdownNode& node) {
  const mufpen::SourceRange range = node.sourceRange();
  require(range.byteStart >= 0, QStringLiteral("node source range start is invalid"));
  require(range.byteEnd >= range.byteStart, QStringLiteral("node source range end is invalid"));
  require(range.byteEnd <= markdown.size(), QStringLiteral("node source range exceeds markdown size"));
  return markdown.mid(range.byteStart, range.byteEnd - range.byteStart);
}

inline bool containsInlineMathText(const QVector<mufpen::InlineNode>& inlines, const QString& text) {
  for (const mufpen::InlineNode& inlineNode : inlines) {
    if (inlineNode.type() == mufpen::InlineType::InlineMath && inlineNode.text() == text) {
      return true;
    }
    if (containsInlineMathText(inlineNode.children(), text)) {
      return true;
    }
  }
  return false;
}

inline bool containsInlineMathText(const mufpen::MarkdownNode& node, const QString& text) {
  if (containsInlineMathText(node.inlines(), text)) {
    return true;
  }
  for (const auto& child : node.children()) {
    if (containsInlineMathText(*child, text)) {
      return true;
    }
  }
  return false;
}

inline int countMathBlocks(const mufpen::MarkdownNode& node) {
  int count = node.type() == mufpen::BlockType::MathBlock ? 1 : 0;
  for (const auto& child : node.children()) {
    count += countMathBlocks(*child);
  }
  return count;
}
