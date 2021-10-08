#pragma once

#include <cstdint>
#include <string>

/* clang-format off */
#include "glad/glad.h"
#include "GLFW/glfw3.h"
/* clang-format on */

namespace goya {

class Window {
 public:
  Window(std::int32_t width, std::int32_t height, std::string title);

  auto Refresh() -> bool;

  ~Window();

 private:
  std::string title_;
  GLFWwindow* win_ptr_;
};

}  // namespace goya
