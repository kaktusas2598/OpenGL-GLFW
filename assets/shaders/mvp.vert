#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

out vec3 v_normal;
out vec3 v_fragPos;
out vec2 v_texCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    //gl_Position = projection * view * model * vec4(position, 1.0);
    // Calculate fragment position for lighting in world space
    v_fragPos = vec3(model * vec4(position, 1.0));
    gl_Position = projection * view * vec4(v_fragPos, 1.0);
    v_normal = normal;
    v_texCoords = texCoords;
    // Example of normal matrix calculation ( in world space, htat's why its derived from model matrix)
    // inverse is a very costly operation for shaders though!
    // But this must be used if we apply non uniform scale
    //v_normal = mat3(transpose(inverse(model))) * normal;
}

