#version 400

layout (location = 0) in vec3 aPos;

uniform mat4 mModelViewProj;

void main() {
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}