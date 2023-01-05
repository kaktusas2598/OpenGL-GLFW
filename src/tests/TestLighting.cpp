#include "TestLighting.hpp"

#include "../Renderer.hpp"

#include "glm/gtc/matrix_transform.hpp"

// For glm::to_string()
#include "glm/gtx/string_cast.hpp"
#include <iostream>

#include "imgui/imgui.h"

#include <GLFW/glfw3.h>

namespace test {

    TestLighting::TestLighting() {
        // TODO: pass screen coords in instead of creating these raw pointers here
        // to get screen measurements
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        screenWidth = mode->width;
        screenHeight = mode->height;

        // Enable depth testing
        glEnable(GL_DEPTH_TEST);

        //free(monitor);
        //free(mode);

        // position, normal
        float positions [] = {
            // FRONT
            -0.5f, -0.5f, -1.0f,  0.0f, 0.0f, 1.0f, //1.0f, 1.0f, // 0
            -0.5f, 0.5f, -1.0f,  0.0f, 0.0f, 1.0f, // 0.0f, 1.0f, // 1
            0.5f, 0.5f, -1.0f,   0.0f, 0.0f, 1.0f, // 0.0f, 0.0f, // 2
            0.5f, -0.5f, -1.0f,   0.0f, 0.0f, 1.0f, //1.0f, 0.0f, // 3

            // BACK
            -0.5f, -0.5f, -2.0f,  0.0f, 0.0f, -1.0f, //1.0f, 1.0f, // 4
            -0.5f, 0.5f, -2.0f,   0.0f, 0.0f, -1.0f, //0.0f, 1.0f, // 5
            0.5f, 0.5f, -2.0f,    0.0f, 0.0f, -1.0f, //0.0f, 0.0f, // 6
            0.5f, -0.5f, -2.0f,   0.0f, 0.0f, -1.0f, //1.0f, 0.0f, // 7

            // UP
            0.5f, 0.5f, -2.0f,  0.0f, 1.0f, 0.0f, //1.0f, 1.0f,
            -0.5f, 0.5f, -2.0f,  0.0f, 1.0f, 0.0f, //0.0f, 1.0f,
            -0.5f, 0.5f, -1.0f,  0.0f, 1.0f, 0.0f, //0.0f, 0.0f,
            0.5f, 0.5f, -1.0f,  0.0f, 1.0f, 0.0f, //1.0f, 0.0f,

            // DOWN
            0.5f, -0.5f, -1.0f,  0.0f, -1.0f, 0.0f, //1.0f, 1.0f,
            -0.5f, -0.5f, -1.0f,  0.0f, -1.0f, 0.0f, //0.0f, 1.0f,
            -0.5f, -0.5f, -2.0f,  0.0f, -1.0f, 0.0f, //0.0f, 0.0f,
            0.5f, -0.5f, -2.0f,  0.0f, -1.0f, 0.0f, //1.0f, 0.0f,

            // LEFT
            -0.5f, 0.5f, -1.0f,  -1.0f, 0.0f, 0.0f, //1.0f, 1.0f,
            -0.5f, 0.5f, -2.0f,  -1.0f, 0.0f, 0.0f, //0.0f, 1.0f,
            -0.5f, -0.5f, -2.0f, -1.0f, 0.0f, 0.0f, //0.0f, 0.0f,
            -0.5f, -0.5f, -1.0f, -1.0f, 0.0f, 0.0f, //1.0f, 0.0f,

            // RIGHT
            0.5f, 0.5f, -2.0f,  1.0f, 0.0f, 0.0f, //1.0f, 1.0f,
            0.5f, 0.5f, -1.0f,  1.0f, 0.0f, 0.0f, //0.0f, 1.0f,
            0.5f, -0.5f, -1.0f, 1.0f, 0.0f, 0.0f, //0.0f, 0.0f,
            0.5f, -0.5f, -2.0f, 1.0f, 0.0f, 0.0f, //1.0f, 0.0f,
        };

        unsigned int indices[] = {
            // FRONT
            0, 1, 2,
            2, 3, 0,
            // BACK
            4, 5, 6,
            6, 7, 4,
            // UP
            8, 9, 10,
            10, 11, 8,
            // DOWN
            12, 13, 14,
            14, 15, 12,
            // LEFT
            16, 17, 18,
            18, 19, 16,
            // RIGHT
            20, 21, 22,
            22, 23, 20
        };

        vao = std::make_unique<VertexArray>();
        // 24 vertices with 5 attributes each: 3 position and 3 normal
        vbo = std::make_unique<VertexBuffer>(positions, 24 * 6 * sizeof(float));

        VertexBufferLayout layout;
        layout.push<float>(3); // position
        layout.push<float>(3); // normals

        vao->addBuffer(*vbo, layout);

        // Generate and bind index buffer object
        ibo = std::make_unique<IndexBuffer>(indices, 36);


        lightingShader = std::make_unique<Shader>("assets/shaders/lighting.glsl");
        lightingShader->bind();

        texture = std::make_unique<Texture>("assets/textures/dirt.png");
        texture->bind(); // bound to default slot 0
        // Set uniform to tell shader that we need to sample texture from slot 0
        lightingShader->setUniform1i("u_Texture", 0);

        lightSourceShader = std::make_unique<Shader>("assets/shaders/lightSource.glsl");

        lightPosition = glm::vec3(1.0f, 2.0f, -5.0f);
    }

    TestLighting::~TestLighting() {
    }

    void TestLighting::onUpdate(float deltaTime) {
    }

    void TestLighting::onRender() {
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        texture->bind(); // bound to default slot 0


        // First param - FOV could be changed for zooming effect
        // 2nd param - aspect ratio
        // 3rd and 4th params - near and far planes
        proj = glm::perspective(glm::radians(camera->Zoom), (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);

        glm::mat4 view = glm::mat4(1.0f);
        view = camera->getViewMatrix();

        // Creating model matrix for transformations
        //glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5, 1.0, 0.0));
        // If we applied a non-uniform scale here, it would change direction for normals and
        // thus would distorn lighting, to solve that we could use different model matrix
        // for lighting called - normal matrix
        model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

        Renderer renderer;

        lightingShader->bind();
        lightingShader->setUniform3f("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader->setUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader->setUniform3f("lightPosition", lightPosition.x, lightPosition.y, lightPosition.z);
        lightingShader->setUniform3f("viewPosition", camera->Position.x, camera->Position.y, camera->Position.z);
        lightingShader->setUniformMat4f("projection", proj);
        lightingShader->setUniformMat4f("view", view);
        lightingShader->setUniformMat4f("model", model);
        renderer.draw(*vao, *ibo, *lightingShader);

        // Translate same cube and use light source shader to produce light source cube
        lightSourceShader->bind();
        model = glm::translate(model, lightPosition);
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

        glm::mat4 mvp = proj * view * model;
        lightSourceShader->setUniformMat4f("u_MVP", mvp);
        renderer.draw(*vao, *ibo, *lightSourceShader);
    }

    void TestLighting::onImGuiRender() {
        ImGui::Text("Light Position");
        ImGui::SliderFloat("X", &lightPosition.x, -10.0f, 10.0f);
        ImGui::SliderFloat("Y", &lightPosition.y, -10.0f, 10.0f);
        ImGui::SliderFloat("Z", &lightPosition.z, -10.0f, 10.0f);
    }
}

