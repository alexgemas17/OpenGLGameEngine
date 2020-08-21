#version 430 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;
//layout (location = 3) in vec3 vTangent;
//layout (location = 4) in vec3 vBitangent;

out vec3 FragPos;
out vec3 VertexNormal;
out vec2 TexCoords;
//out mat3 TBN;

//uniform mat4 ModelViewMatrix; //view * model
//uniform mat3 NormalMatrix;
//uniform mat4 MVP; //projection * view * model

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    //FragPos = vec3( ModelViewMatrix * vec4(vPosition,1.0) );

    //mat3 NormalMatrix = mat3(transpose(inverse(ModelViewMatrix)));

    //VertexNormal = vec3( NormalMatrix * vNormal );

    //TexCoords = vTexCoords;

    // Use the normal matrix to maintain the orthogonal property of a vector when it is scaled non-uniformly
	//vec3 T = normalize(NormalMatrix * vTangent);
	//vec3 B = normalize(NormalMatrix * vBitangent);
	//vec3 N = normalize(NormalMatrix * vNormal);
	//TBN = mat3(T, B, N);

    vec4 worldPos = model * vec4(vPosition, 1.0);
    FragPos = worldPos.xyz; 
    TexCoords = vTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    VertexNormal = normalMatrix * vNormal;

    gl_Position = projection * view * worldPos;

    //gl_Position = MVP * vec4(vPosition, 1.0);
}