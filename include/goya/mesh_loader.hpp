#pragma once

#include <string>

#include "goya/mesh_obj_data.hpp"

namespace goya {

auto LoadMeshObjData(std::string const& path) -> MeshObjData;

auto LoadMeshObjData(char const* path) -> MeshObjData;

}  // namespace goya
