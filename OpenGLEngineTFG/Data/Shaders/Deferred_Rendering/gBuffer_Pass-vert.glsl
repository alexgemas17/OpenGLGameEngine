// ------------------ DEFERRED RENDERING ------------------
#version 430 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;

out VS_OUT {
    out vec3 FragPos;
    out vec3 VertexNormal;
    out vec2 TexCoords;
    out mat3 TBN;
} vertex_out;  

uniform mat4 ModelMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

void main()
{
    vec4 worldPos = ModelMatrix * vec4(vPosition, 1.0);

    mat3 NormalMatrix = transpose(inverse(mat3(ModelMatrix)));
    vec3 T = normalize(NormalMatrix * vTangent);
    vec3 N = normalize(NormalMatrix * vNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    vertex_out.FragPos = worldPos.xyz; 
    vertex_out.VertexNormal = NormalMatrix * vNormal;
    vertex_out.TexCoords = vTexCoords;
    vertex_out.TBN = mat3(T, B, N);

    gl_Position = ProjectionMatrix * ViewMatrix * worldPos;
}