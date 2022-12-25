#include "TestTexture2D.hpp"

#include "../Renderer.hpp"

#include "glm/gtc/matrix_transform.hpp"
// For glm::to_string()
#include "glm/gtx/string_cast.hpp"

#include "imgui/imgui.h"

#include <GLFW/glfw3.h>

namespace test {

    TestTexture2D::TestTexture2D()
        : r(0.0f), inc(0.05f), translationA(200, 200, 0), translationB(600, 200, 0),
        cameraTranslation(0, 0, 0), scale(0.5), rotation(90.0f) {

        // TODO: pass screen coords in instead of creating these raw pointers here
        // to get screen measurements
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        screenWidth = mode->width;
        screenHeight = mode->height;

        //free(monitor);
        //free(mode);

        // 1st 2 floats position, 2nd two - tex coords
        float positions [] = {
            100.0f, 100.0f, 0.0f, 0.0f, // 0
            600.0, 100.0f, 1.0f, 0.0f, // 1
            600.0f, 400.0f, 1.0f, 1.0f, // 2
            100.0f, 400.0f, 0.0f, 1.0f// 3
        };

        // Index vertices to save memory, only need to define 4 vertices instead of 6 to draw square
        unsigned int indices[] = {
            0, 1, 2, // 1st triangle indices
            2, 3, 0 // 2nd triangle indices
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
        vbo = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.push<float>(2); // position
        layout.push<float>(2); // texture coords

        vao->addBuffer(*vbo, layout);

        // Generate and bind index buffer object
        ibo = std::make_unique<IndexBuffer>(indices, 6);

        // Create 4:3 aspect ratio projection ortographic matrix
        // This helps us use easier coords system, and it gets converted back to GL system in vertex shader, by multypling
        // each vertex position by this matrix
        proj = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight, -1.0f, 1.0f);

        // Demonstration of how our vertex position goes back to gl coords system by multiplying
        glm::vec4 vp(100.0f, 100.0f, 0.0f, 1.0f);
        glm::vec4 result = proj * vp;
        std::cout << "Vertex position as defined by us: "<< glm::to_string(vp) << std::endl;
        std::cout << "Vertex position after being multiplied with projection matrix: "<< glm::to_string(result) << std::endl;

        shader = std::make_unique<Shader>("assets/shaders/shader.glsl");
        shader->bind();
        shader->setUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);


        texture = std::make_unique<Texture>("assets/textures/slime.png");
        texture->bind(); // bound to default slot 0
        // Set uniform to tell shader that we need to sample texture from slot 0
        shader->setUniform1i("u_Texture", 0);

    }

    TestTexture2D::~TestTexture2D() {
    }

    void TestTexture2D::onUpdate(float deltaTime) {
        if (r > 1.0f) {
            inc = -0.05f;
        } else if (r < 0.0f) {
            inc = 0.05f;
        }
        r += inc;
    }

    void TestTexture2D::onRender() {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        texture->bind(); // bound to default slot 0

        // Creating view matrix, first param here is identity view matrix
        glm::mat4 view = glm::translate(glm::mat4(1.0f), cameraTranslation);
        // Creating model matrix for transformations
        glm::mat4 modelA = glm::translate(glm::mat4(1.0f), translationA);
        modelA = glm::rotate(modelA, glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));
        modelA = glm::scale(modelA, glm::vec3(scale, scale, scale));

        // MVP gets multiplied in reverse order here, because OpenGL stores matrices in column order
        // On Direct x this multiplication would be model * view * proj
        glm::mat4 mvpA = proj * view * modelA;

        Renderer renderer;

        shader->bind();
        shader->setUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
        shader->setUniformMat4f("u_MVP", mvpA);
        renderer.draw(*vao, *ibo, *shader);

        // One way to draw multiple objects is to apply different mvp as in here, which is fast,
        // another way is to allocate another Vertex Buffer, for something like drawing tiles we want to have one call to
        // save on performance so we use Batch Rendering for that
        glm::mat4 modelB = glm::translate(glm::mat4(1.0f), translationB);
        glm::mat4 mvpB = proj * view * modelB;
        shader->setUniformMat4f("u_MVP", mvpB);
        renderer.draw(*vao, *ibo, *shader);
    }

    void TestTexture2D::onImGuiRender() {
        ImGui::Text("Model Matrix");
        ImGui::SliderFloat("Translation X", &translationA.x, 0.0f, screenWidth);
        ImGui::SliderFloat("Translation Y", &translationA.y, 0.0f, screenHeight);
        ImGui::SliderFloat("Translation Z", &translationA.z, -1.0f, 1.0f);
        ImGui::SliderFloat("Scale", &scale, 0.0f, 5.0f);
        ImGui::SliderFloat("Rotation", &rotation, -180.0f, 180.0f);
        ImGui::Text("View Matrix");
        ImGui::SliderFloat("View Translation X", &cameraTranslation.x, 0.0f, screenWidth);
        ImGui::SliderFloat("View Translation Y", &cameraTranslation.y, 0.0f, screenHeight);
        ImGui::SliderFloat("View Translation Z", &cameraTranslation.z, -1.0f, 1.0f);

    }
}

