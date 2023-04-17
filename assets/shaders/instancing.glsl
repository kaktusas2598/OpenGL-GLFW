#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 offset;


// Alternative and simpler solution to do instanced drawing is to pass translations
// uniform vec3 offsets[100];
// as uniform array and use gl_InstanceID built:
// gl_Position = inPosition + offsets[gl_InstanceID]
// However while easier, this approach is limiting, because GLSL
// has a limit on how many uniform data we can pass to the shaders, so
// the preferred solution is to use instanced arrays in vertex attributes

out vec2 v_texCoord;

uniform mat4 u_MVP;

void main() {
    //vec3 pos = position * (gl_InstanceID / 100.0);
    //gl_Position = u_MVP * vec4(pos + offset, 1.0);
    gl_Position = u_MVP * vec4(position + offset, 1.0);
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
