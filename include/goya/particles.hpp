#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "glm/glm.hpp"
#include "goya/mesh.hpp"
#include "goya/primitives.hpp"
#include "goya/shader.hpp"

namespace goya {

struct Particle {
  glm::vec3 position;
  glm::vec3 velocity;
  glm::vec4 color;
  float life_len;
};

class ParticleEffect : public IDrawable {
 public:
  ParticleEffect(std::shared_ptr<Shader> shader,
                std::function<Particle(void)> particle_src,
                float const particle_life_span,
                 std::size_t const size);

  ~ParticleEffect();

  auto Update(TimeType const delta) -> void;
  auto Draw() -> void override;

  auto SetScale(glm::mat4 const scale_matrix) -> void;

 private:
  auto UpdateLife(TimeType const delta) -> void;
  auto UpdatePositionsBuffer() -> void;
  auto UpdateColorBuffer() -> void;
  auto Respawn(TimeType const delta) -> void;
  auto UpdateBuffers() -> void;

  std::shared_ptr<Shader> shader_;
  std::function<Particle(void)> particle_src_;

  float particle_life_span_;
  float respawn_units_;

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
