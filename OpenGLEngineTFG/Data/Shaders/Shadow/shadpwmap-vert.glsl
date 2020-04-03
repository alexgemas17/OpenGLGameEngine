#version 430

layout (location = 0) in vec3 vertexPosition;

uniform mat4 lightSpaceViewProjectionMatrix;
uniform mat4 ModelMatrix;

void main() {
	gl_Position = lightSpaceViewProjectionMatrix * ModelMatrix * vec4(vertexPosition, 1.0f);
}
