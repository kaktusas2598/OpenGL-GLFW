#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 v_normal;
out vec3 v_fragPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    // Calculate fragment position for lighting in world space
    v_fragPos = vec3(model * vec4(position, 1.0));
    v_normal = normal;
    // Example of normal matrix calculation ( in world space, htat's why its derived from model matrix)
    // inverse is a very costly operation for shaders though!
    // But this must be used if we apply non uniform scale
    //v_normal = mat3(transpose(inverse(model))) * normal
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_normal;
in vec3 v_fragPos;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPosition;

void main() {
    float ambientStrenght = 0.1;
    vec3 ambient = ambientStrenght * lightColor;

    vec3 norm = normalize(v_normal);
    vec3 lightDirection = normalize(lightPosition - v_fragPos);

    // Calculate diffuce impact
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 finalColor = (ambient + diffuse) * objectColor;

    color = vec4(finalColor, 1.0);
}
