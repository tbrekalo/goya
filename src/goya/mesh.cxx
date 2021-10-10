#include "goya/mesh.hpp"

#include "GL/glew.h"

namespace goya {

namespace detail {

auto TransformObjToVertices(MeshObjData const& obj) -> std::vector<Vertex3d> {
  auto dst = std::vector<Vertex3d>();

  for (auto const& face : obj.faces) {
    for (auto const idx : face) {
      dst.push_back(obj.vertices[idx]);
    }
  }

  return dst;
};

}  // namespace detail

MeshConcept::MeshConcept(std::shared_ptr<Shader> shader)
    : shader_(std::move(shader)) {}

VBOMesh::VBOMesh(std::shared_ptr<Shader> shader, MeshObjData obj_data)
    : MeshConcept(shader) {
  auto const vertices = detail::TransformObjToVertices(obj_data);
  n_vertices_ = vertices.size();

  glGenVertexArrays(GL_ARRAY_BUFFER, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizei>(sizeof(decltype(vertices)::value_type) *
                                    vertices.size()),
               vertices.data(), GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        3 * sizeof(decltype(vertices)::value_type::value_type),
                        nullptr);
  glEnableVertexAttribArray(0);

  // clean up
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

auto VBOMesh::Draw() -> void {
  this->shader_->Use();
  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLES, 0, static_cast<std::int32_t>(n_vertices_));
}

}  // namespace goya
