find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB IMGUI_SOURCE_DIR *.cpp)
add_library(imgui STATIC ${IMGUI_SOURCE_DIR})

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${OPENGL_INCLUDE_DIR})

target_link_libraries(imgui ${OPENGL_LIBRARIES} glfw)
