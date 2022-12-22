#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>


static unsigned int compileShader(unsigned int type, const std::string& source) {

    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();

    glShaderSource(id, 1, &src, nullptr);

    glCompileShader(id);

    // shader error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        // Use alloca() to allocate error message buffer on the stack
        char * message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile shader: " << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader) {

    unsigned int program = glCreateProgram();

    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    // Delete intermediate shaders once they are linked to program
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void) {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "Coult not initialise Glew!" << std::endl;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    // Generate vertex buffers
    unsigned int buffer;
    glGenBuffers(1, &buffer);

    glBindBuffer(GL_ARRAY_BUFFER, buffer);


    const unsigned int VERTICES_NUM = 6;
    float positions [VERTICES_NUM] = {
        -0.5f, -0.5f,
        0.0f, 0.5f,
        0.5f, -0.5f
    };

    glBufferData(GL_ARRAY_BUFFER, VERTICES_NUM * sizeof(float), positions, GL_STATIC_DRAW);

    // Specifying vertex layout below by enabling and configuring vertex vattributes
    // Enable vertex attributes
    glEnableVertexAttribArray(0);
    // Set up vertex attributes (position, colour, texture uv, normals)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0); // position

    // C++11 Raw string literals FTW
    const std::string vertexShader = R"glsl(
        #version 330 core

        layout(location = 0) in vec4 position;

        void main () {
            gl_Position = position;
        }
        )glsl";
    const std::string fragmentShader = R"glsl(
        #version 330 core

        layout(location = 0) out vec4 color;

        void main () {
            color = vec4(1.0, 0.0, 0.0, 1.0);
        }
        )glsl";

    unsigned int shader = createShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // LEGACY Testing code - fixed pipeline, don't use this, unless for testing, debug reasons
        //glBegin(GL_TRIANGLES);
        //glVertex2f(-0.5f, -0.5f);
        //glVertex2f(0.0f, 0.5f);
        //glVertex2f(0.5f, -0.5f);
        //glEnd();

        // Use this if we dont have index buffer
        glDrawArrays(GL_TRIANGLES, 0, 3);


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
