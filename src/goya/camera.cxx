#include "goya/camera.hpp"

#include <cmath>
#include <utility>

#include "glm/gtc/matrix_transform.hpp"

namespace goya {

namespace detail {
auto constexpr kWorldUp = glm::vec3(0.f, 1.f, 0.f);
}

Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up,
               std::shared_ptr<Shader> shader, glm::mat4 projection)
    : pos_(pos),
      front_(front),
      up_(up),
      right_(glm::normalize(glm::cross(front_, up_))),
      shader_(std::move(shader)),
      projection_(projection) {}

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
  shader_->Use();

  shader_->SetMat4("projection", projection_);
  shader_->SetMat4("view", view_);
}

auto Camera::MoveBack() -> void { pos_ -= speed_ * front_; }

auto Camera::MoveFront() -> void { pos_ += speed_ * front_; }

auto Camera::MoveLeft() -> void { pos_ -= speed_ * right_; }

auto Camera::MoveRight() -> void { pos_ += speed_ * right_; }

auto Camera::ShiftLook(float x_offset, float y_offset) -> void {
  x_offset *= sensitivity_;
  y_offset *= sensitivity_;

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
