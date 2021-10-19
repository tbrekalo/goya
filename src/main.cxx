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

int main(int argc, char** argv) {
  try {
    auto model_path = argv[1];

    auto win = goya::Window(1080, 720, "Goya");
    auto obj = goya::LoadMeshObjData(model_path);
    auto shader = std::make_shared<goya::Shader>("shaders/camera.vs",
                                                 "shaders/camera.fs");

    auto mesh = std::unique_ptr<goya::IMesh>(new goya::MeshVbo(obj));
    auto model = goya::Model(shader, std::move(mesh));

    shader->Use();
    auto projection =
        glm::perspective(glm::radians(90.f), win.AspectRatio(), 0.1f, 100.f);
    shader->SetMat4("projection", projection);

    auto camera =
        goya::Camera(glm::vec3(0.f, 0.f, 2.f), glm::vec3(0.f, 0.f, -1.f),
                     glm::vec3(0.f, 1.f, 0.f), shader, projection);

    win.AddWinResizeHandler([&](goya::ResizeEvent e) -> void {
      camera.UpdateAspectRatio(static_cast<float>(e.width) /
                               static_cast<float>(e.height));
    });

    win.AddKeyHandler([&](goya::KeyEvent e, goya::TimeType delta) -> void {
      if (e.glfw_key_action) {
        switch (e.glfw_key_code) {
          case GLFW_KEY_W:
            camera.MoveFront(delta);
            break;
          case GLFW_KEY_S:
            camera.MoveBack(delta);
            break;
          case GLFW_KEY_A:
            camera.MoveLeft(delta);
            break;
          case GLFW_KEY_D:
            camera.MoveRight(delta);
            break;
        }
      }
    });

    win.AddCursorHandler(
        [&camera, xy = win.CursorPosition()](
            goya::CursorEvent e, goya::TimeType delta) mutable -> void {
          camera.ShiftLook(static_cast<float>(e.x_pos - xy.first),
                           static_cast<float>(e.y_pos - xy.second), delta);

          xy = {e.x_pos, e.y_pos};
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
