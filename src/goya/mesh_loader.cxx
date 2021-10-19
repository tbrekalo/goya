#include "goya/mesh_loader.hpp"

#include <algorithm>
#include <fstream>
#include <limits>
#include <sstream>

namespace goya {

namespace detail {

class ValueRange {
 public:
  // unwrap the glm nesting
  using ValueType = std::vector<Vertex3d>::value_type::value_type;

  auto Update(ValueType const val) noexcept -> void {
    lo = std::min(lo, val);
    hi = std::max(hi, val);
  }

  auto Lo() const noexcept -> ValueType { return lo; }
  auto Hi() const noexcept -> ValueType { return hi; }

  auto Range() const noexcept -> ValueType { return hi - lo; }

 private:
  ValueType lo = std::numeric_limits<ValueType>::max();
  ValueType hi = std::numeric_limits<ValueType>::min();
};

auto NormalizeVertices(std::vector<Vertex3d>& vertices) -> void {
  if (vertices.empty()) {
    return;
  }

  auto value_ranges = std::vector<ValueRange>(vertices.front().length());
  for (auto const vertex : vertices) {
    for (auto i = 0; i < vertex.length(); ++i) {
      value_ranges[static_cast<std::size_t>(i)].Update(vertex[i]);
    }
  }

  auto scale_factor = std::numeric_limits<Vertex3d::value_type>::max();
  auto lower_bound = std::numeric_limits<Vertex3d::value_type>::max();

  for (auto const& value_range : value_ranges) {
    if (value_range.Range() > 1e-9) {
      scale_factor = std::min(scale_factor, 2.f / value_range.Range());
      lower_bound = std::min(lower_bound, value_range.Lo());
    }
  }

  for (auto& vertex : vertices) {
    for (auto i = 0; i < vertex.length(); ++i) {
      vertex[i] = -1.f + (vertex[i] - lower_bound) * scale_factor;
    }
  }
}

}  // namespace detail

auto LoadMeshObjData(std::string const& path) -> MeshObjData {
  return LoadMeshObjData(path.c_str());
}

auto LoadMeshObjData(char const* path) -> MeshObjData {
  auto ifstrm = std::ifstream(path);

  auto vertices = std::vector<Vertex3d>();
  auto faces = std::vector<Face>();

  auto const parse_vertex_line = [&]() -> Vertex3d {
    auto dst = Vertex3d();
    for (auto i = 0; i < dst.length(); ++i) {
      ifstrm >> dst[i];
    }

    ifstrm.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return dst;
  };

  auto const parse_face_line = [&]() -> Face {
    auto line = std::string();
    std::getline(ifstrm, line);

    auto istrm = std::istringstream(std::move(line));
    auto dst = Face();

    auto index = IndexType();
    istrm.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    while (istrm >> index) {
      dst.push_back(index);
      istrm.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    }

    return dst;
  };

  auto line_tag = '\0';
  while (ifstrm >> line_tag) {
    if (line_tag == 'v') {
      vertices.push_back(parse_vertex_line());
    } else if (line_tag == 'f') {
      faces.push_back(parse_face_line());
    } else {
      ifstrm.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
  }

  detail::NormalizeVertices(vertices);

  return MeshObjData(std::move(vertices), std::move(faces));
}

}  // namespace goya
