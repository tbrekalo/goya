#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "goya/mesh.hpp"
#include "goya/mesh_loader.hpp"
#include "goya/shader.hpp"
#include "goya/window.hpp"

// use GLM stuff
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main(void) {
  try {
    auto win = goya::Window(1080, 720, "Goya");
    auto obj = goya::LoadMeshObjData("resources/mesh/triangle.obj");
    auto shader =
        std::make_shared<goya::Shader>("shaders/camera.vs", "shaders/basic.fs");

    auto mesh = goya::MeshVbo(shader, obj);

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::scale(transform, glm::vec3(0.5f, 0.5f, 0.5f));

    shader->Use();
    shader->SetMat4("transform", transform);

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
