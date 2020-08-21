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

uniform sampler2D diffTexture;

const int NR_LIGHTS = 150;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main()
{
    // retrieve data from gbuffer
    vec3 Diffuse = texture(diffTexture, TexCoords).rgb;
    //float Specular = texture(gAlbedoSpec, TexCoords).a;

    // then calculate lighting as usual
    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    for(int i = 0; i < NR_LIGHTS; ++i)
    {
        // calculate distance between light source and current fragment
        float distance = length(lights[i].Position - FragPos);
        if(distance < lights[i].Radius)
        {
            /*// diffuse
            vec3 lightDir = normalize(lights[i].Position - FragPos);
            vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;

            // specular
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
            //vec3 specular = lights[i].Color * spec * Specular;
            vec3 specular = lights[i].Color * spec;

            // attenuation
            float distance = length(lights[i].Position - FragPos);
            float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);

            diffuse *= attenuation;
            specular *= attenuation;
            lighting += diffuse + specular;
            */
            vec3 lightDir = normalize(lights[i].Position - FragPos);

            // diffuse shading
            //float diff = max(dot(normal, lightDir), 0.0);

            // specular shading
            vec3 reflectDir = reflect(-lightDir, Normal);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);

            // attenuation
            float distance = length(lights[i].Position - FragPos);
            //float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance); 
            float attenuation = 1.0 / (1.0f + lights[i].Linear * distance + lights[i].Quadratic * (distance * distance)); 

            // combine results
            vec3 ambient = Diffuse;
            vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
            vec3 specular = lights[i].Color * spec;

            ambient *= attenuation;
            diffuse *= attenuation;
            specular *= attenuation;
            lighting += (ambient + diffuse + specular);
        }        
    }

    FragColor = vec4(lighting, 1.0);
}

/*

void main()
{
    vec3 normal = normalize(Normal);

    //Ambient Light
    vec4 ambientColor = vec4(aLight.color,1.0)*aLight.intensity;

    //Directional Light
    float dLightDiffuseFactor = dot(normal, -dLight.direction);
    vec4 dLightDiffuseColor;
    if (dLightDiffuseFactor > 0.0) dLightDiffuseColor = vec4(dLight.color,1.0)*dLight.intensity*dLightDiffuseFactor;
    else dLightDiffuseColor = vec4(0,0,0,0);

    //Point Lights
    vec4 pTotalLightDiffuseColor = vec4(0.0,0.0,0.0,0.0);

    for (int i = 0; i < nLights ; i++) {
        float lRadius = texelFetch(lightsTexBuffer,i*4+1).y;
        vec3 lPosition = texelFetch(lightsTexBuffer, i*4+2).xyz;

        vec3 pDirection = position0 - lPosition;
        float pDistance = length(pDirection);

        if (pDistance <= lRadius) {
            float lIntensity = texelFetch(lightsTexBuffer,i*4+1).x;
            vec3 lColor = texelFetch(lightsTexBuffer,i*4).rgb;
            vec3 lAttenuation = texelFetch(lightsTexBuffer, i*4+3).xyz;

            pDirection = normalize(pDirection);
            float pLightDiffuseFactor = dot(normal, -pDirection);

            vec4 pLightDiffuseColor;
            if (pLightDiffuseFactor > 0.0) pLightDiffuseColor = vec4(lColor,1.0)*lIntensity*pLightDiffuseFactor;
            else pLightDiffuseColor = vec4(0.0,0.0,0.0,0.0);
        
            float pAttenuation = lAttenuation.x + lAttenuation.y*pDistance + lAttenuation.z*pDistance*pDistance;
            pTotalLightDiffuseColor += pLightDiffuseColor/pAttenuation;
        }
    }
    
    vec3 Diffuse = texture(diffTexture, TexCoords).rgb;
    FragColor =  Diffuse * (ambientColor + dLightDiffuseColor + pTotalLightDiffuseColor);
}


*/
