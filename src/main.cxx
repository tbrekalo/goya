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
    if (argc != 3) {
      throw std::runtime_error(
          "[goya] please supply two command line arguments. Path to model, "
          "path to spline control points");
    }

    auto model_path = argv[1];
    auto spline_path = argv[2];

    auto win = goya::Window(1080, 720, "Goya");
    auto obj = goya::LoadMeshObjData(model_path);
    auto model_shader =
        std::make_shared<goya::Shader>("shaders/model.vs", "shaders/model.fs");

    auto particle_shader = std::make_shared<goya::Shader>(
        "shaders/particle.vs", "shaders/particle.fs");

    auto particle_effect = std::make_unique<goya::ParticleEffect>(
        particle_shader, glm::vec3{0.f, 0.f, 0.f}, 3.f, 1000);

    auto mesh = std::make_unique<goya::MeshTriangle>(obj);
    auto model = goya::Model(model_shader, std::move(mesh));

    auto spline =
        goya::CubeBSpline(goya::LoadControloPoints(spline_path), model_shader);

    auto spline_center = spline.CenterCoord();
    auto camera_pos = goya::Vertex3d(10.f, 2.5f, 15.f);

    model_shader->Use();
    auto projection =
        glm::perspective(glm::radians(90.f), win.AspectRatio(), 0.1f, 200.f);
    model_shader->SetMat4("projection", projection);

    auto camera = goya::Camera(camera_pos, glm::normalize(spline_center),
                               glm::vec3(0.f, 1.f, 0.f), projection);

    camera.AddShader(model_shader);
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

    win.AddAnimationHandler([&](goya::TimeType delta) -> void {
      particle_effect->Update(delta);
      spline.TimeUpdate(delta);
      model.SetModelMatrix(spline.ModelMatrix());
    });

    while (win.Refresh()) {
      particle_effect->Render();
      spline.Draw();
      model.Draw();

      camera.Refresh();
    }

  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_SUCCESS;
}
