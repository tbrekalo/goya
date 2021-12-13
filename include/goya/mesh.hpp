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

class MeshLines : public IMesh {
 public:
  MeshLines(std::vector<Vertex3d> const& points);
  ~MeshLines();

  auto DrawArrays() -> void override;

  private:
    std::size_t n_points_;

    std::uint32_t vao_;
    std::uint32_t vbo_;
};

class MeshTriangle : public IMesh {
 public:
  MeshTriangle(MeshObjData obj_data);
  ~MeshTriangle();

  auto DrawArrays() -> void override;

 private:
  std::size_t n_vertices_;

  std::uint32_t vao_;
  std::uint32_t vbo_;
};

}  // namespace goya
