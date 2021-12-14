#include "goya/particles.hpp"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <random>
#include <stdexcept>
#include <type_traits>

#include "GL/glew.h"

namespace goya {

namespace detail {

/* clang-format off */
auto constexpr kParticleMesh = {
  -0.5f, -0.5f,  0.0f,
   0.5f, -0.5f,  0.0f,
  -0.5f,  0.5f,  0.0f,
   0.5f,  0.5f,  0.0f,
};
/* clang-format on */

auto constexpr kDefaultColor = glm::vec4{1.0f, 0.66f, 0.66f, 1.0f};

auto GenerateRandomParticles(std::size_t const n_particles,
                             glm::vec3 const center) -> std::vector<Particle> {
  auto dst = std::vector<Particle>();
  dst.reserve(n_particles);

  auto rng_device = std::random_device();

  auto rng_speed = std::ranlux48_base(rng_device());
  auto rng_position = std::ranlux48_base(rng_device());

  auto xy_speed_dis = std::uniform_real_distribution<>(0.05, 0.33);
  auto z_speed_dis = std::uniform_real_distribution<>(0.33, 0.66);

  auto xy_pos_dis = std::uniform_real_distribution<>(0., 0.42);

  std::generate_n(std::back_inserter(dst), n_particles, [&]() -> Particle {
    auto dst = Particle();

    dst.position = center + glm::vec3{xy_pos_dis(rng_position),
                                      xy_pos_dis(rng_position), 0.};

    dst.velocity = {xy_speed_dis(rng_speed), xy_speed_dis(rng_speed),
                    z_speed_dis(rng_speed)};

    dst.color = kDefaultColor;

    dst.life_len = 0.f;

    return dst;
  });

  return dst;
}

}  // namespace detail

ParticleEffect::ParticleEffect(std::shared_ptr<Shader> shader,
                               glm::vec3 const pos,
                               float const particle_life_span,
                               std::size_t const size)
    : shader_(std::move(shader)),
      pos_(pos),
      particle_life_span_(particle_life_span),
      particles_(detail::GenerateRandomParticles(size, pos)),
      live_particles_end_(particles_.begin()),
      pos_buffer_(size),
      color_buffer_(size) {
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_vertex_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(detail::kParticleMesh),
               std::addressof(detail::kParticleMesh), GL_STATIC_DRAW);

  glGenBuffers(1, &vbo_pos_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_pos_);
  glBufferData(
      GL_ARRAY_BUFFER,
      pos_buffer_.capacity() * sizeof(decltype(pos_buffer_)::value_type),
      nullptr, GL_STREAM_DRAW);

  glGenBuffers(1, &vbo_color_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_color_);
  glBufferData(
      GL_ARRAY_BUFFER,
      color_buffer_.capacity() * sizeof(decltype(color_buffer_)::value_type),
      nullptr, GL_STREAM_DRAW);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_pos_);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_color_);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

  glVertexAttribDivisor(0, 0);
  glVertexAttribDivisor(1, 1);
  glVertexAttribDivisor(2, 1);

  glBindVertexArray(0);
}

ParticleEffect::~ParticleEffect() {
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vbo_vertex_);
  glDeleteBuffers(1, &vbo_pos_);
  glDeleteBuffers(1, &vbo_color_);
}

auto ParticleEffect::Update(TimeType const delta) -> void {
  UpdateLife(delta);
  UpdatePositions(delta);
  UpdateColor();
  Respawn();
  UpdateBuffers();
}

auto ParticleEffect::Render() -> void {
  if (particles_.begin() == live_particles_end_) {
    return;
  }

  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_pos_);
  glBufferData(
      GL_ARRAY_BUFFER,
      pos_buffer_.capacity() * sizeof(decltype(pos_buffer_)::value_type),
      nullptr, GL_STREAM_DRAW);
  glBufferSubData(
      GL_ARRAY_BUFFER, 0,
      pos_buffer_.size() * sizeof(decltype(pos_buffer_)::value_type),
      pos_buffer_.data());

  glBindBuffer(GL_ARRAY_BUFFER, vbo_color_);
  glBufferData(
      GL_ARRAY_BUFFER,
      color_buffer_.capacity() * sizeof(decltype(color_buffer_)::value_type),
      nullptr, GL_STREAM_DRAW);
  glBufferSubData(
      GL_ARRAY_BUFFER, 0,
      color_buffer_.size() * sizeof(decltype(color_buffer_)::value_type),
      color_buffer_.data());

  glBindVertexArray(vao_);
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 3,
                        std::distance(particles_.begin(), live_particles_end_));
  glBindVertexArray(0);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

auto ParticleEffect::UpdateLife(TimeType const delta) -> void {
  for (auto iter = particles_.begin(); iter != live_particles_end_; ++iter) {
    iter->life_len += delta;
    if (iter->life_len > particle_life_span_) {
      live_particles_end_ = std::prev(live_particles_end_);
      std::swap(*iter, *live_particles_end_);
    }
  }
}

auto ParticleEffect::UpdatePositions(TimeType const delta) -> void {
  for (auto iter = particles_.begin(); iter != live_particles_end_; ++iter) {
    iter->position += iter->velocity * delta;
  }
};

auto ParticleEffect::UpdateColor() -> void {
  for (auto iter = particles_.begin(); iter != live_particles_end_; ++iter) {
    iter->color.r = 1.f - (iter->life_len / particle_life_span_);
  }
}

auto ParticleEffect::Respawn() -> void {
  auto const max_respawns = std::min(
      std::distance(live_particles_end_, particles_.end()),
      static_cast<decltype(particles_)::difference_type>(static_cast<float>(
          std::round(particles_.size()) / std::max(2.f, particle_life_span_))));

  auto n_respawns = 0;
  while (live_particles_end_ != live_particles_end_ &&
         n_respawns < max_respawns) {
    live_particles_end_->position = pos_;
    live_particles_end_->color = detail::kDefaultColor;
    live_particles_end_->life_len = 0;

    ++live_particles_end_;
    ++n_respawns;
  }
}

auto ParticleEffect::UpdateBuffers() -> void {
  pos_buffer_.clear();
  std::transform(particles_.begin(), live_particles_end_,
                 std::back_inserter(pos_buffer_),
                 [](Particle const& p) -> glm::vec3 { return p.position; });

  color_buffer_.clear();
  std::transform(particles_.begin(), live_particles_end_,
                 std::back_inserter(pos_buffer_),
                 [](Particle const& p) -> glm::vec4 { return p.color; });
}

}  // namespace goya
