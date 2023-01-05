#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Renderer.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include "Texture.hpp"

#include "Camera.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
// For glm::to_string()
#include "glm/gtx/string_cast.hpp"

#include "tests/TestClearColor.hpp"
#include "tests/TestTexture2D.hpp"
#include "tests/TestBatchRendering.hpp"
#include "tests/TestDynamicBatchRendering.hpp"
#include "tests/TestCube3D.hpp"
#include "tests/TestTexturedCube.hpp"
#include "tests/TestCamera.hpp"
#include "tests/TestCameraClass.hpp"
#include "tests/TestLighting.hpp"

float lastX, lastY;
bool firstMouse;
Camera* camera = nullptr;

void mouseCallback(GLFWwindow* window, double xPos, double yPos) {
    // To prevent sudden camera jamp when mouse callback first gets called upon entering screen
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos; // Reversed because Y axis goes from bottom to top in opengl

    lastX = xPos;
    lastY = yPos;
    if (camera != nullptr)
        camera->processMouseMovement(xOffset, yOffset);
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    if (camera != nullptr)
        camera->processMouseScroll(yOffset);
}

void processInput(GLFWwindow* window, Camera* camera, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Camera up and down, same controls as in minecraft creative
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (camera != nullptr)
            camera->Position.y += 2.5f * deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        if (camera != nullptr)
            camera->Position.y -= 2.5f * deltaTime;
    }


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->processKeyboard(CameraMovement::FORWARD, deltaTime);
    } if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->processKeyboard(CameraMovement::BACKWARD, deltaTime);
    } if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->processKeyboard(CameraMovement::LEFT, deltaTime);
    } if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->processKeyboard(CameraMovement::RIGHT, deltaTime);
    }

    // Disable and enable cursor for fps experience
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        switch (glfwGetInputMode(window, GLFW_CURSOR)) {
            case GLFW_CURSOR_NORMAL:
                //Hide cursor for 3D camera
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                break;
            case GLFW_CURSOR_DISABLED:
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                break;
            default:
                break;
        }
    }
}

int main(void) {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // Enable GL Core Profile in GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    const int screenWidth = mode->width;
    const int screenHeight = mode->height;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(mode->width, mode->height, "OpenGL test", monitor, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Set callbacks
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    // For camera
    lastX = screenWidth / 2.0f;
    lastY = screenHeight / 2.0f;
    firstMouse = true;

    glfwSwapInterval(1); // Enable vsync

    if (glewInit() != GLEW_OK) {
        std::cout << "Coult not initialise Glew!" << std::endl;
    }

    // imgui gl context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;


    //imgui style
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");


    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    Renderer renderer;
    bool show_demo_window = false;
    bool wireframe_mode = false;

    test::Test* currentTest = nullptr;
    test::TestMenu* testMenu = new test::TestMenu(currentTest);
    currentTest = testMenu;

    testMenu->registerTest<test::TestClearColor>("Clear Color");
    testMenu->registerTest<test::TestTexture2D>("2D Textures");
    testMenu->registerTest<test::TestBatchRendering>("Batch rendering");
    testMenu->registerTest<test::TestDynamicBatchRendering>("Batch rendering (dynamic geometry)");
    testMenu->registerTest<test::TestCube3D>("3D Cube");
    testMenu->registerTest<test::TestTexturedCube>("3D Cube (textured)");
    testMenu->registerTest<test::TestCamera>("Camera Example");
    testMenu->registerTest<test::TestCameraClass>("Fly Like Camera Demo");
    testMenu->registerTest<test::TestLighting>("Basic Lighting");

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window, camera, deltaTime);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        renderer.clear();

        if (currentTest) {
            // Had to set camera on every frame to make it work, this is obviously horrible, but the
            // purpose of this projec is to learn opengl and create different test scenarios,
            // so for now I will let this slide
            // Why does this break batch tests??? They aren't even using camera, WTF
            currentTest->setCamera(camera);
            currentTest->processInput(window, deltaTime);
            currentTest->onUpdate(deltaTime);
            currentTest->onRender();
            ImGui::Begin("Test");
            if (currentTest != testMenu && ImGui::Button("<-")) {
                delete currentTest;
                currentTest = testMenu;
            }
            currentTest->onImGuiRender(); // Must be after new imgui frame

            if(wireframe_mode)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            ImGui::Checkbox("Wireframe mode", &wireframe_mode);
            ImGui::Checkbox("Demo Window", &show_demo_window);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            if(ImGui::Button("Close Application"))
                glfwSetWindowShouldClose(window, 1);

            ImGui::End();
        }

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    delete camera;
    delete currentTest;
    if (currentTest != testMenu)
        delete testMenu;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    // If allocating index and vertex buffers on the heap, delete them here
    // currently they are allocated on stack and this causes program to hand due to gl get error loop
    // complaining about no context

    glfwTerminate();
    return 0;
}
