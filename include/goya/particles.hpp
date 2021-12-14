#pragma once

#include <memory>
#include <vector>

#include "glm/glm.hpp"
#include "goya/primitives.hpp"
#include "goya/shader.hpp"

namespace goya {

struct Particle {
  glm::vec3 position;
  glm::vec3 velocity;
  glm::vec4 color;
  float life_len;
};

class ParticleEffect {
 public:
  ParticleEffect(std::shared_ptr<Shader> shader, glm::vec3 const pos,
                 float const particle_life_span, std::size_t const size);

  ~ParticleEffect();

  auto Update(TimeType const delta) -> void;
  auto Render() -> void;

 private:
  auto UpdateLife(TimeType const delta) -> void;
  auto UpdatePositions(TimeType const delta) -> void;
  auto UpdateColor() -> void;
  auto Respawn() -> void;
  auto UpdateBuffers() -> void;

  std::shared_ptr<Shader> shader_;

  glm::vec3 pos_;

  float particle_life_span_;

  std::vector<Particle> particles_;
  decltype(particles_)::iterator live_particles_end_;

  std::vector<glm::vec3> pos_buffer_;
  std::vector<glm::vec4> color_buffer_;

  std::uint32_t vao_;
  std::uint32_t vbo_vertex_;
  std::uint32_t vbo_pos_;
  std::uint32_t vbo_color_;
};

}  // namespace goya
