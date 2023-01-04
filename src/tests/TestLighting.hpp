#ifndef __TestLighting__
#define __TestLighting__

#include "Test.hpp"
#include "glm/glm.hpp"
#include "../VertexBuffer.hpp"
#include "../Camera.hpp"
#include "../VertexBufferLayout.hpp"
#include "../Texture.hpp"

#include <memory>

namespace test {

    class TestLighting : public Test {
        public:
            TestLighting();
            ~TestLighting();

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
    };
}
#endif // __TestLighting__
