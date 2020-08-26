#version 430

layout (location = 0) out vec4 FragColor;

in vec3 FragPos; 
in vec3 VertexNormal; 
in vec2 TexCoords;
out mat3 TBN;

struct Light {
	vec4 Position;
	vec4 Color;
	vec4 IntensityandRadius;
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
uniform sampler2D texture_albedo;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;
uniform bool has_texture_normal;

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
    vec3 Diffuse = texture(texture_albedo, TexCoords).rgb;
    float Specular = texture(texture_specular, TexCoords).a;

    vec3 Normal = vec3(0.0f);
    if(has_texture_normal){
        Normal = texture(texture_normal, TexCoords).rgb;
        Normal = normalize(TBN *  normalize(Normal * 2.0 - 1.0));
    }else{
        Normal = (gl_FrontFacing) ? normalize(VertexNormal) : normalize(-VertexNormal);
    }

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

		vec3 lightPosition = light.Position.xyz;
        float radius = light.IntensityandRadius.y;

        vec3 lDir = lightPosition - FragPos;
        vec3 lightColor = light.Color.rgb;
        float intensity = light.IntensityandRadius.x;

        vec3 lightDir = normalize(lDir);

        // specular shading
        vec3 reflectDir = reflect(-lightDir, Normal);

        // attenuation
        float distance = length(lDir);
        float attenuation = 1.0 / (1.0f + Linear * distance + Quadratic * (distance * distance)); 

        // combine results
        vec3 ambient = lightColor * intensity;
        
        vec3 diffuse = vec3(0.0f);
        float dotL = dot(Normal, lightDir);
        if ( dotL > 0.0f){
            diffuse = max(dotL, 0.0) * Diffuse * lightColor * intensity;
        }

        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
        vec3 specular = lightColor * spec * Specular;

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += (ambient + diffuse + specular); 
	}

	const float gamma = 2.2;
    const float exposure = 1.5f;
    vec3 result = vec3(1.0) - exp(-lighting * exposure);
    result = pow(result, vec3(1.0 / gamma));
    
    FragColor = vec4(result, 1.0);
}