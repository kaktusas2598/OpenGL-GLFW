#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in mat4 instanceMatrix;

out vec2 v_texCoord;

uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * instanceMatrix * vec4(position, 1.0);
    v_texCoord = texCoords;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_texCoord;

uniform sampler2D u_texture;

void main() {
    color = texture(u_texture, v_texCoord);
}
