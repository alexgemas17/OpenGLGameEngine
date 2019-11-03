#version 400

layout (location = 0) out vec4 FragColor;

in vec4 ourColor;

// texture sampler
uniform sampler2D texture1;

void main()
{
	FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}