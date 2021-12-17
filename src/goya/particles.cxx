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

}  // namespace detail

ParticleEffect::ParticleEffect(std::shared_ptr<Shader> shader,
                               std::function<Particle(void)> particle_src,
                               float const particle_life_span,
                               std::size_t const size)
    : shader_(std::move(shader)),
      particle_src_(std::move(particle_src)),
      particle_life_span_(particle_life_span),
      respawn_units_(0.f),
      particles_(size),
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

  SetScale(glm::mat4(1.f));
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

auto ParticleEffect::Draw() -> void {
  if (particles_.begin() == live_particles_end_) {
    return;
  }

  shader_->Use();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

auto ParticleEffect::SetScale(glm::mat4 const scale_matrix) -> void {
  shader_->Use();
  shader_->SetMat4("systemScale", scale_matrix);
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
                            glm::vec4(1.f, 0.08f, 0.12f, 1.f) *
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
      *live_particles_end_ = particle_src_();

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
