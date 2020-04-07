#version 430

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;

out vec3 FragPos;
out vec3 VertexNormal;
out vec3 VertexTangent;
out vec2 TexCoords;

uniform mat4 ModelViewMatrix; //view * model
uniform mat4 MVP; //projection * view * model
//uniform mat3 NormalMatrix;

void main()
{
    FragPos = vec3( ModelViewMatrix * vec4(vPosition,1.0) );

    TexCoords = vTexCoords;

    //mat3 NormalMatrix = mat3(transpose(inverse(ModelViewMatrix)));

    VertexNormal = vec3( ModelViewMatrix * vec4(vNormal,0.0) );
    //VertexNormal = normalize( NormalMatrix * vNormal );

    VertexTangent = vec3( ModelViewMatrix * vec4(vTangent, 0.0) );
    //VertexTangent = normalize( NormalMatrix * vTangent );

    gl_Position = MVP * vec4(vPosition, 1.0);
}
