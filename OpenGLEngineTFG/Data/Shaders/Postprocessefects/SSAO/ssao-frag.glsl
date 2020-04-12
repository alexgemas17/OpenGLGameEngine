#version 430 core

out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;    
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform float ssaoStrength;
uniform float sampleRadius;
uniform float sampleRadius2;
uniform int numKernelSamples;
uniform vec3 samples[64];

// tile noise texture over screen based on screen dimensions divided by noise size
uniform vec2 noiseScale;

uniform mat4 mViewProj; //Projection * View
uniform mat4 projection;

vec3 WorldPosFromDepth(vec2 textureCoordinates);

void main() {
	// Early out if there is no data in the GBuffer at this particular sample
	vec3 normal = texture(gNormal, TexCoords).xyz;
	if (normal == vec3(0.0, 0.0, 0.0)) {
		FragColor = 1.0;
		return;
	}

	vec3 fragPos = texture(gPosition, TexCoords).xyz;
	vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;

	// Make a TBN matrix to go from tangent -> world space (so we can put our hemipshere tangent sample points into world space)
	// Since our normal is already in world space, this TBN matrix will take our tangent space vector and put it into world space
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	// Calculate the total amount of occlusion for this fragment
	float occlusion = 0.0f;
	for (int i = 0; i < numKernelSamples; ++i) {
		vec3 sampleWorld = (TBN * samples[i]) * sampleRadius;
		sampleWorld = fragPos + sampleWorld;

		// Take our sample position in world space and convert it to screen coordinates
		vec4 sampleScreenSpace = vec4(sampleWorld, 1.0);
		sampleScreenSpace = mViewProj * sampleScreenSpace; // World -> View -> Clip Space
		sampleScreenSpace.xyz /= sampleScreenSpace.w; // Perspective division (Clip Space -> NDC)
		sampleScreenSpace.xyz = (sampleScreenSpace.xyz * 0.5) + 0.5; // [-1, 1] -> [0, 1]

		// Check if our current samples depth is behind the screen space geometry's depth, if so then we know it is occluded in screenspace
		//float sceneDepth = texture(depthTexture, sampleScreenSpace.xy).r;
		float sceneDepth = texture(gPosition, sampleScreenSpace.xy).z;

		// Peform a range check on the current fragment we are calculating the occlusion factor for, and the occlusion position
		//vec3 occlusionPos = WorldPosFromDepth(sampleScreenSpace.xy);
		vec3 occlusionPos = texture(gPosition, sampleScreenSpace.xy).rgb;
		vec3 fragToOcclusionPos = fragPos - occlusionPos;
		if (dot(fragToOcclusionPos, fragToOcclusionPos) <= sampleRadius2) {
			occlusion += ((sampleScreenSpace.z > sceneDepth) ? 1.0 : 0.0);
		}
	}
	// Finally we need to normalize our occlusion factor
	occlusion = 1.0 - (occlusion / numKernelSamples);

	FragColor = pow(occlusion, ssaoStrength);
}