#include "TestInstancing.hpp"

#include "../Renderer.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
#include "imgui/imgui.h"
#include <GLFW/glfw3.h>

namespace test {

    const float NUM_CUBES = pow(10, 3);
    const int NUM_ASTEROIDS = 20000;

    TestInstancing::TestInstancing() {

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        screenWidth = mode->width;
        screenHeight = mode->height;

        // Enable depth testing
        glEnable(GL_DEPTH_TEST);

        // Setup data for instanced array for 1000 cubes rendered in one instanced call!
        int index = 0;
        float offset = -20.0f;
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

        // Setup transformations for asteroid used in instanced call
        //asteroidTransforms = new glm::mat4[NUM_ASTEROIDS];
        //srand(static_cast<unsigned int>(glfwGetTime())); // random seed
        //float radius = 150.0;
        //offset = 25.0;
        //for (int i = 0; i < NUM_ASTEROIDS; i++) {
            //glm::mat4 model = glm::mat4(1.0f);
            //// translation, displace around the circle with radius
            //float angle = (float)i / (float)NUM_ASTEROIDS * 360.0f;
            //float displacement = (rand() % (int)(2 * offset * 100)) / 100.f - offset;
            //float x = sin(angle) * radius + displacement;
            //displacement = (rand() % (int)(2 * offset * 100)) / 100.f - offset;
            //float y = displacement * 0.4f;
            //displacement = (rand() % (int)(2 * offset * 100)) / 100.f - offset;
            //float z = cos(angle) * radius + displacement;
            //model = glm::translate(model, glm::vec3(x, y, z));

            //float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
            //model = glm::scale(model, glm::vec3(scale));

            //float rotAngle = static_cast<float>((rand() % 360));
            //model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

            //asteroidTransforms[i] = model;
        //}
        asteroidTransforms = new glm::mat4[NUM_ASTEROIDS];
        srand(glfwGetTime()); // initialize random seed
        float radius = 35.0;
        offset = 5.5f;
        for(unsigned int i = 0; i < NUM_ASTEROIDS; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            // 1. translation: displace along circle with 'radius' in range [-offset, offset]
            float angle = (float)i / (float)NUM_ASTEROIDS * 360.0f;
            float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float x = sin(angle) * radius + displacement;
            displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
            displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float z = cos(angle) * radius + displacement;
            model = glm::translate(model, glm::vec3(x, y, z));

            // 2. scale: scale between 0.05 and 0.25f
            float scale = (rand() % 20) / 100.0f + 0.05;
            model = glm::scale(model, glm::vec3(scale));

            // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
            float rotAngle = (rand() % 360);
            model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

            // 4. now add to list of matrices
            asteroidTransforms[i] = model;
        }

        // Cube Positions and UVs
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
        instanceVbo = std::make_unique<VertexBuffer>(cubePositions, sizeof(glm::vec3) * NUM_CUBES);
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

        rockModel = std::make_unique<Model>("assets/models/rock.obj");
        planetModel = std::make_unique<Model>("assets/models/planet.obj");

        instanceMatrixShader = std::make_unique<Shader>("assets/shaders/instanceMatrix.glsl"); // For asteroids
        mvpTextureShader = std::make_unique<Shader>("assets/shaders/cube_textured.glsl"); // For planet

        asteroidInstanceVbo = std::make_unique<VertexBuffer>(&asteroidTransforms[0], NUM_ASTEROIDS * sizeof(glm::mat4));

        for (unsigned int i = 0; i < rockModel->getMeshes()->size(); i++) {
            (*rockModel->getMeshes())[i].getVao()->bind();
            //asteroidInstanceVbo->bind();

            std::size_t vec4Size = sizeof(glm::vec4);
            // Vertex attributes pointers can only be up to vec4 in size, so we split
            // mat4 into 4 vertex attribs
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

            glVertexAttribDivisor(2, 1);
            glVertexAttribDivisor(3, 1);
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);
            (*rockModel->getMeshes())[i].getVao()->unbind();
        }
    }

    void TestInstancing::onRender() {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        texture->bind(); // bound to default slot 0

        proj = glm::perspective(glm::radians(camera->Zoom), (float)screenWidth/(float)screenHeight, 0.1f, 1000.0f);

        Renderer renderer;

        shader->bind();
        // MVP gets multiplied in reverse order here, because OpenGL stores matrices in column order
        // On Direct x this multiplication would be model * view * proj
        shader->setUniformMat4f("u_MVP", proj * camera->getViewMatrix());
        renderer.drawInstanced(*vao, *ibo, *shader, NUM_CUBES);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f));
        mvpTextureShader->bind();
        mvpTextureShader->setUniform1i("u_texture", 0);
        mvpTextureShader->setUniformMat4f("u_MVP", proj * camera->getViewMatrix());
        planetModel->draw(*mvpTextureShader);

        instanceMatrixShader->bind();
        instanceMatrixShader->setUniformMat4f("u_MVP", proj * camera->getViewMatrix());
        instanceMatrixShader->setUniform1i("u_texture", 0);
        for (unsigned int i = 0; i < rockModel->getMeshes()->size(); i++) {
            (*rockModel->getMeshes())[i].getVao()->bind();
            // NOTE: Easily 60FPS with over 20k asteroids!
            // Starts lagging at around 50k
            (*rockModel->getMeshes())[i].drawInstanced(*instanceMatrixShader, NUM_ASTEROIDS);
            (*rockModel->getMeshes())[i].getVao()->unbind();
        }
    }

    void TestInstancing::onImGuiRender() {
        ImGui::SliderFloat("Camera pos X", &camera->Position.x, -1000.0f, 1000.0f);
        ImGui::SliderFloat("Camera pos Y", &camera->Position.y, -1000.0f, 1000.0f);
        ImGui::SliderFloat("Camera pos Z", &camera->Position.z, -1000.0f, 1000.0f);
    }
}

