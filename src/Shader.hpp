#ifndef __Shader__
#define __Shader__

#include <string>
#include <unordered_map>

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};


class Shader {
    public:
        Shader(const std::string& fileName);
        ~Shader();


        void bind() const;
        void unbind() const;

        // Set uniforms, TODO: use templates to have multiple types of uniforms
        // Should be using glm::vec4 here
        void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

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
