#include "goya/mesh.hpp"

#include <iostream>

#include "GL/glew.h"

namespace goya {

namespace detail {

auto TransformObjToVertices(MeshObjData const& obj) -> std::vector<Vertex3d> {
  auto dst = std::vector<Vertex3d>();

  for (auto const& face : obj.faces) {
    for (auto const idx : face) {
      dst.push_back(obj.vertices[idx - 1]);
    }
  }

  return dst;
};

}  // namespace detail

MeshTriangle::MeshTriangle(MeshObjData obj_data) {
  auto const vertices = detail::TransformObjToVertices(obj_data);
  n_vertices_ = vertices.size();

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);

  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);

  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizei>(sizeof(decltype(vertices)::value_type) *
                                    vertices.size()),
               vertices.data(), GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        sizeof(decltype(vertices)::value_type), nullptr);
  glEnableVertexAttribArray(0);

  // clean up
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

MeshTriangle::~MeshTriangle() {
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vbo_);
}

auto MeshTriangle::Draw() -> void {
  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLES, 0, static_cast<std::int32_t>(n_vertices_));
  glBindVertexArray(0);
}

MeshLines::MeshLines(std::vector<Vertex3d> const& points) {
  n_points_ = points.size();

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);

  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);

  glBufferData(
      GL_ARRAY_BUFFER,
      static_cast<GLsizei>(sizeof(std::decay_t<decltype(points)>::value_type) *
                           points.size()),
      points.data(), GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        sizeof(std::decay_t<decltype(points)>::value_type),
                        nullptr);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

MeshLines::~MeshLines() {
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vbo_);
}

auto MeshLines::Draw() -> void {
  glBindVertexArray(vao_);
  glDrawArrays(GL_LINE_STRIP, 0, static_cast<std::int32_t>(n_points_));
  glBindVertexArray(0);
}

}  // namespace goya
