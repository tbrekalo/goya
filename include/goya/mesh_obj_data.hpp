#pragma once

#include <vector>

#include "goya/primitives.hpp"

namespace goya {

struct MeshObjData {
  MeshObjData(std::vector<Vertex3d>&& vertices, std::vector<Face>&& faces)
      : vertices(std::move(vertices)), faces(std::move(faces)) {}

  std::vector<Vertex3d> vertices;
  std::vector<Face> faces;
};

}  // namespace goya
