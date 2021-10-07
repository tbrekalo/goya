#pragma once

namespace goya {

class Engine {
 public:
   auto Run() -> void;
 private:
  auto InitWindow() -> void;
  auto InitVulkan() -> void;
  auto MainLoop() -> void;
  auto Cleanup() -> void;
};

}  // namespace goya
