#ifndef __Texture__
#define __Texture__

#include "Renderer.hpp"

class Texture {
    public:
        Texture(const std::string& fileName);
        ~Texture();

        void bind(unsigned int slot = 0) const;
        void unbind() const;

        inline int getWidth() const { return width; }
        inline int getHeight() const { return height; }
    private:
        unsigned int rendererID;
        std::string filePath;
        unsigned char* localBuffer;
        int width, height, BPP; // Bits per picture
};

#endif // __Texture__
