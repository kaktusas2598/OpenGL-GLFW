#include "TestDynamicBatchRendering.hpp"

#include "../Renderer.hpp"

#include "glm/gtc/matrix_transform.hpp"
// For glm::to_string()
#include "glm/gtx/string_cast.hpp"

#include "imgui/imgui.h"

#include <GLFW/glfw3.h>
#include <array>

namespace test {

    // Of course in actual angine I will use glm for this, no need to reinvent wheel in math stuff
    struct Vec2 {
        float x, y;
    };

    struct Vec3 {
        float x, y, z;
    };

    struct Vec4 {
        float x, y, z, w;
    };


    // Example struture of single vertex data for this example
    struct Vertex {
        Vec3 position;
        Vec2 texCoord;
        Vec4 color;
        float texID; // GL Texture slot
    };

    static Vertex* createQuad(Vertex* target, float x, float y, float w, float h, float textureID) {

        target->position = { x, y, 0.0f };
        target->texCoord = { 0.0f, 0.0f };
        target->color = { 1.0f, 0.0f, 0.0f, 1.0f };
        target->texID = textureID;
        target++;

        target->position = { x + w, y, 0.0f };
        target->texCoord = { 1.0f, 0.0f };
        target->color = { 0.0f, 1.0f, 0.0f, 1.0f };
        target->texID = textureID;
        target++;

        target->position = { x + w, y + h, 0.0f };
        target->texCoord = { 1.0f, 1.0f };
        target->color = { 0.0f, 0.0f, 1.0f, 1.0f };
        target->texID = textureID;
        target++;

        target->position = { x, y + h, 0.0f };
        target->texCoord = { 0.0f, 1.0f };
        target->color = { 0.3f, 0.3f, 0.3f, 1.0f };
        target->texID = textureID;
        target++;

        return target;
    }

    const size_t MaxQuadCount = 1000;
    const size_t MaxVertexCount = MaxQuadCount * 4;
    const size_t MaxIndexCount = MaxQuadCount * 6;

    TestDynamicBatchRendering::TestDynamicBatchRendering()
        : translation(0, 0, 0),
        cameraTranslation(0, 0, -1.0f), cameraRotation(0, 0, 0) ,scale(0.2), rotation(45.0f), quad0x(0.1f), quad0y(0.1f) {

        // TODO: pass screen coords in instead of creating these raw pointers here
        // to get screen measurements
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        screenWidth = mode->width;
        screenHeight = mode->height;

        //free(monitor);
        //free(mode);

        // TODO: if this is uint32_t, the cast in IndexBuffer constructor to
        // const unsigned int pointer causes weird issues and big lag
        unsigned int indices[MaxIndexCount];
        uint32_t offset = 0;
        // Set index buffer for quads only using 0, 1, 2, 2, 3, 0 pattern
        // This type of quad obviously won't work for stuff with triangles or polugons or whatever
        // this index buffer arrangement will not work for 3D models too
        for (size_t i = 0; i < MaxIndexCount; i += 6) {
            indices[i + 0] = 0 + offset;
            indices[i + 1] = 1 + offset;
            indices[i + 2] = 2 + offset;

            indices[i + 3] = 2 + offset;
            indices[i + 4] = 3 + offset;
            indices[i + 5] = 0 + offset;

            offset += 4;
        }

        // Using vertex array means we dont need to specify vertex attributes every time we draw
        // also let's us specify different vertex layouts, default vao can be used with compability profile
        // core profile requires vao to be set
        vao = std::make_unique<VertexArray>();
        // If we want to change vertex data dynamically, we set data to nullptr and use GL_DYNAMIC_DRAW
        vbo = std::make_unique<VertexBuffer>(nullptr, MaxVertexCount * sizeof(Vertex), GL_DYNAMIC_DRAW);

        VertexBufferLayout layout;
        layout.push<float>(3); // position
        layout.push<float>(2); // texture coords
        layout.push<float>(4); // vertex color
        layout.push<float>(1); // texture slot id

        vao->addBuffer(*vbo, layout);

        // Generate and bind index buffer object
        ibo = std::make_unique<IndexBuffer>(indices, MaxIndexCount);

        shader = std::make_unique<Shader>("assets/shaders/batch.glsl");
        shader->bind();
        shader->setUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

        auto loc = glGetUniformLocation(shader->getRendererID(), "u_textures");
        int samplers[2] = {0, 1};
        glUniform1iv(loc, 2, samplers);

        texture = std::make_unique<Texture>("assets/textures/slime.png");
        texture2 = std::make_unique<Texture>("assets/textures/mountains.png");
        texture->bind(); // bound to default slot 0
        texture2->bind(1);
        // Set uniform to tell shader that we need to sample texture from slot 0
        shader->setUniform1i("u_Texture", 0);
    }

    TestDynamicBatchRendering::~TestDynamicBatchRendering() {
    }

    void TestDynamicBatchRendering::onUpdate(float deltaTime) {
        std::array<Vertex, 1000> vertices;
        Vertex* buffer = vertices.data();
        // Setting dynamic vertex buffer data
        // Creating couple of quads and copying them into vertices guffer which
        // then is passed to glBufferSubData
        for (int y = 0; y < 5; y++) {
            for (int x = 0; x < 5; x++) {
                buffer = createQuad(buffer, x, y, 0.8f, 0.5f, 1.0f);
            }
        }
        buffer = createQuad(buffer, quad0x, quad0y, 0.5f, 0.5f, 0.0f);

        vbo->bind();
        GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(), vertices.data()));
    }

    void TestDynamicBatchRendering::onRender() {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        texture->bind(); // bound to default slot 0
        texture2->bind(1);

        //proj = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight, -1.0f, 1.0f);
        proj = glm::perspective(glm::radians(rotation), (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);

        // Creating view matrix, first param here is identity view matrix
        glm::mat4 view = glm::translate(glm::mat4(1.0f), cameraTranslation);
        view = glm::rotate(view, glm::radians(cameraRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::rotate(view, glm::radians(cameraRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::rotate(view, glm::radians(cameraRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        //view = glm::rotate();
        // Creating model matrix for transformations
        glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
        model = glm::scale(model, glm::vec3(scale, scale, scale));

        // MVP gets multiplied in reverse order here, because OpenGL stores matrices in column order
        // On Direct x this multiplication would be model * view * proj
        glm::mat4 mvp = proj * view * model;

        Renderer renderer;

        shader->bind();
        shader->setUniform4f("u_Color", 0.5f, 0.3f, 0.8f, 1.0f);
        shader->setUniformMat4f("u_MVP", mvp);
        renderer.draw(*vao, *ibo, *shader);
    }

    void TestDynamicBatchRendering::onImGuiRender() {
        ImGui::Text("Model Matrix");
        //ImGui::SliderFloat("Translation X", &translation.x, 0.0f, screenWidth);
        //ImGui::SliderFloat("Translation Y", &translation.y, 0.0f, screenHeight);
        //ImGui::SliderFloat("Translation Z", &translation.z, -1.0f, 1.0f);
        ImGui::SliderFloat("Scale", &scale, 0.0f, 5.0f);
        ImGui::Text("View Matrix");
        ImGui::SliderFloat("View Translation X", &cameraTranslation.x, -1.0f, 1.0f);
        ImGui::SliderFloat("View Translation Y", &cameraTranslation.y, -1.0f, 1.0f);
        ImGui::SliderFloat("View Translation Z", &cameraTranslation.z, -1.0f, 1.0f);
        ImGui::SliderFloat("View Rotation X", &cameraRotation.x, 0.0f, 90.0f);
        ImGui::SliderFloat("View Rotation Y", &cameraRotation.y, 0.0f, 90.0f);
        ImGui::SliderFloat("View Rotation Z", &cameraRotation.z, 0.0f, 90.0f);

        ImGui::Text("Projection Matrix");
        ImGui::SliderFloat("Rotation", &rotation, -180.0f, 180.0f);

        ImGui::Text("Quad 0 Controls");
        ImGui::SliderFloat("X", &quad0x, -1.0f, 1.0f);
        ImGui::SliderFloat("Y", &quad0y, -1.0f, 1.0f);

    }
}

