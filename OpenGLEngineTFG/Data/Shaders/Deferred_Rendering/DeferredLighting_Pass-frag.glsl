#version 400

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

// Data info from Framebuffers
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

struct Light {
    vec3 Position;
    //vec3 Color;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float Constant;
    float Linear;
    float Quadratic;
};
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedo, TexCoords).rgb; 

    // then calculate lighting as usual
    vec3 lighting = vec3(0.0);
    vec3 viewDir  = normalize(viewPos - FragPos);

    for(int i = 0; i < NR_LIGHTS; ++i)
    {

        vec3 lightDir = normalize(lights[i].Position - FragPos);

        // diffuse shading
        float diff = max(dot(normal, lightDir), 0.0);

        // specular shading
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

        // attenuation
        float distance = length(lights[i].Position - FragPos);
        float attenuation = 1.0 / (lights[i].Constant + lights[i].Linear * distance + lights[i].Quadratic * (distance * distance));    

        // combine results
        vec3 ambient = lights[i].ambient * Diffuse;
        vec3 diffuse = lights[i].diffuse * diff * Diffuse;
        //vec3 specular = lights[i].specular * spec * vec3(texture(material.specular, TexCoords));
        vec3 specular = lights[i].specular * spec;

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;

        lighting = (ambient + diffuse + specular); 
    }

    FragColor = vec4(lighting, 1.0);
}