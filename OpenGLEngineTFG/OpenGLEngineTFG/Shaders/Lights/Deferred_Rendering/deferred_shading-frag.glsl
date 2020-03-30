#version 400

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
//uniform sampler2D gNormalSpec;

//BUMP MAPPING
//uniform sampler2D gBinormal;
//uniform sampler2D gTangent;

//vec3 Color;
//float Linear;
//float Quadratic;
//float Radius;
uniform vec3 LightPosition; // Light position in eye coords.
vec3 LightIntensity = vec3(0.2, 02, 0.2); // A,D,S intensity

struct MaterialInfo {
    vec3 Ka; // Ambient reflectivity
    vec3 Kd; // Diffuse reflectivity
    vec3 Ks; // Specular reflectivity
    float Shininess; // Specular shininess factor
};
uniform MaterialInfo Material;

// -------------- DEL PDF SHADER COOKBOOK -------------- 
vec3 ads( vec3 FragPos, vec3 Normal, vec3 texAlbedo, float texSpecular )
{
    vec3 n;
    if (gl_FrontFacing) {
        n = normalize(Normal);
    } else {
        n = normalize(-Normal);
    }

    vec3 s = normalize( LightPosition - FragPos );
    vec3 v = normalize( -FragPos );
    vec3 r = reflect( -s, n );
    vec3 h = normalize( v + s );

    vec3 Diff = texAlbedo * Material.Kd * max( dot(s, n), 0.0 );
    vec3 Spec = Material.Ks * pow(max(dot(r,n),0.0), Material.Shininess );

    return LightIntensity * (Material.Ka + Diff + Spec);
}


void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;

    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 texAlbedo = texture(gAlbedoSpec, TexCoords).rgb;
    float texSpecular = texture(gAlbedoSpec, TexCoords).a;

    FragColor = vec4(ads(FragPos, Normal, texAlbedo, texSpecular), 1.0);
    //FragColor = vec4(AmbientDiffSpecCalculation(FragPos, light, Diffuse, Specular, Normal), 1.0);
}

   //float distance = length(light.Position - FragPos);
    //if(distance < light.Radius)
    //{
        //TO-DO
    //}

    //vec3 Binormal = texture(gBinormal, TexCoords).rgb;
    //vec3 Tangent = texture(gTangent, TexCoords).rgb;  

    // calculate the bump mapping data
    //mat3 TBN = transpose(mat3(Tangent, Binormal, Normal));
    //vec3 lDir = normalize(TBN * ( light.Position - FragPos ));
    //vec3 vDir = normalize(TBN * ( -FragPos ));

    // get the bump mapping normal
    //vec4 normalBumpMapping = (2.0 * texture(gNormalSpec, TexCoords)) - 1.0;

    // light calculation
    //FragColor = vec4( fatt(FragPos, light.Position) * AmbientDiffSpecCalculationBumpMapping(FragPos, light, Diffuse, Specular, normalBumpMapping, lDir, vDir), 1.0);


/*
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

    //vec3 diffuse = (light.Id * Specular * Kad * max( dot(l, n), 0.0));
    vec3 diffuse = (light.Id * Kad * max( dot(l, n), 0.0));
    vec3 specular = (dot(l, n) < 0.0) ? vec3(0.0) : (light.Is * light.Ks * pow( max( dot(r, v), 0.0), light.Shininess ));

    return diffuse + specular;
}
*/