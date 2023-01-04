#shader vertex
#version 330 core

// From vertex buffer: vertex attributes
layout(location = 0) in vec4 position;

uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * position;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {
    float ambientStrenght = 0.1;
    vec3 ambient = ambientStrenght * lightColor;

    color = vec4(ambient * objectColor, 1.0);
}
