#version 400

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

//BUMP MAPPING
out vec3 Tvector;
out vec3 Nvector;
out vec3 Bvector;

uniform mat4 mModel;
uniform mat4 mMVP;

void main()
{

	vec4 worldPos = mModel * vec4(vPosition, 1.0);
    FragPos = worldPos.xyz; 
    
    TexCoords = vTexCoord;

    mat3 normalMatrix = transpose(inverse(mat3(mModel)));
    vec3 T = normalize(normalMatrix * vTangent);
    vec3 N = normalize(normalMatrix * vNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    Normal = normalMatrix * vNormal;
    Tvector = T;
    Nvector = N;
    Bvector = B;

    gl_Position = mMVP * vec4(vPosition, 1.0);
}