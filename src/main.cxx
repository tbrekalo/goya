#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>


#include "goya/mesh.hpp"
#include "goya/mesh_loader.hpp"
#include "goya/shader.hpp"
#include "goya/window.hpp"

int main(void) {
  try {
    auto win = goya::Window(1080, 720, "Goya");
    auto obj = goya::LoadMeshObjData("resources/mesh/triangle.obj");
    auto shader =
        std::make_shared<goya::Shader>("shaders/basic.vs", "shaders/basic.fs");

    auto mesh = goya::MeshVbo(shader, obj);

    while (win.Refresh()) {
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      mesh.Draw();
    }

  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_SUCCESS;
}
