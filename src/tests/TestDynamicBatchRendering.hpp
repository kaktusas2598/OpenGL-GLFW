#ifndef __TestDynamicBatchRendering__
#define __TestDynamicBatchRendering__

#include "Test.hpp"
#include "glm/glm.hpp"
#include "../VertexBuffer.hpp"
#include "../VertexBufferLayout.hpp"
#include "../Texture.hpp"

#include <memory>

namespace test {

    class TestDynamicBatchRendering : public Test {
        public:
            TestDynamicBatchRendering();
            ~TestDynamicBatchRendering();

            void onUpdate(float deltaTime) override;
            void onRender() override;
            void onImGuiRender() override;
        private:
            glm::vec3 translation;
            glm::vec3 cameraTranslation;
            glm::vec3 cameraRotation;

            float scale;
            float rotation;

            float quad0x;
            float quad0y;

            std::unique_ptr<VertexArray> vao;
            std::unique_ptr<VertexBuffer> vbo;
            //std::unique_ptr<VertexBufferLayout> layout;
            std::unique_ptr<IndexBuffer> ibo;
            std::unique_ptr<Shader> shader;
            std::unique_ptr<Texture> texture;
            std::unique_ptr<Texture> texture2;

            glm::mat4 proj;
            int screenWidth, screenHeight;
    };
}
#endif // __TestDynamicBatchRendering__
