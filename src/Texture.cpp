#include "Texture.hpp"

#include "stb_image.h"

Texture::Texture(const std::string& fileName)
    : rendererID(0), filePath(fileName), localBuffer(nullptr), width(0), height(0), BPP(0) {

    // Not sure why I need to flip texture for GL
    stbi_set_flip_vertically_on_load(1);

    localBuffer = stbi_load(fileName.c_str(), &width, &height, &BPP, 4);

    GLCall(glGenTextures(1, &rendererID));
    GLCall(glBindTexture(GL_TEXTURE_2D, rendererID));

    // TODO: read about glTexParameteri and GLTexImage2D params
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));

    if (localBuffer)
        stbi_image_free(localBuffer);
}

Texture::~Texture() {
    GLCall(glDeleteTextures(1, &rendererID));
}

void Texture::bind(unsigned int slot) const {
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, rendererID));
}

void Texture::unbind() const {
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
