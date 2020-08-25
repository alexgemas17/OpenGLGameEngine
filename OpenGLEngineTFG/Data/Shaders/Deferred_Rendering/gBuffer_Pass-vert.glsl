#version 430

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;

out vec3 FragPos;
out vec3 VertexNormal;
out vec2 TexCoords;
out mat3 TBN;

uniform mat4 ModelMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

void main()
{
    vec4 worldPos = ModelMatrix * vec4(vPosition, 1.0);
    FragPos = worldPos.xyz; 
    TexCoords = vTexCoords;
    
    mat3 NormalMatrix = transpose(inverse(mat3(ModelMatrix)));
    VertexNormal = NormalMatrix * vNormal;

    vec3 T = normalize(NormalMatrix * vTangent);
    vec3 B = normalize(NormalMatrix * vBitangent);
    vec3 N = normalize(NormalMatrix * vNormal);
    TBN = mat3(T, B, N);

    gl_Position = ProjectionMatrix * ViewMatrix * worldPos;
}