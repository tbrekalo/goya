#include "goya/camera.hpp"

#include "glm/gtc/matrix_transform.hpp"

namespace goya {

namespace detail {
auto constexpr up = glm::vec3(0.f, 1.f, 0.f);
}

Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up,
               std::shared_ptr<Shader> shader, glm::mat4 projection)
    : pos_(pos),
      front_(front),
      up_(up),
      shader_(std::move(shader)),
      projection_(projection) {}

auto Camera::Refresh() -> void {
  view_ = glm::lookAt(pos_, front_, detail::up);

  UpdateUniforms();
}

auto Camera::UpdateUniforms() const -> void {
  shader_->Use();

  shader_->SetMat4("projection", projection_);
  shader_->SetMat4("view", view_);
}

UserCamera::UserCamera(glm::vec3 pos, glm::vec3 front, glm::vec3 up,
                       std::shared_ptr<Shader> shader, glm::mat4 projection)
    : Camera(pos, front, up, shader, projection) {}

auto UserCamera::MoveBack() -> void { pos_ -= speed_ * front_; }

auto UserCamera::MoveFront() -> void { pos_ += speed_ * front_; }

auto UserCamera::MoveLeft() -> void {
  pos_ -= speed_ * glm::normalize(glm::cross(front_, up_));
}

auto UserCamera::MoveRight() -> void {
  pos_ += speed_ * glm::normalize(glm::cross(front_, up_));
}

auto UserCamera::SetSpeed(float speed) noexcept -> void { speed_ = speed; }

auto UserCamera::GetSpeed() const noexcept -> float { return speed_; }

}  // namespace goya
