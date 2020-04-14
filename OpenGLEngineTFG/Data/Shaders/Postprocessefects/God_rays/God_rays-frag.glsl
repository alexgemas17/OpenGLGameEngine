#version 430

in vec2 Tex_Coord;

out vec4 FragColour;

uniform vec3 ScreenLightPos;
uniform sampler2D input_texture;

const int NUM_SAMPLES = 100;
const float Weight = 0.58767f;
const float Decay = 0.96815f;
const float Exposure = 0.2f;
const float Density = 0.926;

void main() {
	vec2 TexCoords = Tex_Coord;

	// Calculate vector from pixel to light source in screen space.    
	vec2 deltaTexCoord = (TexCoords - ScreenLightPos.xy);   

	// Divide by number of samples and scale by control factor.   
	deltaTexCoord *= 1.0f / NUM_SAMPLES * Density;   

	// Store initial sample.    
	vec3 color = texture(input_texture, TexCoords).rgb;   

	// Set up illumination decay factor.   
	float illuminationDecay = 1.0f;   

	// Evaluate summation from Equation 3 NUM_SAMPLES iterations.    
	for (int i = 0; i < NUM_SAMPLES; i++)   {     
		// Step sample location along ray.     
		TexCoords -= deltaTexCoord;     

		// Retrieve sample at new location.    
		vec3 sampleTex = texture(input_texture, TexCoords).rgb;

		// Apply sample attenuation scale/decay factors.     
		sampleTex *= illuminationDecay * Weight;     

		// Accumulate combined color.     
		color += sampleTex;     

		// Update exponential decay factor.     
		illuminationDecay *= Decay;   
	}   

	// Output final color with a further scale control factor.    
	FragColour = vec4( color * Exposure, 1.0f);
}
