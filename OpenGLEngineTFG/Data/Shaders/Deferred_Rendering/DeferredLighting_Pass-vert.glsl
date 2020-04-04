#version 400

layout (location = 0) in vec3 vPosition;
layout (location = 2) in vec2 vTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = vTexCoords;
    gl_Position = vec4(vPosition, 1.0);
}