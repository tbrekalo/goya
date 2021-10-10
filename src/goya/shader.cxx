#include "goya/shader.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "glad/glad.h"
#include <OpenGL/gl.h>
#include "OpenGL/OpenGL.h"
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

}  // namespace goya
