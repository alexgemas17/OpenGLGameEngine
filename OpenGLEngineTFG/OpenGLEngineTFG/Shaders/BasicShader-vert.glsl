#version 400

layout (location = 0) in vec3 vectorPosition;

out vec4 ourColor;

void main()
{
	gl_Position = vec4(vectorPosition, 1.0);
	gl_PointSize = 10.0f;
}