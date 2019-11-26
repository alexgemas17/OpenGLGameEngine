#version 400

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 mModel;
uniform mat4 mMVP;

void main()
{
    FragPos = vec3(mModel * vec4(vPosition, 1.0));
    Normal = mat3(transpose(inverse(mModel))) * vNormal;  
    
    gl_Position = mMVP * vec4(vPosition, 1.0);
}