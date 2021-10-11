#pragma once

namespace goya {

class IDrawable {
 public:
  virtual auto Draw() -> void = 0;
  virtual ~IDrawable() = default;
};

}  // namespace goya
