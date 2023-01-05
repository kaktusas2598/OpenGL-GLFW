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
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


layout(location = 0) out vec4 color;

in vec3 v_normal;
in vec3 v_fragPos;

uniform Material material;
uniform Light light;
// Alternatively we can calculate lighting in view space, this uniform becaumes 0,0,0 always and is not needed
// but we need to convert all relevant vectors, view matrix and normal matrix if used
uniform vec3 viewPosition;

void main() {
    vec3 ambient = light.ambient * material.ambient;

    // diffuse light
    vec3 norm = normalize(v_normal);
    vec3 lightDirection = normalize(light.position - v_fragPos);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // specular light
    vec3 viewDirection = normalize(viewPosition - v_fragPos);
    vec3 reflectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    vec3 finalColor = ambient + diffuse + specular;

    color = vec4(finalColor, 1.0);
}
