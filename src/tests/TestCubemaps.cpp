#include "TestCubemaps.hpp"

#include "../Renderer.hpp"

#include "glm/gtc/matrix_transform.hpp"

// For glm::to_string()
#include "glm/gtx/string_cast.hpp"
#include <iostream>
#include <map>

#include "imgui/imgui.h"

#include <GLFW/glfw3.h>

namespace test {

    TestCubemaps::TestCubemaps() {
        // TODO: pass screen coords in instead of creating these raw pointers here
        // to get screen measurements
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        screenWidth = mode->width;
        screenHeight = mode->height;
        std::cout << "Screen Width: " << screenWidth << ", Screen Height: " << screenHeight << "\n";

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        //free(monitor);
        //free(mode);

        // position, normal, tex coords
        float positions [] = {
            // FRONT
            -0.5f, -0.5f, -1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // 0
            -0.5f, 0.5f, -1.0f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // 1
            0.5f, 0.5f, -1.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 2
            0.5f, -0.5f, -1.0f,   0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // 3

            // BACK
            -0.5f, -0.5f, -2.0f,  0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // 4
            -0.5f, 0.5f, -2.0f,   0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // 5
            0.5f, 0.5f, -2.0f,    0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // 6
            0.5f, -0.5f, -2.0f,   0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // 7

            // UP
            0.5f, 0.5f, -2.0f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, -2.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, 0.5f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            0.5f, 0.5f, -1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

            // DOWN
            0.5f, -0.5f, -1.0f,  0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, -1.0f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -2.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -2.0f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f,

            // LEFT
            -0.5f, 0.5f, -1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, -2.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -2.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

            // RIGHT
            0.5f, 0.5f, -2.0f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            0.5f, 0.5f, -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -2.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
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
        // 24 vertices with 5 attributes each: 3 position and 3 normal, 2 UV
        vbo = std::make_unique<VertexBuffer>(positions, 24 * 8 * sizeof(float));

        VertexBufferLayout layout;
        layout.push<float>(3); // position
        layout.push<float>(3); // normals
        layout.push<float>(2); // UV

        vao->addBuffer(*vbo, layout);

        // Generate and bind index buffer object
        ibo = std::make_unique<IndexBuffer>(indices, 36);


        // Floor
        // position, normal, tex coords
        float floorPositions [] = {
            5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            5.0f, -0.5f, 5.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -5.0f, -0.5f, 5.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f
        };

        unsigned int floorIndices[] = {
            0, 1, 2,
            2, 3, 0
        };

        floorVao = std::make_unique<VertexArray>();
        floorVbo = std::make_unique<VertexBuffer>(floorPositions, 4 * 8 * sizeof(float));
        floorVao->addBuffer(*floorVbo, layout);
        floorIbo = std::make_unique<IndexBuffer>(floorIndices, 6);

        objectShader = std::make_unique<Shader>("assets/shaders/lightCasters.glsl");
        objectShader->bind();

        diffuseMap = std::make_unique<Texture>("assets/textures/container.png");
        specularMap = std::make_unique<Texture>("assets/textures/container_specular.png");
	// Bind both maps to different slots and set uniforms
        diffuseMap->bind();
	specularMap->bind(1);
        objectShader->setUniform1i("material.diffuseMap", 0);
        objectShader->setUniform1i("material.specularMap", 1);

        lightSourceShader = std::make_unique<Shader>("assets/shaders/lightSourceVariableColor.glsl");

        lightPosition = glm::vec3(1.0f, 2.0f, -10.0f);
        lightDirection = glm::vec3(-0.2f, -1.0f, -0.3f);

        dirLightColor = glm::vec3(1.0f);
        pointLightColor = glm::vec3(1.0f, 0.0f, 0.0f);
        spotLightColor = glm::vec3(1.0f, 1.0f, 0.0f);

        constant = 1.0f;
        linear = 0.35f;
        quadratic = 0.44f;

        //--- Framebuffer Configuration ---///
        //--------------------------------///
        // For screen quad , using NDC coords
        float quadVertices[] = {
            // positions   // texCoords
            1.0f,  1.0f,  1.0f, 1.0f,
            -1.0f, 1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f,  -1.0f,  1.0f, 0.0f
        };

        screenQuadVao = std::make_unique<VertexArray>();
        screenQuadVbo = std::make_unique<VertexBuffer>(quadVertices, 4 * 4 * sizeof(float));

        VertexBufferLayout layout2d;
        layout2d.push<float>(2);
        layout2d.push<float>(2);

        screenQuadVao->addBuffer(*screenQuadVbo, layout2d);
        floorIbo->bind();

        // Setup skybox

        float skyboxVertices[] = {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,

            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,
        };

        skyboxVao = std::make_unique<VertexArray>();
        skyboxVbo = std::make_unique<VertexBuffer>(skyboxVertices, 24 * 3 * sizeof(float));

        VertexBufferLayout skyboxLayout;
        skyboxLayout.push<float>(3);

        skyboxVao->addBuffer(*skyboxVbo, skyboxLayout);

        std::vector<std::string> faces{
            "assets/textures/skybox/right.jpg",
            "assets/textures/skybox/left.jpg",
            "assets/textures/skybox/top.jpg",
            "assets/textures/skybox/bottom.jpg",
            "assets/textures/skybox/front.jpg",
            "assets/textures/skybox/back.jpg"
        };

        cubemapTexture = std::make_unique<Texture>(faces);
        cubemapShader = std::make_unique<Shader>("assets/shaders/cubemap.glsl");

        cubemapShader->bind();
        cubemapShader->setUniform1i("cubemap", 0);
    }

    TestCubemaps::~TestCubemaps() {
        glDisable(GL_DEPTH_TEST);
    }

    void TestCubemaps::onUpdate(float deltaTime) {
    }

    void TestCubemaps::onRender() {

        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        diffuseMap->bind(); // bound to default slot 0
	specularMap->bind(1);

        // First param - FOV could be changed for zooming effect
        // 2nd param - aspect ratio
        // 3rd and 4th params - near and far planes
        proj = glm::perspective(glm::radians(camera->Zoom), (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);

        glm::mat4 view = glm::mat4(1.0f);
        view = camera->getViewMatrix();

        glm::mat4 model;

        glm::vec3 cubePositions[] = {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(3.0f, 0.0f, -15.0f),
            glm::vec3(-5.5f, 0.0f, -2.5f)
        };

        Renderer renderer;

        objectShader->bind();
        objectShader->setUniform1f("material.shininess", 32.0f);

        objectShader->setUniformVec3("dirLight.direction", lightDirection);
        objectShader->setUniformVec3("dirLight.ambient", dirLightColor * glm::vec3(0.1f));
        objectShader->setUniformVec3("dirLight.diffuse", dirLightColor * glm::vec3(0.5f));
        objectShader->setUniform3f("dirLight.specular", 1.0f, 1.0f, 1.0f);

        objectShader->setUniformVec3("pointLight.position", lightPosition);
        objectShader->setUniformVec3("pointLight.ambient", pointLightColor * glm::vec3(0.1f));
        objectShader->setUniformVec3("pointLight.diffuse", pointLightColor * glm::vec3(0.2f));
        objectShader->setUniform3f("pointLight.specular", 1.0f, 1.0f, 1.0f);
        objectShader->setUniform1f("pointLight.constant", constant);
        objectShader->setUniform1f("pointLight.linear", linear);
        objectShader->setUniform1f("pointLight.quadratic", quadratic);

        // Make spot light position same as camera thus simulating flashlight!
        objectShader->setUniformVec3("spotLight.position", camera->Position);
        objectShader->setUniformVec3("spotLight.direction", camera->Front);
        objectShader->setUniform1f("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        objectShader->setUniformVec3("spotLight.ambient", spotLightColor * glm::vec3(0.2f));
        objectShader->setUniformVec3("spotLight.diffuse", spotLightColor * glm::vec3(0.9f));
        objectShader->setUniform3f("spotLight.specular", 1.0f, 1.0f, 1.0f);

        objectShader->setUniformVec3("viewPosition", camera->Position);

        objectShader->setUniformMat4f("projection", proj);
        objectShader->setUniformMat4f("view", view);

        // Render floor
        model = glm::translate(glm::mat4(1.0f), cubePositions[0]);
        model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
        objectShader->setUniformMat4f("model", model);
        renderer.draw(*floorVao, *floorIbo, *objectShader);

        // Render containers
        for (unsigned int i = 0; i < 3; i++) {
            model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
            objectShader->setUniformMat4f("model", model);
            renderer.draw(*vao, *ibo, *objectShader);
        }

        // Translate same cube and use light source shader to produce light source cube
        lightSourceShader->bind();
        model = glm::translate(model, lightPosition);
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

        glm::mat4 mvp = proj * view * model;
        lightSourceShader->setUniformMat4f("u_MVP", mvp);
        lightSourceShader->setUniform3f("lightColor", pointLightColor.x, pointLightColor.y, pointLightColor.z);
        renderer.draw(*vao, *ibo, *lightSourceShader);

        // Render skybox
        // Could be drawn before everything else, but that would be much slower because all of it would be rendered
        // We render skybox last and set depth func to equal because in skybox shader we will set depth calues to Z
        // and skybox fragments wont be drawn if depth is less than 1.0 meaning there are objects in front of the skybox
        glDepthFunc(GL_EQUAL);
        cubemapShader->bind();
        cubemapShader->setUniformMat4f("projection", proj);
        glm::mat4 skyboxViewMat = glm::mat4(glm::mat3(view));
        cubemapShader->setUniformMat4f("view", skyboxViewMat);
        cubemapTexture->bind();
        renderer.draw(*skyboxVao, *ibo, *cubemapShader);
        glDepthFunc(GL_LESS); // Back to default
    }

    void TestCubemaps::onImGuiRender() {
    }
}

