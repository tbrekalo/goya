#include "goya/window.hpp"

#include <stdexcept>

namespace goya {

Window::Window(std::int32_t width, std::int32_t height, std::string title)
    : width_(width), height_(height), title_(std::move(title)) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  win_ptr_ =
      glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);

  if (win_ptr_ == nullptr) {
    throw std::runtime_error(
        "[goya::Window] failed to initialzie glfw window.");
  }

  glfwMakeContextCurrent(win_ptr_);
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    throw std::runtime_error("[goya::Window] failed to initialize GLAD.");
  }

  glViewport(0, 0, width_, height_);
}

auto Window::Refresh() -> bool {
  glfwSwapBuffers(win_ptr_);
  glfwPollEvents();

  return !glfwWindowShouldClose(win_ptr_);
}

Window::~Window() {
  glfwDestroyWindow(win_ptr_);
  glfwTerminate();
}

}  // namespace goya
