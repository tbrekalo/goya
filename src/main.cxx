#include <stdexcept>
#include <iostream>
#include <cstdlib>

#include "goya/window.hpp"
#include "goya/engine.hpp"

int main(void) {

  try {
    auto win = goya::Window(1080, 720, "Goya");

    while (win.Refresh()) {}

  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_SUCCESS;
}
