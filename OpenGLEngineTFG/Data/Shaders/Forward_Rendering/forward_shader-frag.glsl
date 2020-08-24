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

uniform sampler2D texture_albedo;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;
uniform bool has_texture_normal;

//const int NR_LIGHTS = 250;
//uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

// Shader storage buffer objects
layout(std430, binding = 0) readonly buffer LightsBuffer {
    Light data[];
} lightsBuffer;
uniform int lightCount;

const float Linear =  0.7f;
const float Quadratic = 1.8f;

void main()
{
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
    
    for(uint i = 0; i < lightCount; ++i)
    {
        Light light = lightsBuffer.data[i];

        vec3 lightPosition = light.Position.xyz;
        float radius = light.IntensityandRadius.y;

        float distance = length(lightPosition - FragPos);
        if(distance < radius)
        {
            vec3 lightColor = light.Color.rgb;
            float intensity = light.IntensityandRadius.x;

            vec3 lightDir = normalize(lightPosition - FragPos);

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

/*for(int i = 0; i < NR_LIGHTS; ++i)
    {
        float distance = length(lights[i].Position - FragPos);
        if(distance < lights[i].Radius)
        {
            vec3 lightDir = normalize(lights[i].Position - FragPos);

            // specular shading
            vec3 reflectDir = reflect(-lightDir, Normal);

            // attenuation
            float distance = length(lights[i].Position - FragPos);
            float attenuation = 1.0 / (1.0f + Linear * distance + Quadratic * (distance * distance)); 

            // combine results
            vec3 ambient = lights[i].Color * lights[i].Intensity;
            
            vec3 diffuse = vec3(0.0f);
            float dotL = dot(Normal, lightDir);
            if ( dotL > 0.0f){
                diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color * lights[i].Intensity;
            }

            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
            vec3 specular = lights[i].Color * spec * Specular;

            ambient *= attenuation;
            diffuse *= attenuation;
            specular *= attenuation;
            lighting += (ambient + diffuse + specular);


        }        
    }*/