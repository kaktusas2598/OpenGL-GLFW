cmake_minimum_required(VERSION 3.25.1)

# Setup project and set name
project(OpenGLTest)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# includes cmake/FindSDL2.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# These require findPackageName.cmake file to be available
# as an alternative we could use find_library() instead
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(ASSIMP REQUIRED)
include_directories(${LUA_INCLUDE_DIR})
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIRS})

file(GLOB sources src/*.cpp)
file(GLOB test_sources src/tests/*.cpp)
add_executable(${PROJECT_NAME} ${sources} ${test_sources})

add_subdirectory(src/vendor/glm)
add_subdirectory(src/vendor/imgui)
add_library(stb_image "src/vendor/stb_image.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC vendor)
#target_link_directories(${PROJECT_NAME} PUBLIC vendor)
#target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor)

# Link libraries
#file(GLOB SDL2_LIBS src/lib/*.a)
target_link_libraries(${PROJECT_NAME} ${ASSIMP_LIBRARIES} ${GLEW_LIBRARIES} glfw glm imgui stb_image)