#ifndef __TestLightCasters__
#define __TestLightCasters__

#include "Test.hpp"
#include "glm/glm.hpp"
#include "../VertexBuffer.hpp"
#include "../Camera.hpp"
#include "../VertexBufferLayout.hpp"
#include "../Texture.hpp"

#include <memory>

namespace test {

    class TestLightCasters : public Test {
        public:
            TestLightCasters();
            ~TestLightCasters();

            void onUpdate(float deltaTime) override;
            void onRender() override;
            void onImGuiRender() override;
        private:
            std::unique_ptr<VertexArray> vao;
            std::unique_ptr<VertexBuffer> vbo;
            std::unique_ptr<IndexBuffer> ibo;
            std::unique_ptr<Shader> lightingShader;
            std::unique_ptr<Shader> lightSourceShader;
            std::unique_ptr<Texture> diffuseMap;
            std::unique_ptr<Texture> specularMap;

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
#endif // __TestLightCasters__
