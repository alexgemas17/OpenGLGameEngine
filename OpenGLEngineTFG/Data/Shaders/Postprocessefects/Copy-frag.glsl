#version 430

in vec2 TexCoords;

out vec4 FragColour;

uniform sampler2D input_texture;

void main() {
	FragColour = texture(input_texture, TexCoords);
}
