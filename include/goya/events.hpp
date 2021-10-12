#pragma once

#include <cstdint>
#include <functional>

namespace goya {

using KeyEventHandler = std::function<void(std::int32_t, int32_t)>;
using CursorEventHandler = std::function<void(double, double)>;
using WinResizeEventHandler = std::function<void(std::int32_t, std::int32_t)>;

}  // namespace goya
