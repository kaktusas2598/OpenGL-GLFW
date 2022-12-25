#ifndef __TestTexture2D__
#define __TestTexture2D__

#include "Test.hpp"
#include "glm/glm.hpp"
#include "../VertexBuffer.hpp"
#include "../VertexBufferLayout.hpp"
#include "../Texture.hpp"

#include <memory>

namespace test {

    class TestTexture2D : public Test {
        public:
            TestTexture2D();
            ~TestTexture2D();

            void onUpdate(float deltaTime) override;
            void onRender() override;
            void onImGuiRender() override;
        private:
            float r;
            float inc;

            glm::vec3 translationA;
            glm::vec3 translationB;
            glm::vec3 cameraTranslation;

            float scale;
            float rotation;

            std::unique_ptr<VertexArray> vao;
            std::unique_ptr<VertexBuffer> vbo;
            //std::unique_ptr<VertexBufferLayout> layout;
            std::unique_ptr<IndexBuffer> ibo;
            std::unique_ptr<Shader> shader;
            std::unique_ptr<Texture> texture;

            glm::mat4 proj;
            int screenWidth, screenHeight;
    };
}
#endif // __TestTexture2D__
