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

  auto MoveBack() -> void;
  auto MoveFront() -> void;
  auto MoveLeft() -> void;
  auto MoveRight() -> void;

  auto ShiftLook(float x_offset, float y_offset) -> void;

  auto SetSpeed(float speed) noexcept -> void;
  auto GetSpeed() const noexcept -> float;

  auto SetSensitivity(float sensitivity) -> void;
  auto GetSensitivity() const noexcept -> float;

  auto UpdateAspectRatio(float ratio) -> void;

 protected:
  auto UpdateUniforms() const -> void;

  glm::vec3 pos_;
  glm::vec3 front_;
  glm::vec3 up_;
  glm::vec3 right_;

  float yaw_ = -90.f;
  float pitch_ = 0.f;

  float speed_ = 0.05f;
  float sensitivity_ = 0.05f;

  std::shared_ptr<Shader> shader_;

  glm::mat4 projection_;
  glm::mat4 view_;
};

}  // namespace goya
