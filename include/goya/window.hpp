#pragma once

#include <cstdint>
#include <string>
#include <vector>

/* clang-format off */
#include "GL/glew.h"
#include "GLFW/glfw3.h"
/* clang-format on */

#include "goya/events.hpp"

namespace goya {

class Window {
 public:
  Window(std::int32_t width, std::int32_t height, std::string title);

  Window(Window const&) = delete;
  Window& operator=(Window const&) = delete;

  Window(Window&&) = delete;
  Window& operator=(Window&&) = delete;

  auto Refresh() -> bool;

  auto Width() const noexcept -> std::int32_t;
  auto Height() const noexcept -> std::int32_t;
  auto AspectRatio() const noexcept -> float;

  auto CursorPosition() const -> std::pair<double, double>;

  auto Title() const -> std::string const&;
  auto WinPtr() -> GLFWwindow*;

  auto AddKeyHandler(KeyEventHandler key_handler) -> void;
  auto AddCursorHandler(CursorEventHandler mouse_handler) -> void;
  auto AddWinResizeHandler(WinResizeEventHandler win_resize_handlers) -> void;
  auto AddAnimationHandler(AnimationHandler animation_handler) -> void;

  ~Window();

 private:
  struct GlfwBridge {
    auto ResizeCallback(std::int32_t const width, std::int32_t const height)
        -> void;

    auto CallKeyEventHandlers(TimeType const dleta) -> void;

    auto CallCursorEventHandlers(TimeType const delta) -> void;

    auto CallAnimationHandlers(TimeType const delta) -> void;

    std::int32_t width_;
    std::int32_t height_;

    std::vector<KeyEvent> key_events_;
    std::vector<CursorEvent> cursor_events_;

    std::vector<KeyEventHandler> key_handlers_;
    std::vector<CursorEventHandler> cursor_handlers_;
    std::vector<WinResizeEventHandler> win_resize_handlers_;
    std::vector<AnimationHandler> animation_handlers_;
  };

  std::string title_;
  GLFWwindow* win_ptr_;

  double prev_refresh_;
  GlfwBridge gb_;
};

}  // namespace goya
