#version 400

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gNormalSpec;

uniform sampler2D gTvector;
uniform sampler2D gNvector;
uniform sampler2D gBvector;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
    float Radius;
};

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    vec3 normal = texture(gNormalSpec, TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

    vec3 T = texture(gTvector, TexCoords).rgb;
    vec3 N = texture(gNvector, TexCoords).rgb;
    vec3 B = texture(gBvector, TexCoords).rgb;

    mat3 TBN = transpose(mat3(T, B, N));
    vec3 TangentViewPos = TBN * viewPos;
    vec3 TangentFragPos = TBN * FragPos;
    
    // then calculate lighting as usual
    vec3 ambient  = Diffuse * 0.1; // hard-coded ambient component
    //vec3 viewDir  = normalize(viewPos - FragPos);
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec3 diffuse;
    vec3 specular;
    for(int i = 0; i < NR_LIGHTS; ++i)
    {
        // calculate distance between light source and current fragment
        float distance = length(lights[i].Position - FragPos);
        if(distance < lights[i].Radius)
        {
            vec3 TangentLightPos = TBN * lights[i].Position;

            // diffuse
            //vec3 lightDir = normalize(lights[i].Position - FragPos);
            vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
            diffuse = max(dot(normal, lightDir), 0.0) * Diffuse * lights[i].Color;

            // specular
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
            specular = lights[i].Color * spec * Specular;

            // attenuation
            float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
            
            diffuse *= attenuation;
            specular *= attenuation;
            ambient *= diffuse + specular;
        }
    } 

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}