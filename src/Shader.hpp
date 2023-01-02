#ifndef __Shader__
#define __Shader__

#include <string>
#include <unordered_map>

#include "glm/glm.hpp"

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};


class Shader {
    public:
        // TODO: write another const to accpept 2 files or more to separate vertex and fragment shaders
        Shader(const std::string& fileName);
        ~Shader();


        void bind() const;
        void unbind() const;

        unsigned int getRendererID() const { return rendererID; }

        // Set uniforms, TODO: use templates to have multiple types of uniforms
        // Should be using glm::vec4 here
        void setUniform1i(const std::string& name, float value);
        void setUniform1f(const std::string& name, int value);
        void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
        void setUniformMat4f(const std::string& name, const glm::mat4& matrix);

    private:
        unsigned int rendererID;
        std::string filePath;
        // caching for uniforms
        std::unordered_map<std::string, int> uniformLocationCache;

        unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);
        unsigned int compileShader(unsigned int type, const std::string& source);
        ShaderProgramSource parseShader(const std::string& fileName);

        int getUniformLocation(const std::string& name);
};

#endif // __Shader__
