#pragma once

#include <memory>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "goya//drawable.hpp"
#include "goya/mesh.hpp"

namespace goya {

class Model : public IDrawable {
 public:
  Model(std::shared_ptr<Shader> shader, std::unique_ptr<IMesh> mesh);

  auto Rotate(float const degrees, glm::vec3 const axis) -> void;
  auto Translate(glm::vec3 const vec) -> void;
  auto Scale(glm::vec3 const vec) -> void;

  auto Draw() -> void override;

 private:
  auto UpdateUniforms() -> void;

  glm::mat4 model_matrix_ = glm::mat4(1.f);

  std::shared_ptr<Shader> shader_;
  std::unique_ptr<IMesh> mesh_;
};

}  // namespace goya