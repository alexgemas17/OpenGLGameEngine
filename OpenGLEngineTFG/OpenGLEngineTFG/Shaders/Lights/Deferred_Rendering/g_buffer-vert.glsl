#version 400

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 mModel;
//uniform mat4 mModelView;
uniform mat4 mMVP;

void main()
{

	vec4 worldPos = mModel * vec4(vPosition, 1.0);
    FragPos = worldPos.xyz; 
    
    TexCoords = vTexCoord;

    mat3 normalMatrix = transpose(inverse(mat3(mModel)));
    Normal = normalMatrix * vNormal;

    gl_Position = mMVP * vec4(vPosition, 1.0);
}