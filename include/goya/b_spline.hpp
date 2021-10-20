#pragma once

#include <string>

#include "glm/glm.hpp"
#include "goya/model.hpp"
#include "goya/primitives.hpp"
#include "goya/shader.hpp"

namespace goya {

/* clang-format off */
inline auto const N_matrix = (1.f / 6.f) * glm::mat4{
   -1.f,  3.f, -3.f,  1.f, 
    3.f, -6.f,  0.f,  4.f, 
   -3.f,  3.f,  3.f,  1.f, 
    1.f,  0.f,  0.f,  0.f
};

auto LoadControloPoints(std::string const& path) -> std::vector<Vertex3d>;
auto LoadControloPoints(char const* path) -> std::vector<Vertex3d>;

class CubeBSpline {
public:
  CubeBSpline(std::vector<Vertex3d> control_points, std::shared_ptr<Shader> shader);
  
  auto TimeUpdate(TimeType delta) -> void;

  auto ModelMatrix() -> glm::mat4;

  auto SplineCoord(float t, std::uint32_t idx) -> Vertex3d;
  auto SplineDCoord(float t, std::uint32_t idx) -> Vertex3d;
  auto SplineDdCoord(float t, std::uint32_t idx) -> Vertex3d;

  auto SplineCoord() -> Vertex3d;
  auto SplineDCoord() -> Vertex3d;
  auto SplineDdCoord() -> Vertex3d;

  auto CenterCoord() -> Vertex3d;

  auto Draw() -> void;

private: 
  auto RiMatrix(std::uint32_t idx) -> glm::mat3x4;

  auto SplinePoints() -> std::vector<Vertex3d>;
  auto NormalPoints() -> std::vector<Vertex3d>;

  std::vector<Vertex3d> control_points_;

  Model control_model_;
  Model spline_model_;
  Model normal_model_;

  float animation_speed_ = 1.5f;
  float seg_t_ = 0.f;

  std::uint32_t curr_idx_ = 0;
  
};

/* clang-format on */

}  // namespace goya
