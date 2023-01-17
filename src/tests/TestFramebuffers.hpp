#ifndef __TestFramebuffers__
#define __TestFramebuffers__

#include "Test.hpp"
#include "glm/glm.hpp"
#include "../VertexBuffer.hpp"
#include "../Camera.hpp"
#include "../VertexBufferLayout.hpp"
#include "../Texture.hpp"

#include <memory>

namespace test {

    class TestFramebuffers : public Test {
        public:
            TestFramebuffers();
            ~TestFramebuffers();

            void onUpdate(float deltaTime) override;
            void onRender() override;
            void onImGuiRender() override;
        private:
            std::unique_ptr<VertexArray> vao;
            std::unique_ptr<VertexBuffer> vbo;
            std::unique_ptr<IndexBuffer> ibo;

            std::unique_ptr<VertexArray> floorVao;
            std::unique_ptr<VertexBuffer> floorVbo;
            std::unique_ptr<IndexBuffer> floorIbo;

            std::unique_ptr<Shader> objectShader;
            std::unique_ptr<Shader> lightSourceShader;

            std::unique_ptr<Texture> diffuseMap;
            std::unique_ptr<Texture> specularMap;

            std::unique_ptr<Texture> grassTexture;
            std::unique_ptr<Texture> windowTexture;

            unsigned int fbo; // Framebuffer object
            unsigned int renderTextureID; // Texture to be attached to the framebuffer
            unsigned int rbo; // Renderbuffer object also used as an attachment, they can only be written to and are useful for writing or copying data between buffers
            std::unique_ptr<VertexArray> screenQuadVao;
            std::unique_ptr<VertexBuffer> screenQuadVbo;
            std::unique_ptr<Shader> postProcessingShader;

            // Post-Processing Effects flags
            bool invertColors = false;
            bool grayScale = false;
            bool sharpen = false;
            bool blur = false;
            bool edgeDetection = false;

            glm::mat4 proj;
            int screenWidth, screenHeight;

            glm::vec3 lightPosition;
            glm::vec3 lightDirection;
            glm::vec3 ambient;
            glm::vec3 diffuse;
            glm::vec3 specular;
            float snininess;

            glm::vec3 dirLightColor;
            glm::vec3 pointLightColor;
            glm::vec3 spotLightColor;

            float constant, linear, quadratic;
    };
}
#endif // __TestFramebuffers__
