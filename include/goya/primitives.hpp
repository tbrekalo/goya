#pragma once

#include <cstddef>
#include <vector>

#include "glm/vec3.hpp"

namespace goya {

using IndexType = std::uint32_t;
using TimeType = float;

using Vertex3d = glm::vec3;
using Face = std::vector<IndexType>;

}  // namespace goya
