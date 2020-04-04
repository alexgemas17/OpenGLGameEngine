#version 430

layout (location = 0) out vec4 gb_Albedo;
layout (location = 1) out vec3 gb_Normal;
layout (location = 2) out vec4 gb_MaterialInfo;

uniform sampler2D texture_albedo;
uniform sampler2D texture_normal;
uniform sampler2D texture_metallic;
uniform sampler2D texture_roughness;
uniform sampler2D texture_ao;

in mat3 TBN;
in vec2 TexCoords;

void main() {
	vec2 textureCoordinates = TexCoords;

	// Sample textures
	vec4 albedo = texture(texture_albedo, textureCoordinates);
	vec3 normal = texture(texture_normal, textureCoordinates).rgb;
	float metallic = texture(texture_metallic, textureCoordinates).r;
	//float roughness = max(texture(texture_roughness, textureCoordinates).r, 0.04);
	float roughness = texture(texture_roughness, textureCoordinates).r;
	float ao = texture(texture_ao, textureCoordinates).r;

	// Normal mapping code. Opted out of tangent space normal mapping since I would have to convert all of my lights to tangent space
	normal = normalize(TBN * normalize(normal * 2.0 - 1.0));

	gb_Albedo = albedo;
	gb_Normal = normal;
	gb_MaterialInfo = vec4(metallic, roughness, ao, 1.0);
}