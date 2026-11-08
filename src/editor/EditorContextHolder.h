#pragma once

#include "editor/EditorContext.h"

namespace mufpen {

class EditorContextHolder {
public:
  void setContext(const EditorContext& ctx) { ctx_ = ctx; }

protected:
  const EditorContext& context() const { return ctx_; }
  EditorContext& context() { return ctx_; }

  EditorContext ctx_;
};

} // namespace mufpen
