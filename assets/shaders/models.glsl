#shader vertex
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
    gl_Position = projection * view * model * vec4(position, 1.0);
    // Calculate fragment position for lighting in world space
    v_fragPos = vec3(model * vec4(position, 1.0));
    v_normal = normal;
    v_texCoords = texCoords;
    // Example of normal matrix calculation ( in world space, htat's why its derived from model matrix)
    // inverse is a very costly operation for shaders though!
    // But this must be used if we apply non uniform scale
    //v_normal = mat3(transpose(inverse(model))) * normal
}

#shader fragment
#version 330 core
struct Material {
    //vec3 ambient; // same as diffuse but weaker
    sampler2D diffuseMap;
    sampler2D specularMap;
    float shininess;

    // For models
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
};

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // attenuation
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


layout(location = 0) out vec4 color;

in vec3 v_normal;
in vec3 v_fragPos;
in vec2 v_texCoords;

uniform Material material;
uniform DirectionalLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;
// Alternatively we can calculate lighting in view space, this uniform becaumes 0,0,0 always and is not needed
// but we need to convert all relevant vectors, view matrix and normal matrix if used
uniform vec3 viewPosition;

// These methods are absolutely not optimal, contains a lot of duplication
vec3 calculateDirLight(DirectionalLight light, vec3 normal, vec3 viewDirection);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection);
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDirection);

void main() {
    vec3 outputColor = vec3(0.0);
    vec3 norm = normalize(v_normal);
    vec3 viewDirection = normalize(viewPosition - v_fragPos);

    outputColor += calculateDirLight(dirLight, norm, viewDirection);
    // TODO: add more point lightss
    outputColor += calculatePointLight(pointLight, norm, v_fragPos, viewDirection);
    outputColor += calculateSpotLight(spotLight, norm, v_fragPos, viewDirection);

    color = vec4(outputColor, 1.0);
}

vec3 calculateDirLight(DirectionalLight light, vec3 normal, vec3 viewDirection) {
    vec3 lightDirection = normalize(-light.direction);
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuseMap, v_texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuseMap, v_texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specularMap, v_texCoords));

    return (ambient + specular + diffuse);
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection) {
    vec3 lightDirection = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * vec3(texture(material.diffuseMap, v_texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuseMap, v_texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specularMap, v_texCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + specular + diffuse);
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDirection) {
    vec3 lightDirection = normalize(light.position - fragPos);
    float theta = dot(lightDirection, normalize(-light.direction));
    //float epsilon = ligh.cutOff - light.outerCutOff;
    //float intensity = clamp(theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    //diffuse *= intensity;
    // Determine if we're inside of spotlight
    if (theta > light.cutOff) {
        // Do lighting calculations, pretty much same as dir light here, point light would be better
        // with attenuation
        float diff = max(dot(normal, lightDirection), 0.0);
        vec3 reflectDirection = reflect(-lightDirection, normal);
        float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);

        vec3 ambient = light.ambient * vec3(texture(material.diffuseMap, v_texCoords));
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuseMap, v_texCoords));
        vec3 specular = light.specular * spec * vec3(texture(material.specularMap, v_texCoords));

        return (ambient + specular + diffuse);
    } else {
        return vec3(0.0);
        // Just return ambient light if outside spotlight to keep scene lit
        //return light.ambient * vec3(texture(material.diffuseMap, v_texCoords));
    }
}
