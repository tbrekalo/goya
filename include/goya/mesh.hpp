#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "goya/shader.hpp"
#include "mesh_obj_data.hpp"

namespace goya {

class IDrawable {
 public:
  virtual auto Draw() -> void = 0;
  virtual ~IDrawable() = default;
};

class MeshConcept : public IDrawable {
 public:
  MeshConcept(std::shared_ptr<Shader> shader);

 protected:
  std::shared_ptr<Shader> shader_;
};

class MeshVbo : public MeshConcept {
 public:
  MeshVbo(std::shared_ptr<Shader> shader, MeshObjData obj_data);

  auto Draw() -> void override;

 private:
  std::size_t n_vertices_;

  std::uint32_t vao_;
  std::uint32_t vbo_;
};

}  // namespace goya
