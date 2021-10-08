#pragma once

#include <cstdint>

namespace goya {


class Shader {
 public:
   Shader(char const* vertex_src_path, char const* fragment_src_path);

  auto Id() const noexcept -> std::uint32_t;
  auto Use() const noexcept -> void;

 private:
  std::uint32_t id_;
};

}  // namespace goya
