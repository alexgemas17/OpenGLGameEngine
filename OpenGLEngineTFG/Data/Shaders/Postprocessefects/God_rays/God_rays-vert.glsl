#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 Tex_Coord;

void main() {
	gl_Position = vec4(position, 1.0);
	Tex_Coord = texCoord;
}