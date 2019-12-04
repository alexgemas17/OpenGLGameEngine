#version 400

layout (location = 0) out vec4 FragColor;

in vec4 ourColor;

void main()
{
	FragColor = vec4(0.8, 0.8, 0.8, 0.8);
}