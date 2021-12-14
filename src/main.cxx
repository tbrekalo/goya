#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "goya/b_spline.hpp"
#include "goya/camera.hpp"
#include "goya/mesh.hpp"
#include "goya/mesh_loader.hpp"
#include "goya/model.hpp"
#include "goya/particles.hpp"
#include "goya/shader.hpp"
#include "goya/window.hpp"

int main(int argc, char** argv) {
  try {
    auto win = goya::Window(1080, 720, "Goya");
    auto particle_shader = std::make_shared<goya::Shader>(
        "shaders/particle.vs", "shaders/particle.fs");

    auto particle_effect = std::make_unique<goya::ParticleEffect>(
        particle_shader, glm::vec3{0.f, 0.f, 0.f}, 2.f, 100);

    auto camera_pos = goya::Vertex3d(7.f, 3.33f, 7.f);

    auto projection =
        glm::perspective(glm::radians(90.f), win.AspectRatio(), 0.1f, 200.f);

    auto camera = goya::Camera(camera_pos, -glm::normalize(camera_pos),
                               glm::vec3(0.f, 1.f, 0.f), projection);

    camera.AddShader(particle_shader);

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

    win.AddAnimationHandler(
        [&](goya::TimeType delta) -> void { particle_effect->Update(delta); });

    while (win.Refresh()) {
      particle_effect->Render();
      camera.Refresh();
    }

  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_SUCCESS;
}
