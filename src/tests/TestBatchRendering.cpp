#include "TestBatchRendering.hpp"

#include "../Renderer.hpp"

#include "glm/gtc/matrix_transform.hpp"
// For glm::to_string()
#include "glm/gtx/string_cast.hpp"

#include "imgui/imgui.h"

#include <GLFW/glfw3.h>

namespace test {

    TestBatchRendering::TestBatchRendering()
        : translation(200, 200, 0),
        cameraTranslation(0, 0, 0), scale(0.5), rotation(90.0f) {

        // TODO: pass screen coords in instead of creating these raw pointers here
        // to get screen measurements
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        screenWidth = mode->width;
        screenHeight = mode->height;

        //free(monitor);
        //free(mode);

        // We render 2 quads using batch rendering - all vertices defined in vbo
        // 1,2 - positions, 3,4 - tex coords, 5,6,7,8 - color
        float vertices [] = {
            100.0f, 100.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            600.0, 100.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            600.0f, 400.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            100.0f, 400.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

            600.0f, 100.0f, 0.0f, 0.0f, 1.0f, 0.93f, 0.24f, 1.0f,
            1100.0, 100.0f, 1.0f, 0.0f, 1.0f, 0.93f, 0.24f, 1.0f,
            1100.0f, 400.0f, 1.0f, 1.0f, 1.0f, 0.93f, 0.24f, 1.0f,
            600.0f, 400.0f, 0.0f, 1.0f, 1.0f, 0.93f, 0.24f, 1.0f
        };

        // Index vertices to save memory, only need to define 4 vertices instead of 6 to draw square
        unsigned int indices[] = {
            0, 1, 2, // 1st triangle indices
            2, 3, 0, // 2nd triangle indices

            4, 5, 6,
            6, 7, 4
        };

        // OpenGL is a State machine, so things need to be done in certain order, so usually
        // you create vertex array, then vertex buffer
        // then you specify vertex layout/attrivutes, bind buffer
        // after that you generate index buffer to remove number of indices
        //
        // and finally define, compile and link shaders?

        // Using vertex array means we dont need to specify vertex attributes every time we draw
        // also let's us specify different vertex layouts, default vao can be used with compability profile
        // core profile requires vao to be set
        vao = std::make_unique<VertexArray>();
        vbo = std::make_unique<VertexBuffer>(vertices, 8 * 8 * sizeof(float));

        VertexBufferLayout layout;
        layout.push<float>(2); // position
        layout.push<float>(2); // texture coords
        layout.push<float>(4); // vertex color

        vao->addBuffer(*vbo, layout);

        // Generate and bind index buffer object
        ibo = std::make_unique<IndexBuffer>(indices, 12);

        // Create 4:3 aspect ratio projection ortographic matrix
        // This helps us use easier coords system, and it gets converted back to GL system in vertex shader, by multypling
        // each vertex position by this matrix
        proj = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight, -1.0f, 1.0f);

        // Demonstration of how our vertex position goes back to gl coords system by multiplying
        glm::vec4 vp(100.0f, 100.0f, 0.0f, 1.0f);
        glm::vec4 result = proj * vp;
        std::cout << "Vertex position as defined by us: "<< glm::to_string(vp) << std::endl;
        std::cout << "Vertex position after being multiplied with projection matrix: "<< glm::to_string(result) << std::endl;

        shader = std::make_unique<Shader>("assets/shaders/batch.glsl");
        shader->bind();
        shader->setUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);


        texture = std::make_unique<Texture>("assets/textures/slime.png");
        texture->bind(); // bound to default slot 0
        // Set uniform to tell shader that we need to sample texture from slot 0
        shader->setUniform1i("u_Texture", 0);

    }

    TestBatchRendering::~TestBatchRendering() {
    }

    void TestBatchRendering::onUpdate(float deltaTime) {
    }

    void TestBatchRendering::onRender() {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        texture->bind(); // bound to default slot 0

        // Creating view matrix, first param here is identity view matrix
        glm::mat4 view = glm::translate(glm::mat4(1.0f), cameraTranslation);
        // Creating model matrix for transformations
        glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));
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

    void TestBatchRendering::onImGuiRender() {
        ImGui::Text("Model Matrix");
        ImGui::SliderFloat("Translation X", &translation.x, 0.0f, screenWidth);
        ImGui::SliderFloat("Translation Y", &translation.y, 0.0f, screenHeight);
        ImGui::SliderFloat("Translation Z", &translation.z, -1.0f, 1.0f);
        ImGui::SliderFloat("Scale", &scale, 0.0f, 5.0f);
        ImGui::SliderFloat("Rotation", &rotation, -180.0f, 180.0f);
        ImGui::Text("View Matrix");
        ImGui::SliderFloat("View Translation X", &cameraTranslation.x, 0.0f, screenWidth);
        ImGui::SliderFloat("View Translation Y", &cameraTranslation.y, 0.0f, screenHeight);
        ImGui::SliderFloat("View Translation Z", &cameraTranslation.z, -1.0f, 1.0f);

    }
}

