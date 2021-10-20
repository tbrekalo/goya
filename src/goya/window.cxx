#include "goya/window.hpp"

#include <stdexcept>
#include <type_traits>

namespace goya {

Window::Window(std::int32_t width, std::int32_t height, std::string title)
    : title_(std::move(title)) {
  gb_.width_ = width;
  gb_.height_ = height;

  glewExperimental = true;  // core profile
  if (!glfwInit()) {
    throw std::runtime_error("[goya::Window] failed to initialize glfw");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  win_ptr_ = glfwCreateWindow(width, height, title_.c_str(), nullptr, nullptr);

  if (win_ptr_ == nullptr) {
    throw std::runtime_error("[goya::Window] failed to create glfw window.");
  }

  glfwMakeContextCurrent(win_ptr_);
  if (glewInit()) {
    throw std::runtime_error("[goya::Window] failed to initialize glew.");
  }

  glfwSetInputMode(win_ptr_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glEnable(GL_DEPTH_TEST);

  // set callbacks
  glfwSetWindowUserPointer(win_ptr_, &gb_);

  auto const resize_callback_lambda =
      [](GLFWwindow* win_ptr, std::int32_t width, std::int32_t height) -> void {
    static_cast<GlfwBridge*>(glfwGetWindowUserPointer(win_ptr))
        ->ResizeCallback(width, height);
  };

  auto const key_callback_lambda =
      [](GLFWwindow* win_ptr, std::int32_t key, std::int32_t scancode,
         std::int32_t action, std::int32_t mods) -> void {
    static_cast<GlfwBridge*>(glfwGetWindowUserPointer(win_ptr))
        ->key_events_.push_back(KeyEvent(key, action));
  };

  auto const cursor_callback_lambda = [](GLFWwindow* win_ptr, double xpos,
                                         double ypos) -> void {
    static_cast<GlfwBridge*>(glfwGetWindowUserPointer(win_ptr))
        ->cursor_events_.push_back(CursorEvent(xpos, ypos));
  };

  glfwSetFramebufferSizeCallback(win_ptr_, resize_callback_lambda);
  glfwSetKeyCallback(win_ptr_, key_callback_lambda);
  glfwSetCursorPosCallback(win_ptr_, cursor_callback_lambda);

  prev_refresh_ = glfwGetTime();
}

auto Window::Refresh() -> bool {
  glfwSwapBuffers(win_ptr_);
  glfwPollEvents();

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  auto const curr_time = glfwGetTime();
  auto const delta = static_cast<float>(curr_time - prev_refresh_);

  gb_.CallKeyEventHandlers(delta);
  gb_.CallCursorEventHandlers(delta);
  gb_.CallAnimationHandlers(delta);

  prev_refresh_ = curr_time;

  return !glfwWindowShouldClose(win_ptr_);
}

auto Window::Width() const noexcept -> std::int32_t { return gb_.width_; }

auto Window::Height() const noexcept -> std::int32_t { return gb_.height_; }

auto Window::AspectRatio() const noexcept -> float {
  return static_cast<float>(gb_.width_) / static_cast<float>(gb_.height_);
}

auto Window::CursorPosition() const -> std::pair<double, double> {
  double x_pos, y_pos;
  glfwGetCursorPos(win_ptr_, &x_pos, &y_pos);

  return {x_pos, y_pos};
}

auto Window::Title() const -> std::string const& { return title_; }

auto Window::WinPtr() -> GLFWwindow* { return win_ptr_; }

auto Window::AddKeyHandler(KeyEventHandler key_handler) -> void {
  gb_.key_handlers_.push_back(std::move(key_handler));
}

auto Window::AddCursorHandler(CursorEventHandler mouse_handler) -> void {
  gb_.cursor_handlers_.push_back(std::move(mouse_handler));
}

auto Window::AddWinResizeHandler(WinResizeEventHandler win_resize_handlers)
    -> void {
  gb_.win_resize_handlers_.push_back(std::move(win_resize_handlers));
}

auto Window::AddAnimationHandler(AnimationHandler animation_handler) -> void {
  gb_.animation_handlers_.push_back(std::move(animation_handler));
}

Window::~Window() {
  glfwDestroyWindow(win_ptr_);
  glfwTerminate();
}

auto Window::GlfwBridge::ResizeCallback(std::int32_t const width,
                                        std::int32_t const height) -> void {
  width_ = width;
  height_ = height;

  for (auto const& win_resize_handler : win_resize_handlers_) {
    win_resize_handler(ResizeEvent(width, height));
  }
}

auto Window::GlfwBridge::CallKeyEventHandlers(TimeType const delta) -> void {
  for (auto const& key_handler : key_handlers_) {
    for (auto const key_event : key_events_) {
      key_handler(key_event, delta);
    }
  }

  key_events_.clear();
}

auto Window::GlfwBridge::CallCursorEventHandlers(TimeType const delta) -> void {
  for (auto const& cursor_handler : cursor_handlers_) {
    for (auto const cursor_event : cursor_events_) {
      cursor_handler(cursor_event, delta);
    }
  }

  cursor_events_.clear();
}

auto Window::GlfwBridge::CallAnimationHandlers(TimeType const delta) -> void {
  for (auto const& animation_handler : animation_handlers_) {
    animation_handler(delta);
  }
}

}  // namespace goya
