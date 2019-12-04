#version 400

layout (location = 0) in vec3 vectorPosition;

uniform mat4 MatrixMVP;

out vec4 ourColor;

void main()
{
	gl_Position = MatrixMVP * vec4(vectorPosition, 1.0);
	//gl_PointSize = 10.0f;
}