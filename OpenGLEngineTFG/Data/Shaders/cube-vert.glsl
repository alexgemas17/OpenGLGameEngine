#version 430

layout (location = 0) in vec3 vPosition;
layout (location = 2) in vec2 vTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = vTexCoords;
    gl_Position = projection * view * model * vec4(vPosition, 1.0);
}