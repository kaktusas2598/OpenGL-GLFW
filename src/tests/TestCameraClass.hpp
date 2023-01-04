#ifndef __TestCameraClass__
#define __TestCameraClass__

#include "Test.hpp"
#include "glm/glm.hpp"
#include "../VertexBuffer.hpp"
#include "../Camera.hpp"
#include "../VertexBufferLayout.hpp"
#include "../Texture.hpp"

#include <memory>

namespace test {

    class TestCameraClass : public Test {
        public:
            TestCameraClass();
            ~TestCameraClass();

            void processInput(GLFWwindow* window, float deltaTime) override;
            void onUpdate(float deltaTime) override;
            void onRender() override;
            void onImGuiRender() override;
        private:
            glm::vec3 translation;
            glm::vec3 cameraTranslation;

            float scale;
            float rotation;

            std::unique_ptr<VertexArray> vao;
            std::unique_ptr<VertexBuffer> vbo;
            std::unique_ptr<IndexBuffer> ibo;
            std::unique_ptr<Shader> shader;
            std::unique_ptr<Texture> texture;

            glm::mat4 proj;
            int screenWidth, screenHeight;

            float lastX, lastY;
            bool firstMouse;
    };
}
#endif // __TestCameraClass__
