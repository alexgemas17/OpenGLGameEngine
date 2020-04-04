#version 430

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;

out mat3 TBN;
out vec2 TexCoords;

uniform mat3 normalMatrix; //invese(modelMatrix)
uniform mat4 modelMatrix; //model matrix
uniform mat4 VPMatrix; //view * projection

void main() {
	// Use the normal matrix to maintain the orthogonal property of a vector when it is scaled non-uniformly
	vec3 T = normalize(mat3(transpose(inverse(modelMatrix))) * vTangent);
	vec3 B = normalize(mat3(transpose(inverse(modelMatrix))) * vBitangent);
	vec3 N = normalize(mat3(transpose(inverse(modelMatrix))) * vNormal);
	TBN = mat3(T, B, N);

	TexCoords = texCoords;

	vec3 fragPos = vec3(modelMatrix * vec4(vPosition, 1.0f));

	gl_Position = VPMatrix * vec4(fragPos, 1.0);
}