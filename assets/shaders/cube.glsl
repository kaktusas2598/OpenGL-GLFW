#shader vertex
#version 330 core

// From vertex buffer: vertex attributes
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

// Pass on to fragment shader
out vec4 v_color;

uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * position;
    // Assing outgoing params for pipeline
    v_color = color;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_color;


void main() {
    color = v_color;
}
