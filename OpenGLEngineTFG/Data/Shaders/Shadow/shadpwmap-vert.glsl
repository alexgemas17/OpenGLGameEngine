#version 430

layout (location = 0) in vec3 vPosition;

uniform mat4 lightSpaceMatrix;
uniform mat4 ModelMatrix;

void main() {
	gl_Position = lightSpaceMatrix * ModelMatrix * vec4(vPosition, 1.0f);
}
