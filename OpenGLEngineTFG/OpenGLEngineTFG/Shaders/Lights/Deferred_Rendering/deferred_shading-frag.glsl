#version 400

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gNormalSpec;

//BUMP MAPPING
uniform sampler2D gBinormal;
uniform sampler2D gTangent;

struct Light {
    vec3 Position;
    float Shininess;
    float Radius;
	vec3 Ks;

	vec3 Ia;
    vec3 Id;
    vec3 Is;

    //vec3 Color;
    //float Linear;
    //float Quadratic;
    //float Radius;
};

uniform Light light;

vec3 AmbientDiffSpecCalculation(vec3 FragPos, Light light, vec3 texColor, float Specular, vec3 normal)
{
    vec3 Kad = texColor.rgb;
    vec3 n;

    if (gl_FrontFacing) {
        n = normalize(normal);
    } else {
        n = normalize(-normal);
    }

    vec3 l = normalize( light.Position - FragPos );
    vec3 v = normalize( -FragPos );
    vec3 r = reflect( -l, n );

    vec3 ambient = (light.Ia * Kad);
    vec3 diffuse = (light.Id * Specular * Kad * max( dot(l, n), 0.0));
    vec3 spec = (light.Is * light.Ks * pow( max( dot(r, v), 0.0), light.Shininess ));

    // attenuation
    //float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
    
    //diffuse *= attenuation;
    //specular *= attenuation;
    //ambient *=  attenuation;

    return (ambient + diffuse + spec);
}

vec3 AmbientDiffSpecCalculationBumpMapping(vec3 FragPos, Light light, vec3 texColor, float Specular, vec4 normal, vec3 lDir, vec3 vDir)
{
    vec3 Kad = texColor.rgb;
    vec3 n = normal.rgb;

    if (gl_FrontFacing) {
        n = normalize(n);
    } else {
        n = normalize(-n);
    }

    vec3 l = normalize( lDir );
    vec3 v = normalize( vDir );
    vec3 r = reflect( -l, n );

    vec3 ambient = (light.Ia * Kad);
    vec3 diffuse = (light.Id * Specular * Kad * max( dot(l, n), 0.0));
    vec3 spec = (light.Is * light.Ks * pow( max( dot(r, v), 0.0), light.Shininess ));

    // attenuation
    //float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
    
    //diffuse *= attenuation;
    //specular *= attenuation;
    //ambient *=  attenuation;

    return (ambient + diffuse + spec);
}

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;

    //float distance = length(light.Position - FragPos);
    //if(distance < light.Radius)
    //{
    	//TO-DO
    //}

    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    vec3 Binormal = texture(gBinormal, TexCoords).rgb;
    vec3 Tangent = texture(gTangent, TexCoords).rgb;	

    // calculate the bump mapping data
    mat3 TBN = transpose(mat3(Tangent, Binormal, Normal));
    vec3 lDir = normalize(TBN * ( light.Position - FragPos ));
    vec3 vDir = normalize(TBN * ( -FragPos ));

    // get the bump mapping normal
    vec4 normalBumpMapping = (2.0 * texture(gNormalSpec, TexCoords)) - 1.0;

    // light calculation
    FragColor = vec4(AmbientDiffSpecCalculationBumpMapping(FragPos, light, Diffuse, Specular, normalBumpMapping, lDir, vDir), 1.0);
    //FragColor = vec4(AmbientDiffSpecCalculation(FragPos, light, Diffuse, Specular, Normal), 1.0);
}