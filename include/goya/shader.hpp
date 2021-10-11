#pragma once

#include <cstdint>
#include <string>

#include "glm/glm.hpp"

namespace goya {

class Shader {
 public:
  Shader(char const* vertex_src_path, char const* fragment_src_path);

  auto Id() const noexcept -> std::uint32_t;
  auto Use() const noexcept -> void;

  auto SetBool(std::string const& name, bool const val) const -> void;
  auto SetInt32(std::string const& name, std::int32_t const val) const -> void;
  auto SetFloat(std::string const& name, float const val) const -> void;

  auto SetVec2(std::string const& name, glm::vec2 const vec) const -> void;
  auto SetVec3(std::string const& name, glm::vec3 const vec) const -> void;
  auto SetVec4(std::string const& name, glm::vec4 const vec) const -> void;

  auto SetMat2(std::string const& name, glm::mat2 const& mat) const -> void;
  auto SetMat3(std::string const& name, glm::mat3 const& mat) const -> void;
  auto SetMat4(std::string const& name, glm::mat4 const& mat) const -> void;

 private:
  std::uint32_t id_;
};

}  // namespace goya
