#ifndef __TestInstancing__
#define __TestInstancing__

#include "Test.hpp"
#include "glm/glm.hpp"
#include "../Model.hpp"
#include "../VertexBuffer.hpp"
#include "../Camera.hpp"
#include "../VertexBufferLayout.hpp"
#include "../Texture.hpp"

#include <memory>

namespace test {

    class TestInstancing : public Test {
        public:
            TestInstancing();
            ~TestInstancing() {}

            void onUpdate(float deltaTime) override {}
            void onRender() override;
            void onImGuiRender() override;
        private:
            std::unique_ptr<VertexArray> vao;
            std::unique_ptr<VertexBuffer> vbo;

            std::unique_ptr<IndexBuffer> ibo;
            std::unique_ptr<Shader> shader;
            std::unique_ptr<Texture> texture;

            // Use separate VBOs for instanced array vertex attribute
            std::unique_ptr<VertexBuffer> instanceVbo;
            std::unique_ptr<VertexBuffer> asteroidInstanceVbo;

            std::unique_ptr<Model> rockModel;
            std::unique_ptr<Model> planetModel;
            std::unique_ptr<Shader> mvpTextureShader;
            std::unique_ptr<Shader> instanceMatrixShader;

            glm::vec3 cubePositions[1000];
            glm::mat4* asteroidTransforms;

            glm::mat4 proj;
            int screenWidth, screenHeight;
    };
}
#endif // __TestInstancing__
