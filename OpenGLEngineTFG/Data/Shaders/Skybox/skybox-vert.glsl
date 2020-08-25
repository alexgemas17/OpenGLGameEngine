#version 430 core

layout (location = 0) in vec3 vPosition;

out vec3 SampleDirection;

uniform mat4 ViewMatrix;
uniform mat4 ProjMatrix;

void main() {
	SampleDirection = vPosition;

	vec4 Position = ProjMatrix * mat4(mat3(ViewMatrix)) * vec4(vPosition, 1.0f);
	gl_Position = Position.xyww;
}
