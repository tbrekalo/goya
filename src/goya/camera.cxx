#include "goya/camera.hpp"

#include <cmath>
#include <utility>

#include "glm/gtc/matrix_transform.hpp"

namespace goya {

namespace detail {
auto constexpr kWorldUp = glm::vec3(0.f, 1.f, 0.f);
}

Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up,
               glm::mat4 projection)
    : pos_(pos),
      front_(front),
      up_(up),
      right_(glm::normalize(glm::cross(front_, up_))),
      projection_(projection) {}

auto Camera::AddShader(std::shared_ptr<Shader> shader) -> void {
  shaders_.push_back(std::move(shader));
};

auto Camera::Refresh() -> void {
  front_.x = std::cos(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
  front_.y = std::sin(glm::radians(pitch_));
  front_.z = std::sin(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));

  front_ = glm::normalize(front_);
  right_ = glm::normalize(glm::cross(front_, detail::kWorldUp));
  up_ = glm::normalize(glm::cross(right_, front_));

  view_ = glm::lookAt(pos_, pos_ + front_, up_);
  UpdateUniforms();
}

auto Camera::UpdateUniforms() const -> void {
  for (auto& shader : shaders_) {
    shader->Use();

    shader->SetMat4("projection", projection_);
    shader->SetMat4("view", view_);
  }
}

auto Camera::MoveBack(TimeType delta) -> void {
  pos_ -= delta * speed_ * front_;
}

auto Camera::MoveFront(TimeType delta) -> void {
  pos_ += delta * speed_ * front_;
}

auto Camera::MoveLeft(TimeType delta) -> void {
  pos_ -= delta * speed_ * right_;
}

auto Camera::MoveRight(TimeType delta) -> void {
  pos_ += delta * speed_ * right_;
}

auto Camera::ShiftLook(float x_offset, float y_offset, TimeType delta) -> void {
  x_offset *= delta * sensitivity_;
  y_offset *= delta * sensitivity_;

  yaw_ += x_offset;
  pitch_ += y_offset;

  pitch_ = std::min(std::max(pitch_, -89.f), 89.f);
}

auto Camera::SetSpeed(float speed) noexcept -> void { speed_ = speed; }

auto Camera::GetSpeed() const noexcept -> float { return speed_; }

auto Camera::SetSensitivity(float sensitivity) -> void {
  sensitivity_ = sensitivity;
}

auto Camera::GetSensitivity() const noexcept -> float { return sensitivity_; }

auto Camera::UpdateAspectRatio(float ratio) -> void {
  projection_ = glm::perspective(glm::radians(55.f), ratio, 0.1f, 100.f);
}

}  // namespace goya
