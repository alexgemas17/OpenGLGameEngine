#version 430

layout (location = 0) in vec3 vPosition;

uniform mat4 MVP; //ProjectionMatrix * ViewMatrix * ModelMatrix

void main() {
	gl_Position = MVP * vec4(vPosition, 1.0);
}
