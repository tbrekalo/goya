#pragma once

#include <memory>

#include "glm/glm.hpp"
#include "goya/events.hpp"
#include "goya/shader.hpp"

namespace goya {

class Camera {
 public:
  Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, glm::mat4 projection);

  auto AddShader(std::shared_ptr<Shader> shader) -> void;

  auto Refresh() -> void;

  auto MoveBack(TimeType delta) -> void;
  auto MoveFront(TimeType delta) -> void;
  auto MoveLeft(TimeType delta) -> void;
  auto MoveRight(TimeType delta) -> void;

  auto ShiftLook(float x_offset, float y_offset, TimeType delta) -> void;

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

  float speed_ = 12.5f;
  float sensitivity_ = 5.f;

  std::vector<std::shared_ptr<Shader>> shaders_;

  glm::mat4 projection_;
  glm::mat4 view_;
};

}  // namespace goya
