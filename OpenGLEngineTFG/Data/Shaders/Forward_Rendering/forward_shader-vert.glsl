#version 330 compatibility

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 ModelViewMatrix;
uniform mat4 MVP;

void main()
{
    FragPos = vec3( ModelViewMatrix * vec4(vPosition, 1.0));
    Normal = vec3( ModelViewMatrix * vec4(vNormal, 0.0)); 
    TexCoords = vec2( vTexCoord.x, vTexCoord.y);

    gl_Position = MVP * vec4(vPosition, 1.0);
}