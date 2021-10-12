#pragma once

#include <memory>

#include "glm/glm.hpp"
#include "goya/events.hpp"
#include "goya/shader.hpp"

namespace goya {

class Camera {
 public:
  Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up,
         std::shared_ptr<Shader> shader, glm::mat4 projection);

  auto Refresh() -> void;

 protected:
  auto UpdateUniforms() const -> void;

  glm::vec3 pos_;
  glm::vec3 front_;
  glm::vec3 up_;

  std::shared_ptr<Shader> shader_;

  glm::mat4 projection_;
  glm::mat4 view_;
};

class UserCamera : public Camera {
 public:

  UserCamera(glm::vec3 pos, glm::vec3 front, glm::vec3 up,
             std::shared_ptr<Shader> shader, glm::mat4 projection);

  auto MoveBack() -> void;
  auto MoveFront() -> void;
  auto MoveLeft() -> void;
  auto MoveRight() -> void;

 private:

  auto SetSpeed(float speed) noexcept -> void;
  auto GetSpeed() const noexcept -> float;

  float speed_ = 0.05f;
};

}  // namespace goya
