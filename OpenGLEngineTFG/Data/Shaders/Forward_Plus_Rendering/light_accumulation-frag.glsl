#version 430

layout (location = 0) out vec4 FragColor;

in vec3 FragPos; 
in vec3 Normal; 
in vec2 TexCoords;

struct Light {
    vec3 Position;
    vec3 Color;
    float Intensity;
    
    float Linear;
    float Quadratic;
    float Radius;
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
uniform int numberOfTilesX;
uniform vec3 viewPos;

void main() {
	// ----------------------------------------------------
	// Determine which tile this pixel belongs to
	ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileID = location / ivec2(16, 16);
	uint index = tileID.y * numberOfTilesX + tileID.x;
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
	uint offset = index * 1024;
	for (uint i = 0; i < 1024 && visibleLightIndicesBuffer.data[offset + i].index != -1; i++) {
		uint lightIndex = visibleLightIndicesBuffer.data[offset + i].index;
		Light light = lightBuffer.data[lightIndex];
	// ----------------------------------------------------

		vec3 lightDir = normalize(light.Position - FragPos);
        // diffuse shading
        //float diff = max(dot(normal, lightDir), 0.0);

        // specular shading
        vec3 reflectDir = reflect(-lightDir, Normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);

        // attenuation
        float distance = length(light.Position - FragPos);
        //float attenuation = 1.0 / (1.0 + light.Linear * distance + light.Quadratic * distance * distance); 
        float attenuation = 1.0 / (1.0f + light.Linear * distance + light.Quadratic * (distance * distance)); 

        // combine results
        vec3 ambient = Diffuse;
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.Color;
        vec3 specular = light.Color * spec;

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += (ambient + diffuse + specular);
	}

	FragColor = vec4(lighting, 1.0);
}