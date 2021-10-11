#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "goya/mesh.hpp"
#include "goya/mesh_loader.hpp"
#include "goya/model.hpp"
#include "goya/shader.hpp"
#include "goya/window.hpp"

int main(void) {
  try {
    auto win = goya::Window(1080, 720, "Goya");
    auto obj = goya::LoadMeshObjData("resources/mesh/triangle.obj");
    auto shader =
        std::make_shared<goya::Shader>("shaders/camera.vs", "shaders/basic.fs");

    auto mesh = std::unique_ptr<goya::IMesh>(new goya::MeshVbo(obj));
    auto model = goya::Model(shader, std::move(mesh));

    model.Scale(glm::vec3(0.5f, 0.5f, 0.5f));

    while (win.Refresh()) {
      model.Draw(); 
    }

  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_SUCCESS;
}
