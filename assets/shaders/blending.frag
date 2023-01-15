#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_texCoords;
uniform sampler2D texture1;

void main() {
    color = texture(texture1, v_texCoords);

    // If using fully transparent textures we can simply disregard pixels with high transparency
    // Blending becomes tricky with multiple semi-transparent textures, because depth testing does not care
    // about transparency, objects must be drawn from the farthest to the nearest
    //vec4 texColor = texture(texture1, v_texCoords);
    //if(texColor.a < 0.1)
        //discard;
    //color = texColor;
}
