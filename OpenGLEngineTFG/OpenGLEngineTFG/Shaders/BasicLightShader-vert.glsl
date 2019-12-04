#version 400

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 mModelView;
uniform mat4 mMVP;

void main()
{
    FragPos = vec3(mModelView * vec4(vPosition, 1.0));
    Normal = vec3(mModelView * vec4(vNormal, 0.0)); 
    TexCoords = vec2(vTexCoord.x, vTexCoord.y);

    gl_Position = mMVP * vec4(vPosition, 1.0);
}