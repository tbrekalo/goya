#pragma once

#include <cstdint>
#include <functional>

#include "goya/primitives.hpp"

namespace goya {

struct KeyEvent {
  explicit KeyEvent(std::int32_t glfw_key_code, std::int32_t glfw_key_action)
      : glfw_key_code(glfw_key_code), glfw_key_action(glfw_key_action) {}

  std::int32_t glfw_key_code;
  std::int32_t glfw_key_action;
};

struct CursorEvent {
  explicit CursorEvent(double x_pos, double y_pos)
      : x_pos(x_pos), y_pos(y_pos) {}

  double x_pos;
  double y_pos;
};

struct ResizeEvent {
  explicit ResizeEvent(std::int32_t width, std::int32_t height)
    : width(width), height(height) {}

  std::int32_t width;
  std::int32_t height;
};

using KeyEventHandler = std::function<void(KeyEvent, TimeType)>;
using CursorEventHandler = std::function<void(CursorEvent, TimeType)>;
using WinResizeEventHandler = std::function<void(ResizeEvent)>;

using AnimationHandler = std::function<void(TimeType)>;

}  // namespace goya
