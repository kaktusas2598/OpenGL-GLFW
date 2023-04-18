#include "TestCube3D.hpp"

#include "../Renderer.hpp"

#include "glm/gtc/matrix_transform.hpp"
// For glm::to_string()
#include "glm/gtx/string_cast.hpp"

#include "imgui/imgui.h"

#include <GLFW/glfw3.h>

namespace test {

    TestCube3D::TestCube3D()
        : translation(0, 0, 0), cameraTranslation(0, 0, 0), scale(0.5), rotation(90.0f) {

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

        // So tet coords are useless here if using only 8 vertices, because tex coords
        // cannot be duplicated per vertex, so we need here minimally 24 vertices, 4 per face and we can define
        // texture for each face of cube then
        // 1st 2 floats position, 2nd two - tex coords
        float positions [] = {
            -0.5f, -0.5f, -2.0f, 1.0f, 0.0f, 0.0f, 1.0f, // 0
            -0.5f, 0.5f, -2.0f, 0.0f, 1.0f, 0.0f, 1.0f, // 1
            0.5f, 0.5f, -2.0f, 0.0f, 0.0f, 1.0f, 1.0f, // 2
            0.5f, -0.5f, -2.0f, 0.0f, 1.0f, 0.0f, 1.0f,// 3
	    -0.5f, -0.5f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // 4
            -0.5f, 0.5f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, // 5
            0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // 6
            0.5f, -0.5f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f // 7

        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0,
            4, 5, 6,
            6, 7, 4,
            0, 1, 5,
            5, 4, 0,
            3, 2, 6,
            6, 7, 3,
            1, 5, 6,
            6, 2, 1,
            0, 4, 7,
            7, 3, 0
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
        vbo = std::make_unique<VertexBuffer>(positions, 8 * 7 * sizeof(float));

        VertexBufferLayout layout;
        layout.push<float>(3); // position
        layout.push<float>(4); // colours

        vao->addBuffer(*vbo, layout);

        // Generate and bind index buffer object
        ibo = std::make_unique<IndexBuffer>(indices, 36);

        // First param - FOV could be changed for zooming effect
        // 2nd param - aspect ratio
        // 3rd and 4th params - near and far planes
        proj = glm::perspective(glm::radians(45.0f), (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);

        shader = std::make_unique<Shader>("assets/shaders/cube.glsl");
        shader->bind();

        texture = std::make_unique<Texture>("assets/textures/slime.png");
        texture->bind(); // bound to default slot 0
        // Set uniform to tell shader that we need to sample texture from slot 0
        shader->setUniform1i("u_Texture", 0);

    }

    TestCube3D::~TestCube3D() {}

    void TestCube3D::onUpdate(float deltaTime) {
    }

    void TestCube3D::onRender() {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

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
        shader->setUniform4f("u_Color", 1.0f, 0.3f, 0.8f, 1.0f);
        shader->setUniformMat4f("u_MVP", mvp);
        renderer.draw(*vao, *ibo, *shader);
	}

    void TestCube3D::onImGuiRender() {
        ImGui::Text("Model Matrix");
        ImGui::SliderFloat("Translation X", &translation.x, -1.0f, 1.0f);
        ImGui::SliderFloat("Translation Y", &translation.y, -1.0f, 1.0f);
        ImGui::SliderFloat("Translation Z", &translation.z, -1.0f, 1.0f);
        ImGui::SliderFloat("Scale", &scale, 0.0f, 5.0f);
        ImGui::SliderFloat("Rotation", &rotation, -180.0f, 180.0f);
        ImGui::Text("View Matrix");
        ImGui::SliderFloat("View Translation X", &cameraTranslation.x, -1.0f, 1.0f);
        ImGui::SliderFloat("View Translation Y", &cameraTranslation.y, -1.0f, 1.0f);
        ImGui::SliderFloat("View Translation Z", &cameraTranslation.z, -1.0f, 1.0f);

    }
}

