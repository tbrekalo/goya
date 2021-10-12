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

  ~Window();

 private:
  struct GlfwBridge {
    auto ResizeCallback(std::int32_t const width, std::int32_t const height)
        -> void;

    auto CallKeyEventHandlers(std::int32_t const glfw_key_code,
                              std::int32_t const glfw_key_action) const -> void;

    auto CallCursorEventHandlers(double const xpos, double const ypos) -> void;

    std::int32_t width_;
    std::int32_t height_;

    std::vector<KeyEventHandler> key_handlers_;
    std::vector<CursorEventHandler> cursor_handlers_;
    std::vector<WinResizeEventHandler> win_resize_handlers_;
  };

  std::string title_;
  GLFWwindow* win_ptr_;

  GlfwBridge gb_;
};

}  // namespace goya
