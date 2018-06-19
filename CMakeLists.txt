cmake_minimum_required(VERSION 3.11)
project(glslViewer)

add_definitions(-DPLATFORM_LINUX)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-Wall)
add_definitions(-Wpedantic)
find_package(glfw3 REQUIRED)
set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

include_directories(include)
include_directories(include/imgui)
include_directories(src)

add_library(imgui STATIC include/imgui/imgui.cpp include/imgui/imgui_demo.cpp include/imgui/imgui_draw.cpp)

add_subdirectory(include/tinyobjloader)
add_subdirectory(src)


add_executable(glslViewer src/app.cpp src/main.cpp)

target_link_libraries(glslViewer 3d gl tools types ui inspect imgui OpenGL::OpenGL glfw objloader ${CMAKE_THREAD_LIBS_INIT})
