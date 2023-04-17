#include "TestInstancing.hpp"

#include "../Renderer.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
#include "imgui/imgui.h"
#include <GLFW/glfw3.h>

namespace test {

    TestInstancing::TestInstancing() {

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        screenWidth = mode->width;
        screenHeight = mode->height;

        // Enable depth testing
        glEnable(GL_DEPTH_TEST);

        // Setup data for instanced array
        int index = 0;
        float offset = 1.0f;
        for (int y = -50; y < 50; y += 10) {
            for (int x = -50; x < 50; x += 10) {
                for (int z = -50; z < 50; z += 10) {
                    glm::vec3 translation;
                    translation.x = (float)x / 5.0f + offset;
                    translation.y = (float)y / 5.0f + offset;
                    translation.z = (float)z / 5.0f + offset;
                    cubePositions[index++] = translation;
                }
            }
        }

        // x, y, z, tex.s, tex.t
        float positions [] = {
            // FRONT
            -0.5f, -0.5f, -1.0f, 1.0f, 1.0f, // 0
            -0.5f, 0.5f, -1.0f,  0.0f, 1.0f, // 1
            0.5f, 0.5f, -1.0f,   0.0f, 0.0f, // 2
            0.5f, -0.5f, -1.0f,  1.0f, 0.0f, // 3

            // BACK
            -0.5f, -0.5f, -2.0f, 1.0f, 1.0f, // 4
            -0.5f, 0.5f, -2.0f,  0.0f, 1.0f, // 5
            0.5f, 0.5f, -2.0f,   0.0f, 0.0f, // 6
            0.5f, -0.5f, -2.0f,  1.0f, 0.0f, // 7

            // UP
            0.5f, 0.5f, -2.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, -2.0f, 0.0f, 1.0f,
            -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, -1.0f, 1.0f, 0.0f,

            // DOWN
            0.5f, -0.5f, -1.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, -1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -2.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -2.0f, 1.0f, 0.0f,

            // LEFT
            -0.5f, 0.5f, -1.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, -2.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -2.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -1.0f, 1.0f, 0.0f,

            // RIGHT
            0.5f, 0.5f, -2.0f, 1.0f, 1.0f,
            0.5f, 0.5f, -1.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -2.0f, 1.0f, 0.0f,
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
        // 24 vertices with 5 attributes each: 3 position and 2 UV
        vbo = std::make_unique<VertexBuffer>(positions, 24 * 5 * sizeof(float));

        VertexBufferLayout layout;
        layout.push<float>(3); // position
        layout.push<float>(2); // texture coords

        vao->addBuffer(*vbo, layout);

        // Set up vertex attribute for instanced array and bind this buffer to same VAO as well
        // TODO: should be done in OO way, but some of classes like VertexBufferLayout would have to change quite
        // a lot mainly because you would need to specify not just AttribDivisor but layout location offset
        // as well (if instanced arrays are done in separate buffer)
        instanceVbo = std::make_unique<VertexBuffer>(cubePositions, sizeof(glm::vec3) * 1000);
        glEnableVertexAttribArray(2);
        instanceVbo->bind();
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glVertexAttribDivisor(2, 1); // Telling OpenGL that this attribute is instanced array

        // Generate and bind index buffer object
        ibo = std::make_unique<IndexBuffer>(indices, 36);


        shader = std::make_unique<Shader>("assets/shaders/instancing.glsl");
        shader->bind();

        texture = std::make_unique<Texture>("assets/textures/dirt.png");
        texture->bind(); // bound to default slot 0
        // Set uniform to tell shader that we need to sample texture from slot 0
        shader->setUniform1i("u_texture", 0);

    }

    void TestInstancing::onRender() {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        texture->bind(); // bound to default slot 0

        proj = glm::perspective(glm::radians(camera->Zoom), (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);

        Renderer renderer;

        shader->bind();
        // MVP gets multiplied in reverse order here, because OpenGL stores matrices in column order
        // On Direct x this multiplication would be model * view * proj
        shader->setUniformMat4f("u_MVP", proj * camera->getViewMatrix());
        renderer.drawInstanced(*vao, *ibo, *shader, 1000);
    }
}

