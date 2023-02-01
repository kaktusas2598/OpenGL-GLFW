cmake_minimum_required(VERSION 3.22.1)

# Setup project and set name
project(OpenGLTest)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set custom module directory
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# Cmake will generate compile_commands.json file used by clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# These require findPackageName.cmake file to be available
# as an alternative we could use find_library() instead
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
#find_package(glfw3 REQUIRED)
find_package(ASSIMP REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

if(ASSIMP_FOUND)
    message(STATUS "libAssimp found")
else()
    message(STATUS "libAssimp not found")
endif()

file(GLOB sources src/*.cpp)
file(GLOB test_sources src/tests/*.cpp)
add_executable(${PROJECT_NAME} WIN32 ${sources} ${test_sources})

add_subdirectory(src/vendor/glm)
add_subdirectory(src/vendor/glfw)
add_subdirectory(src/vendor/imgui)
add_library(stb_image "src/vendor/stb_image.cpp")

# Set 3rd party include locations
target_include_directories(${PROJECT_NAME} PUBLIC vendor)

message(STATUS "GLEW LIBRARY:${GLEW_LIBRARIES}")
message(STATUS "OpenGL LIBRARY:${OpenGL_LIBRARIES}")

# Link libraries
target_link_libraries(${PROJECT_NAME} ${ASSIMP_LIBRARIES} ${OpenGL_LIBRARIES} ${GLEW_LIBRARIES} ${CMAKE_DL_LIBS} glfw glm imgui stb_image)
