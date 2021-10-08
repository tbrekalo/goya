#pragma once

#include <type_traits>

namespace goya::meta {

template <class E>
auto constexpr ToUnderyling(E e) -> std::underlying_type_t<E> {
  return static_cast<std::underlying_type_t<E>>(e);
}

}  // namespace goya::meta
