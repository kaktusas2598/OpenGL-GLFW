#include "TestCamera.hpp"

#include "../Renderer.hpp"

#include "glm/gtc/matrix_transform.hpp"
// For glm::to_string()
#include "glm/gtx/string_cast.hpp"

#include "imgui/imgui.h"

#include <GLFW/glfw3.h>

namespace test {

    TestCamera::TestCamera()
        : translation(0, 0, 0), cameraTranslation(0, 0, 0), scale(0.5), rotation(45.0f) {

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
        vbo = std::make_unique<VertexBuffer>(positions, 24 * 5 * sizeof(float));

        VertexBufferLayout layout;
        layout.push<float>(3); // position
        layout.push<float>(2); // texture coords

        vao->addBuffer(*vbo, layout);

        // Generate and bind index buffer object
        ibo = std::make_unique<IndexBuffer>(indices, 36);


        shader = std::make_unique<Shader>("assets/shaders/cube_textured.glsl");
        shader->bind();

        texture = std::make_unique<Texture>("assets/textures/dirt.png");
        texture->bind(); // bound to default slot 0
        // Set uniform to tell shader that we need to sample texture from slot 0
        shader->setUniform1i("u_Texture", 0);
    }

    TestCamera::~TestCamera() {
    }

    void TestCamera::onUpdate(float deltaTime) {
    }

    void TestCamera::onRender() {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        texture->bind(); // bound to default slot 0

        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget); // This vector actually points in the opposite direction

        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

        glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);


        // Transformations for cube we will draw them using 10 draw calls too
        glm::vec3 cubePositions[] = {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 5.0f, -15.0f),
            glm::vec3(-1.5f, 5.0f, -2.5f),
            glm::vec3(-3.8, -2.2, -12.3),
            glm::vec3(-2.4, -0.4f, -3.5f),
            glm::vec3(-1.7f, 3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f, 2.0f, -2.5f),
            glm::vec3(1.5f, 0.2f, -1.5f),
            glm::vec3(-1.3f, 1.0f, -1.5f)
        };

        // First param - FOV could be changed for zooming effect
        // 2nd param - aspect ratio
        // 3rd and 4th params - near and far planes
        proj = glm::perspective(glm::radians(rotation), (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);

        // Make "camera" rotate around the scene
        float radius = 10.0f;
        float camX = static_cast<float>(sin(glfwGetTime()) * radius);
        float camZ = static_cast<float>(cos(glfwGetTime()) * radius);
        // Creating view matrix, first param here is identity view matrix
        //glm::mat4 view = glm::translate(glm::mat4(1.0f), cameraTranslation);
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(glm::vec3(camX, 0.0, camZ), cameraTarget, up);
        //view = glm::lookAt(cameraPos, cameraTarget, up);

        for (unsigned int i = 0; i < 10; i++) {
            // Creating model matrix for transformations
            glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
            model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5, 1.0, 0.0));
            model = glm::scale(model, glm::vec3(scale, scale, scale));

            // MVP gets multiplied in reverse order here, because OpenGL stores matrices in column order
            // On Direct x this multiplication would be model * view * proj
            glm::mat4 mvp = proj * view * model;

            Renderer renderer;

            shader->bind();
            shader->setUniform4f("u_Color", 1.0f, 0.3f, 0.8f, 1.0f);
            shader->setUniformMat4f("u_MVP", mvp);
            renderer.draw(*vao, *ibo, *shader);
        }
    }

    void TestCamera::onImGuiRender() {
        ImGui::Text("Model Matrix");
        ImGui::SliderFloat("Scale", &scale, 0.0f, 5.0f);
        ImGui::Text("View Matrix");
        ImGui::SliderFloat("View Translation X", &cameraTranslation.x, -1.0f, 1.0f);
        ImGui::SliderFloat("View Translation Y", &cameraTranslation.y, -1.0f, 1.0f);
        ImGui::SliderFloat("View Translation Z", &cameraTranslation.z, -1.0f, 1.0f);
        ImGui::Text("Projection Matrix");
        ImGui::SliderFloat("Zoom (FOV)", &rotation, -180.0f, 180.0f);

    }
}

