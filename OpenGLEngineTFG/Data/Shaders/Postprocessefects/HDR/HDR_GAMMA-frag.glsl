#version 430 core

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform float exposure;
uniform float gamma;

void main() {

	vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    result = pow(result, vec3(1.0 / gamma));

    FragColor = vec4(result, 1.0);
}
