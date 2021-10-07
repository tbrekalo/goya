#include <stdexcept>
#include <iostream>
#include <cstdlib>

#include "goya/engine.hpp"

int main(void) {
  auto engine = goya::Engine(); 

  try {
    engine.Run();   
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_SUCCESS;
}
