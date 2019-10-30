#version 400

layout (location = 0) in vec3 vectorPosition;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
	gl_Position = vec4(vectorPosition, 1.0);
	//gl_PointSize = 5.0f;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}