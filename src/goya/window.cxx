#include "goya/window.hpp"

#include <stdexcept>
#include <type_traits>

namespace goya {

namespace detail {

auto ResizeCallback(GLFWwindow* win_ptr, std::int32_t width,
                    std::int32_t height) -> void {
  glViewport(0, 0, width, height);
}

}  // namespace detail

Window::Window(std::int32_t width, std::int32_t height, std::string title)
    : width_(width), height_(height), title_(std::move(title)) {
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

  glfwSetWindowUserPointer(win_ptr_, &geh_);

  glfwMakeContextCurrent(win_ptr_);
  if (glewInit()) {
    throw std::runtime_error("[goya::Window] failed to initialize glew.");
  }

  auto const key_callback_lambda =
      [](GLFWwindow* win_ptr, std::int32_t key, std::int32_t scancode,
         std::int32_t action, std::int32_t mods) -> void {
    static_cast<GlfwEventHandler*>(glfwGetWindowUserPointer(win_ptr))
        ->CallKeyEventHandlers(key, action);
  };

  // set callbacks
  glfwSetFramebufferSizeCallback(win_ptr_, detail::ResizeCallback);
  glfwSetKeyCallback(win_ptr_, key_callback_lambda);

  glEnable(GL_DEPTH_TEST);
}

auto Window::Refresh() -> bool {
  glfwSwapBuffers(win_ptr_);
  glfwPollEvents();

  glfwGetWindowSize(win_ptr_, &width_, &height_);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  return !glfwWindowShouldClose(win_ptr_);
}

auto Window::Width() const noexcept -> std::int32_t { return width_; }

auto Window::Height() const noexcept -> std::int32_t { return height_; }

auto Window::AspectRatio() const noexcept -> float {
  return static_cast<float>(width_) / static_cast<float>(height_);
}

auto Window::Title() const -> std::string const& { return title_; }

auto Window::WinPtr() -> GLFWwindow* { return win_ptr_; }

auto Window::AddKeyHandler(KeyEventHandler key_handler) -> void {
  geh_.key_handlers_.push_back(std::move(key_handler));
}

Window::~Window() {
  glfwDestroyWindow(win_ptr_);
  glfwTerminate();
}

auto Window::GlfwEventHandler::CallKeyEventHandlers(
    std::int32_t const glfw_key_code, std::int32_t const glfw_key_action) const
    -> void {
  for (auto const& key_handler : key_handlers_) {
    key_handler(glfw_key_code, glfw_key_action);
  }
}

}  // namespace goya
