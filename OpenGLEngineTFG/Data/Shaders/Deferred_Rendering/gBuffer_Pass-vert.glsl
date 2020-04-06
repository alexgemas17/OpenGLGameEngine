#version 400

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;

out vec3 FragPos;
out vec3 VertexNormal;
out vec3 VertexTangent;
out vec2 TexCoords;

uniform mat4 ModelMatrix;
uniform mat4 ViewProjMatrix; //projection * view

void main()
{
    vec4 worldPos = ModelMatrix * vec4(vPosition, 1.0);
    FragPos = worldPos.xyz; 
    
    VertexNormal = (ModelMatrix * vec4(vNormal, 0.0)).xyz;

    VertexTangent = (ModelMatrix * vec4(vTangent, 0.0)).xyz;

    TexCoords = vTexCoords;

    gl_Position = ViewProjMatrix * worldPos;
}
