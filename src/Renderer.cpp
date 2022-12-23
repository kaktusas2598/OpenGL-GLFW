#include "Renderer.hpp"

#include <iostream>

void GLClearError() {
    while(glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* func, const char* file, int line) {
    while(GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] " << "(" << error << "): " << func << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

