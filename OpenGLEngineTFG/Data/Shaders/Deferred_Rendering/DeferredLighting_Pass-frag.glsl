#version 430

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

const float Linear =  0.7f;
const float Quadratic = 1.8f;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
    vec4 Position;
    vec4 Color;
    vec4 IntensityandRadius;
};

// Shader storage buffer objects
layout(std430, binding = 0) readonly buffer LightsBuffer {
    Light data[];
} lightsBuffer;

uniform int lightCount;
uniform vec3 viewPos;

void main()
{   
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    
    // then calculate lighting as usual
    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    
    for(uint i = 0; i < lightCount; ++i)
    {
        Light light = lightsBuffer.data[i];

        vec3 lightPosition = light.Position.xyz;
        float radius = light.IntensityandRadius.y;

        vec3 lDir = lightPosition - FragPos;
        float distance = length(lDir);
        if(distance < radius)
        {
            vec3 lightColor = light.Color.rgb;
            float intensity = light.IntensityandRadius.x;

            vec3 lightDir = normalize(lDir);

            // specular shading
            vec3 reflectDir = reflect(-lightDir, Normal);

            // attenuation
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
    }

    const float gamma = 2.2;
    const float exposure = 1.5f;
    vec3 result = vec3(1.0) - exp(-lighting * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    
    FragColor = vec4(lighting, 1.0);
}