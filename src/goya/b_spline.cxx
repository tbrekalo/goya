#include "goya/b_spline.hpp"

#include <cmath>
#include <fstream>

#include "GL/glew.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace goya {

auto LoadControloPoints(std::string const& path) -> std::vector<Vertex3d> {
  return LoadControloPoints(path.c_str());
}

auto LoadControloPoints(char const* path) -> std::vector<Vertex3d> {
  auto dst = std::vector<Vertex3d>();
  auto ifstrm = std::ifstream(path);
  auto v = Vertex3d();
  while (ifstrm >> v.x >> v.y >> v.z) {
    dst.push_back(v);
  }

  return dst;
}

CubeBSpline::CubeBSpline(std::vector<Vertex3d> control_points,
                         std::shared_ptr<Shader> shader)
    : control_points_(std::move(control_points)),
      control_model_(shader, std::make_unique<MeshLines>(control_points_)),
      spline_model_(shader, std::make_unique<MeshLines>(SplinePoints())),
      normal_model_(shader, std::make_unique<MeshLines>(NormalPoints())) {
  control_model_.SetColor(glm::vec3{1.0f, 0.f, 0.f});
  spline_model_.SetColor(glm::vec3(0.f, 0.5f, 0.5f));
  normal_model_.SetColor(glm::vec3(0.1f, 0.1f, 0.1f));
}

auto CubeBSpline::TimeUpdate(TimeType delta) -> void {
  seg_t_ += delta * animation_speed_;
  if (seg_t_ >= 1.f) {
    seg_t_ -= 1.f;
    curr_idx_ = (curr_idx_ + 1) % (control_points_.size() - 3);
  }
}

auto CubeBSpline::ModelMatrix() -> glm::mat4 {
  auto model = glm::mat4(1.f);

  model = glm::translate(model, SplineCoord());
  // auto const s = glm::vec3{0.f, 0.f, 1.f};
  // auto const e = glm::normalize(SplineDCoord());

  // auto const axis = glm::normalize(glm::cross(s, e));
  // auto const deg =
  //     std::acos(glm::dot(s, e) / (glm::length(s) * glm::length(e)));

  // return glm::rotate(model, deg, axis);

  auto const v = glm::normalize(SplineDCoord());    // z
  auto const u = glm::normalize(SplineDdCoord());   // y
  auto const w = glm::normalize(glm::cross(u, v));  // x

  /* clang-format off */
  auto const dcm = glm::mat4{
    w.x, w.y, w.z, 0,
    u.x, u.y, u.z, 0,
    v.x, v.y, v.z, 0,
    0,   0,   0,   1
  };
  /* clnag-format on */

  return model * dcm;
}

auto CubeBSpline::SplineCoord() -> Vertex3d {
  return SplineCoord(seg_t_, curr_idx_);
}

auto CubeBSpline::SplineCoord(float t, std::uint32_t idx) -> Vertex3d {
  return glm::vec4{t * t * t, t * t, t, 1} * N_matrix * RiMatrix(idx);
}

auto CubeBSpline::SplineDCoord() -> Vertex3d {
  return SplineDCoord(seg_t_, curr_idx_);
}

auto CubeBSpline::SplineDCoord(float t, std::uint32_t idx) -> Vertex3d {
  return glm::vec4{3 * t * t, 2 * t, 1, 0} * N_matrix * RiMatrix(idx);
}

auto CubeBSpline::SplineDdCoord() -> Vertex3d {
  return SplineDdCoord(seg_t_, curr_idx_);
}

auto CubeBSpline::SplineDdCoord(float t, std::uint32_t idx) -> Vertex3d {
  return glm::vec4{6 * t, 2, 0, 0} * N_matrix * RiMatrix(idx);
}

auto CubeBSpline::CenterCoord() -> Vertex3d {
  auto dst = Vertex3d{0.f, 0.f, 0.f};

  for (auto const cp : control_points_) {
    dst += cp;
  }

  dst /= control_points_.size();

  return dst;
}

auto CubeBSpline::Draw() -> void {
  control_model_.Draw();
  spline_model_.Draw();
  normal_model_.Draw();
}

auto CubeBSpline::RiMatrix(std::uint32_t idx) -> glm::mat3x4 {
  auto R = glm::mat3x4{};
  for (auto axis = 0; axis < R.length(); ++axis) {
    for (auto i = 0U; i < 4U; ++i) {
      R[axis][static_cast<GLint>(i)] = control_points_[idx + i][axis];
    }
  }

  return R;
}

auto CubeBSpline::SplinePoints() -> std::vector<Vertex3d> {
  auto dst = std::vector<Vertex3d>();
  for (auto idx = 0U; idx < control_points_.size() - 3; ++idx) {
    for (auto seg_t = 0.f; seg_t < 1.f; seg_t += 0.01f) {
      dst.push_back(SplineCoord(seg_t, idx));
    }
  }

  return dst;
}

auto CubeBSpline::NormalPoints() -> std::vector<Vertex3d> {
  auto dst = std::vector<Vertex3d>();
  for (auto idx = 0U; idx < control_points_.size() - 3; ++idx) {
    for (auto seg_t = 0.f; seg_t < 1.f; seg_t += 0.1f) {
      auto const spline_coord = SplineCoord(seg_t, idx);
      auto const normal_dir = glm::normalize(SplineDdCoord(seg_t, idx));

      dst.push_back(spline_coord);
      dst.push_back(spline_coord + normal_dir);
    }
  }

  return dst;
}

}  // namespace goya
