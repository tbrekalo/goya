#include "goya/window.hpp"

#include <stdexcept>

namespace goya {

namespace detail {

auto ResizeCallback(GLFWwindow* win_ptr, std::int32_t width,
                    std::int32_t height) -> void {
  glViewport(0, 0, width, height);
}

}  // namespace detail

Window::Window(std::int32_t width, std::int32_t height, std::string title)
    : title_(std::move(title)) {

  glewExperimental = true; // core profile
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
    throw std::runtime_error(
        "[goya::Window] failed to create glfw window.");
  }

  glfwMakeContextCurrent(win_ptr_);
  glfwSetFramebufferSizeCallback(win_ptr_, detail::ResizeCallback);
  if (glewInit()) {
    throw std::runtime_error("[goya::Window] failed to initialize glew.");
  }
}

auto Window::Refresh() -> bool {
  glfwSwapBuffers(win_ptr_);
  glfwPollEvents();

  // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  // glClear(GL_COLOR_BUFFER_BIT);

  return !glfwWindowShouldClose(win_ptr_);
}

Window::~Window() {
  glfwDestroyWindow(win_ptr_);
  glfwTerminate();
}

}  // namespace goya
