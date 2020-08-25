#version 430

layout (location = 0) in vec3 vPosition;

uniform mat4 ModelMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

void main() {
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vPosition, 1.0);
}
