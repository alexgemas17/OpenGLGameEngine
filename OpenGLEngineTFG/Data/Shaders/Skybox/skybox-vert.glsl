#version 430 core

layout (location = 0) in vec3 vPosition;

out vec3 SampleDirection;

uniform mat4 ViewMatrix;
uniform mat4 ProjMatrix;

void main() {
	SampleDirection = vPosition; // A skymap can be sampled by its vertex positions (since it is centered around the origin)

	// Cut off the translation part of the view matrix (so the skybox will not move when the camera is moved)
	vec4 pos = ProjMatrix * mat4(mat3(ViewMatrix)) * vec4(vPosition, 1.0f);
	gl_Position = pos.xyww; // Put w in depth position so when perspective division occurs w/w = 1 (which is the greatest depth value) so it is rendered behind everything
}
