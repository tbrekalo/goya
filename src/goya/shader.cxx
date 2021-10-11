#include "goya/shader.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "GL/glew.h"
#include "goya/meta.hpp"

namespace goya {

namespace detail {

enum class ShaderType : std::uint32_t {
  kVertex = GL_VERTEX_SHADER,
  kFragment = GL_FRAGMENT_SHADER
};

constexpr auto kErrorBuffSize = 1024UL;

auto LoadShaderSource(char const* path) -> std::string {
  auto ifstrm = std::ifstream();
  auto ostrm = std::ostringstream();

  ifstrm.exceptions(std::ifstream::badbit | std::ifstream::failbit);
  ifstrm.open(path);

  ostrm << ifstrm.rdbuf();
  return ostrm.str();
};

auto CheckShaderCompilation(std::uint32_t const shader_id) {
  auto is_ok = std::int32_t();
  char buff[kErrorBuffSize];

  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &is_ok);
  if (!is_ok) {
    glGetShaderInfoLog(shader_id, kErrorBuffSize, nullptr, buff);

    using namespace std::string_literals;
    throw std::runtime_error("[goya::Shader] failed to prase shader: "s + buff);
  }
}

auto CheckShaderLinking(std::uint32_t const program_id) {
  auto is_ok = std::int32_t();
  char buff[kErrorBuffSize];

  glGetProgramiv(program_id, GL_LINK_STATUS, &is_ok);
  if (!is_ok) {
    glGetShaderInfoLog(program_id, kErrorBuffSize, nullptr, buff);

    using namespace std::string_literals;
    throw std::runtime_error("[goya::Shader] failed to link shader program: "s +
                             buff);
  }
}

auto CompileShader(std::string const& shader_src, ShaderType const type)
    -> std::uint32_t {
  auto shader_type = meta::ToUnderyling(type);
  auto shader_id = glCreateShader(shader_type);

  auto const shader_c_str = shader_src.c_str();
  glShaderSource(shader_id, 1, &shader_c_str, nullptr);
  glCompileShader(shader_id);

  return shader_id;
}

}  // namespace detail

Shader::Shader(char const* vertex_src_path, char const* fragment_src_path) {
  auto const vertex_src = detail::LoadShaderSource(vertex_src_path);
  auto const fragment_src = detail::LoadShaderSource(fragment_src_path);

  auto const vertex_id =
      detail::CompileShader(vertex_src, detail::ShaderType::kVertex);
  detail::CheckShaderCompilation(vertex_id);

  auto const fragment_id =
      detail::CompileShader(fragment_src, detail::ShaderType::kFragment);
  detail::CheckShaderCompilation(fragment_id);

  id_ = glCreateProgram();
  glAttachShader(id_, vertex_id);
  glAttachShader(id_, fragment_id);

  glLinkProgram(id_);
  detail::CheckShaderLinking(id_);

  glDeleteShader(vertex_id);
  glDeleteShader(fragment_id);
}

auto Shader::Id() const noexcept -> std::uint32_t { return id_; }
auto Shader::Use() const noexcept -> void { glUseProgram(id_); }

auto Shader::SetBool(std::string const& name, bool const val) const -> void {
  glUniform1i(glGetUniformLocation(id_, name.c_str()),
              static_cast<std::int32_t>(val));
}

auto Shader::SetInt32(std::string const& name, std::int32_t const val) const
    -> void {
  glUniform1i(glGetUniformLocation(id_, name.c_str()), val);
}

auto Shader::SetFloat(std::string const& name, float const val) const -> void {
  glUniform1f(glGetUniformLocation(id_, name.c_str()), val);
}

auto Shader::SetVec2(std::string const& name, glm::vec2 const vec) const
    -> void {
  glUniform2fv(glGetUniformLocation(id_, name.c_str()), 1, &vec[0]);
}

auto Shader::SetVec3(std::string const& name, glm::vec3 const vec) const
    -> void {
  glUniform3fv(glGetUniformLocation(id_, name.c_str()), 1, &vec[0]);
}

auto Shader::SetVec4(std::string const& name, glm::vec4 const vec) const
    -> void {
  glUniform4fv(glGetUniformLocation(id_, name.c_str()), 1, &vec[0]);
}

auto Shader::SetMat2(std::string const& name, glm::mat2 const& mat) const
    -> void {
  glUniformMatrix2fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE,
                     &mat[0][0]);
}

auto Shader::SetMat3(std::string const& name, glm::mat3 const& mat) const
    -> void {
  glUniformMatrix3fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE,
                     &mat[0][0]);
}

auto Shader::SetMat4(std::string const& name, glm::mat4 const& mat) const
    -> void {
  glUniformMatrix4fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE,
                     &mat[0][0]);
}

}  // namespace goya
