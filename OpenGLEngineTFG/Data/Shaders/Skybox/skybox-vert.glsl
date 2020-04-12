#version 430 core

layout (location = 0) in vec3 vPosition;

out vec3 SampleDirection;

//uniform mat4 ViewMatrix;
//uniform mat4 ProjMatrix;

// Cut off the translation part of the view matrix (so the skybox will not move when the camera is moved)
uniform mat4 ViewProjMatrix; //ProjMatrix * mat4(mat3(ViewMatrix))

void main() {
	SampleDirection = vPosition; // A skymap can be sampled by its vertex positions (since it is centered around the origin)

	//vec4 pos = ProjMatrix * mat4(mat3(ViewMatrix)) * vec4(vPosition, 1.0f);
	vec4 Position = ViewProjMatrix * vec4(vPosition, 1.0f);
	gl_Position = Position.xyww; // Put w in depth position so when perspective division occurs w/w = 1 (which is the greatest depth value) so it is rendered behind everything
}
