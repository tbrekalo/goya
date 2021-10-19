#pragma once

#include "glm/glm.hpp"

namespace goya {

/* clang-format off */
inline auto N_matrix = (1.f / 6.f) * glm::mat4{
  -1.f,  3.f, -3.f,  1.f, 
   3.f, -6.f,  0.f,  4.f, 
  -3.f,  3.f,  3.f,  1.f, 
   1.f,  0.f,  0.f,  0.f
};
/* clang-format on */

}  // namespace goya
