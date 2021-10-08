cmake_minimum_required(VERSION 3.19)
project(goya LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

include(cmake/Warnings.cmake)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)

set(${PROJECT_NAME}_SOURCES
  src/goya/window.cxx
  src/goya/engine.cxx
)

add_subdirectory(lib/glad)

add_executable(${PROJECT_NAME} src/main.cxx ${${PROJECT_NAME}_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include)

set_default_warnings(${PROJECT_NAME} PRIVATE FALSE)
target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw glm)
