#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "goya/camera.hpp"
#include "goya/mesh.hpp"
#include "goya/mesh_loader.hpp"
#include "goya/model.hpp"
#include "goya/shader.hpp"
#include "goya/window.hpp"

int main(void) {
  try {
    auto win = goya::Window(1080, 720, "Goya");
    auto obj = goya::LoadMeshObjData("resources/mesh/teddy.obj");
    auto shader = std::make_shared<goya::Shader>("shaders/camera.vs",
                                                 "shaders/camera.fs");

    auto mesh = std::unique_ptr<goya::IMesh>(new goya::MeshVbo(obj));
    auto model = goya::Model(shader, std::move(mesh));

    model.Scale(glm::vec3(0.5f, 0.5f, 0.5f));
    model.Rotate(45, {0.f, 1.f, 1.f});
    shader->Use();

    auto projection =
        glm::perspective(glm::radians(55.f), win.AspectRatio(), 0.1f, 100.f);
    shader->SetMat4("projection", projection);

    auto camera =
        goya::UserCamera(glm::vec3(3.f, 0.f, 3.f), glm::vec3(0.f, 0.f, -1.f),
                         glm::vec3(0.f, 1.f, 0.f), shader, projection);

    win.AddKeyHandler([&](std::int32_t const glfw_key_code,
                          std::int32_t glfw_key_action) -> void {
      if (glfw_key_action) {
        switch (glfw_key_code) {
          case GLFW_KEY_W:
            camera.MoveFront();
            break;
          case GLFW_KEY_S:
            camera.MoveBack();
            break;
          case GLFW_KEY_A:
            camera.MoveLeft();
            break;
          case GLFW_KEY_D:
            camera.MoveRight();
            break;
        }
      }
    });

    while (win.Refresh()) {
      camera.Refresh();
      model.Draw();
    }

  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_SUCCESS;
}
