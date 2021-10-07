#include "goya/engine.hpp"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "vulkan/vulkan.hpp"

namespace goya {

auto Engine::Run() -> void {
  InitVulkan();
  MainLoop();
  Cleanup();
}

auto Engine::InitWindow() -> void {}

auto Engine::InitVulkan() -> void {}

auto Engine::MainLoop() -> void {}

auto Engine::Cleanup() -> void {}

}  // namespace goya
