#version 400

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;
layout (location = 3) in vec2 vTangent;
layout (location = 4) in vec2 vBitangent;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 ModelMatrix;
uniform mat4 ViewProjMatrix; //projection * view

void main()
{
    vec4 worldPos = ModelMatrix * vec4(vPosition, 1.0);
    FragPos = worldPos.xyz; 

    TexCoords = vTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(ModelMatrix)));
    Normal = normalMatrix * vNormal;

    gl_Position = ViewProjMatrix * worldPos;
}