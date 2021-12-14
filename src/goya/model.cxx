#include "goya/model.hpp"

namespace goya {

Model::Model(std::shared_ptr<Shader> shader,
             std::shared_ptr<IDrawable> drawable)
    : shader_(std::move(shader)), drawable_(std::move(drawable)) {}

auto Model::Rotate(float const degrees, glm::vec3 const axis) -> void {
  model_matrix_ = glm::rotate(model_matrix_, glm::radians(degrees), axis);
}

auto Model::Translate(glm::vec3 const vec) -> void {
  model_matrix_ = glm::translate(model_matrix_, vec);
}

auto Model::Scale(glm::vec3 const vec) -> void {
  model_matrix_ = glm::scale(model_matrix_, vec);
}

auto Model::SetModelMatrix(glm::mat4 model) -> void {
  model_matrix_ = std::move(model);
}

auto Model::SetColor(glm::vec3 color) -> void { color_ = color; }

auto Model::Draw() -> void {
  UpdateUniforms();
  drawable_->Draw();
}

auto Model::UpdateUniforms() -> void {
  shader_->Use();
  shader_->SetVec3("color", color_);
  shader_->SetMat4("model", model_matrix_);
}

}  // namespace goya
