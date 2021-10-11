#pragma once

#include <cstdint>
#include <vector>

#include "goya/shader.hpp"
#include "mesh_obj_data.hpp"

namespace goya {

class IMesh {
 public:
  virtual auto DrawArrays() -> void = 0;
  virtual ~IMesh() = default;
};

class MeshVbo : public IMesh {
 public:
  MeshVbo(MeshObjData obj_data);

  auto DrawArrays() -> void override;

 private:
  std::size_t n_vertices_;

  std::uint32_t vao_;
  std::uint32_t vbo_;
};

}  // namespace goya
