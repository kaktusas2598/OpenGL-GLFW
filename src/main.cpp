#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Renderer.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include "Texture.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
// For glm::to_string()
//#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"

const int screenWidth = 1024;
const int screenHeight = 768;

int main(void) {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // Enable GL Core Profile in GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL test", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        std::cout << "Coult not initialise Glew!" << std::endl;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

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
    VertexArray va;
    VertexBuffer vb(positions, 4 * 4 * sizeof(float));

    VertexBufferLayout layout;
    layout.push<float>(2); // position
    layout.push<float>(2); // texture coords

    va.addBuffer(vb, layout);

    // Generate and bind index buffer object
    IndexBuffer ib(indices, 6);

    // Create 4:3 aspect ratio projection ortographic matrix
    // This helps us use easier coords system, and it gets converted back to GL system in vertex shader, by multypling
    // each vertex position by this matrix
    glm::mat4 proj = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight, -1.0f, 1.0f);
    // Creating view matrix, first param here is identity view matrix
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0));
    // Creating model matrix for transformations
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200, 200, 0));

    // MVP gets multiplied in reverse order here, because OpenGL stores matrices in column order
    // On Direct x this multiplication would be model * view * proj
    glm::mat4 mvp = proj * view * model;

    // Demonstration of how our vertex position goes back to gl coords system by multiplying
    glm::vec4 vp(100.0f, 100.0f, 0.0f, 1.0f);
    glm::vec4 result = proj * vp;
    std::cout << "Vertex position as defined by us: "<< glm::to_string(vp) << std::endl;
    std::cout << "Vertex position after being multiplied with projection matrix: "<< glm::to_string(result) << std::endl;

    Shader shader("assets/shaders/shader.glsl");
    shader.bind();
    shader.setUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
    shader.setUniformMat4f("u_MVP", mvp);


    Texture texture("assets/textures/slime.png");
    texture.bind(); // bound to default slot 0
    // Set uniform to tell shader that we need to sample texture from slot 0
    shader.setUniform1i("u_Texture", 0);

    // Unbind everything after setting it up, then bind again before drawing
    va.unbind();
    vb.unbind();
    ib.unbind();
    shader.unbind();

    float r = 0.0f;
    float inc = 0.05f;

    Renderer renderer;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        renderer.clear();

        shader.bind();
        shader.setUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

        renderer.draw(va, ib, shader);

        if (r > 1.0f) {
            inc = -0.05f;
        } else if (r < 0.0f) {
            inc = 0.05f;
        }
        r += inc;

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // If allocating index and vertex buffers on the heap, delete them here
    // currently they are allocated on stack and this causes program to hand due to gl get error loop
    // complaining about no context

    glfwTerminate();
    return 0;
}
