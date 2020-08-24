#version 430

layout (location = 0) out vec4 FragColor;

in vec3 FragPos; 
in vec3 Normal; 
in vec2 TexCoords;

struct Light {
	vec4 Position;
	vec4 Color;
	vec4 Radius;
};

struct VisibleIndex {
	int index;
};

// -------------------------------------- LO IMPORTANTE --------------------------------------
// Shader storage buffer objects
layout(std430, binding = 0) readonly buffer LightBuffer {
	Light data[];
} lightBuffer;

layout(std430, binding = 1) readonly buffer VisibleLightIndicesBuffer {
	VisibleIndex data[];
} visibleLightIndicesBuffer;
// -------------------------------------------------------------------------------------------

// Uniforms
uniform sampler2D diffTexture;
uniform float numberOfTilesX;
uniform vec3 viewPos;
uniform int lightCount;

const float Linear =  0.7f;
const float Quadratic = 1.8f;

void main() {
	// ----------------------------------------------------
	// Determine which tile this pixel belongs to
	ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileID = location / ivec2(16, 16);
	int numTilesX = int(numberOfTilesX);
	uint index = tileID.y * numTilesX + tileID.x;
	// ----------------------------------------------------

	// retrieve data from gbuffer
    vec3 Diffuse = texture(diffTexture, TexCoords).rgb;
    //float Specular = texture(gAlbedoSpec, TexCoords).a;

    // then calculate lighting as usual
    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);

	// The offset is this tile's position in the global array of valid light indices.
	// Loop through all these indices until we hit max number of lights or the end (indicated by an index of -1)
	// Calculate the lighting contribution from each visible point light
	// ----------------------------------------------------
	//uint offset = index * 1024; lightCount
	uint offset = index * lightCount;
	for (uint i = 0; i < lightCount && visibleLightIndicesBuffer.data[offset + i].index != -1; i++) {
		uint lightIndex = visibleLightIndicesBuffer.data[offset + i].index;
		Light light = lightBuffer.data[lightIndex];
	// ----------------------------------------------------

		vec3 lightPosition = light.Position.xyz;
		vec3 lightDir = normalize(lightPosition - FragPos);
        // diffuse shading
        //float diff = max(dot(normal, lightDir), 0.0);

        // specular shading
        vec3 reflectDir = reflect(-lightDir, Normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);

        // attenuation
        float distance = length(lightPosition - FragPos);
        //float attenuation = 1.0 / (1.0 + light.Linear * distance + light.Quadratic * distance * distance); 
        float attenuation = 1.0 / (1.0f + Linear * distance + Quadratic * (distance * distance)); 

        // combine results
        vec3 ambient = Diffuse;
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.Color.rgb;
        vec3 specular = light.Color.rgb * spec;

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += (ambient + diffuse + specular);
	}

	FragColor = vec4(lighting, 1.0);
}