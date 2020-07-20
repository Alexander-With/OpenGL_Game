cmake_minimum_required(VERSION 3.18)
project(Hello_OpenGL)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(BUILD_SHARED_LIBS FALSE)

set(GLFW_BUILD_DOCS FALSE CACHE BOOL "docstring" FORCE)
set(GLFW_BUILD_TESTS FALSE CACHE BOOL "docstring" FORCE)
set(GLFW_BUILD_EXAMPLES FALSE CACHE BOOL "docstring" FORCE)


add_subdirectory(glfw-3.3.2)
link_libraries(glfw)
add_executable(main main.cpp glad/glad.c)