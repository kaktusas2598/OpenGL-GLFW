#ifndef __TestMaterials__
#define __TestMaterials__

#include "Test.hpp"
#include "glm/glm.hpp"
#include "../VertexBuffer.hpp"
#include "../Camera.hpp"
#include "../VertexBufferLayout.hpp"
#include "../Texture.hpp"

#include <memory>

namespace test {

    class TestMaterials : public Test {
        public:
            TestMaterials();
            ~TestMaterials();

            void onUpdate(float deltaTime) override;
            void onRender() override;
            void onImGuiRender() override;
        private:
            std::unique_ptr<VertexArray> vao;
            std::unique_ptr<VertexBuffer> vbo;
            std::unique_ptr<IndexBuffer> ibo;
            std::unique_ptr<Shader> lightingShader;
            std::unique_ptr<Shader> lightSourceShader;
            std::unique_ptr<Texture> texture;

            glm::mat4 proj;
            int screenWidth, screenHeight;

            glm::vec3 lightPosition;
            glm::vec3 ambient;
            glm::vec3 diffuse;
            glm::vec3 specular;
            float snininess;
    };
}
#endif // __TestMaterials__
