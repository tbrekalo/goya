#pragma once

#include <cstdint>
#include <vector>

#include "goya/shader.hpp"
#include "goya/drawable.hpp"
#include "goya/mesh_obj_data.hpp"

namespace goya {

class IMesh : public IDrawable {};

class MeshLines : public IMesh {
 public:
  MeshLines(std::vector<Vertex3d> const& points);
  ~MeshLines();

  auto Draw() -> void override;

  private:
    std::size_t n_points_;

    std::uint32_t vao_;
    std::uint32_t vbo_;
};

class MeshTriangle : public IMesh {
 public:
  MeshTriangle(MeshObjData obj_data);
  ~MeshTriangle();

  auto Draw() -> void override;

 private:
  std::size_t n_vertices_;

  std::uint32_t vao_;
  std::uint32_t vbo_;
};

}  // namespace goya
