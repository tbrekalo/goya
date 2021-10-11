#include "goya/model.hpp"

namespace goya {

Model::Model(std::shared_ptr<Shader> shader, std::unique_ptr<IMesh> mesh)
    : shader_(std::move(shader)), mesh_(std::move(mesh)) {}

auto Model::Rotate(float const degrees, glm::vec3 const axis) -> void {
  model_matrix_ = glm::rotate(model_matrix_, glm::radians(degrees), axis);
}

auto Model::Translate(glm::vec3 const vec) -> void {
  model_matrix_ = glm::translate(model_matrix_, vec);
}

auto Model::Scale(glm::vec3 const vec) -> void {
  model_matrix_ = glm::scale(model_matrix_, vec);
}

auto Model::Draw() -> void { 
  UpdateUniforms(); 
  mesh_->DrawArrays(); 
}

auto Model::UpdateUniforms() -> void {
  shader_->Use();
  shader_->SetMat4("model", model_matrix_);
}

}  // namespace goya
