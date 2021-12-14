#include "goya/particles.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <iterator>
#include <limits>
#include <random>
#include <stdexcept>
#include <type_traits>

#include "GL/glew.h"

namespace goya {

namespace detail {

/* clang-format off */
auto constexpr kParticleMesh = std::array<float, 12>{
  -0.5f, -0.5f,  0.0f,
   0.5f, -0.5f,  0.0f,
  -0.5f,  0.5f,  0.0f,
   0.5f,  0.5f,  0.0f,
};
/* clang-format on */

auto constexpr kDefaultColor = glm::vec4{0.33f, 0.66f, 0.66f, 0.2f};

auto GenerateRandomParticles(std::size_t const n_particles,
                             glm::vec3 const center) -> std::vector<Particle> {
  auto dst = std::vector<Particle>();
  dst.reserve(n_particles);

  auto rng_device = std::random_device();

  auto rng_speed = std::ranlux48_base(rng_device());
  auto rng_position = std::ranlux48_base(rng_device());

  auto xz_speed_dis = std::uniform_real_distribution<>(-2.2f, 3.14f);
  auto y_speed_dis = std::uniform_real_distribution<>(16.9f, 24.f);

  auto xz_pos_dis = std::uniform_real_distribution<>(-0.5f, 0.5f);

  std::generate_n(std::back_inserter(dst), n_particles, [&]() -> Particle {
    auto dst = Particle();

    dst.position = center + glm::vec3{xz_pos_dis(rng_position), 0.f,
                                      xz_pos_dis(rng_position)};

    dst.velocity = {xz_speed_dis(rng_speed), y_speed_dis(rng_speed),
                    xz_speed_dis(rng_speed)};

    dst.color = kDefaultColor;

    dst.life_len = std::numeric_limits<float>::max();

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
      respawn_units_(0.f),
      particles_(detail::GenerateRandomParticles(size, pos)),
      live_particles_end_(particles_.begin()),
      pos_buffer_(),
      color_buffer_() {

  pos_buffer_.reserve(size);
  color_buffer_.reserve(size);

  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_vertex_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(detail::kParticleMesh),
               detail::kParticleMesh.data(), GL_STATIC_DRAW);

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
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_color_);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

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
  Respawn(delta);
  UpdateBuffers();
}

auto ParticleEffect::Render() -> void {
  if (particles_.begin() == live_particles_end_) {
    return;
  }

  shader_->Use();

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
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4,
                        std::distance(particles_.begin(), live_particles_end_));
  glBindVertexArray(0);
}

auto ParticleEffect::UpdateLife(TimeType const delta) -> void {
  for (auto iter = particles_.begin(); iter < live_particles_end_; ++iter) {
    iter->life_len += delta;
    if (iter->life_len > particle_life_span_) {
      live_particles_end_ = std::prev(live_particles_end_);
      std::swap(*iter, *live_particles_end_);
    }
  }
}

auto ParticleEffect::UpdatePositionsBuffer() -> void {
  pos_buffer_.clear();
  for (auto iter = particles_.begin(); iter != live_particles_end_; ++iter) {
    pos_buffer_.push_back(
        iter->position + iter->velocity * iter->life_len +
        (glm::vec3(0.f, -9.81, 0.f) * iter->life_len * iter->life_len));
  }
};

auto ParticleEffect::UpdateColorBuffer() -> void {
  color_buffer_.clear();
  for (auto iter = particles_.begin(); iter != live_particles_end_; ++iter) {
    color_buffer_.push_back(iter->color +
                            glm::vec4(1.f, 0.f, 0.f, 1.f) *
                                (1.f - (iter->life_len / particle_life_span_)));
  }
}

auto ParticleEffect::Respawn(TimeType const delta) -> void {
  respawn_units_ += delta;
  if (live_particles_end_ != particles_.end()) {
    auto const spawn_trigger =
        particle_life_span_ / static_cast<float>(std::distance(
                                  live_particles_end_, particles_.end()));

    while (respawn_units_ >= spawn_trigger &&
           live_particles_end_ != particles_.end()) {
      live_particles_end_->position = pos_;
      live_particles_end_->color = detail::kDefaultColor;
      live_particles_end_->life_len = 0;

      ++live_particles_end_;
      respawn_units_ -= spawn_trigger;
    }
  }
}

auto ParticleEffect::UpdateBuffers() -> void {
  UpdatePositionsBuffer();
  UpdateColorBuffer();
}

}  // namespace goya
