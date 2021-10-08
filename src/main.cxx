#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "goya/engine.hpp"
#include "goya/window.hpp"

int main(void) {
  try {
    auto win = goya::Window(1080, 720, "Goya");

    while (win.Refresh()) {
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
    }

  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_SUCCESS;
}
