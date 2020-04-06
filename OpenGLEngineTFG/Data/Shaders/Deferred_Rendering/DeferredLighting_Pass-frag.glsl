#version 430

layout (location = 0) out vec4 FragColor;

struct DirectionLightData {
    vec3 LightDirection;    // Light direction in cam coords
    vec3 DiffSpecIntensity; // Diffuse/spec intensity
    vec3 AmbientIntensity;  // Amb intensity
};

struct PointLightData {
    vec3 Position;              // Light position in cam. coords.
    vec3 DiffSpecIntensity;     // D,S intensity
    vec3 AmbientIntensity;      // Amb intensity
};

struct SpotLightData {
    vec3 Position;              // Position in cam coords
    vec3 Direction;             // Direction of the spotlight in cam coords.
    vec3 DiffSpecIntensity;     // Diffuse/spec intensity
    vec3 AmbientIntensity;      // Amb intensity
    float Exponent;             // Angular attenuation exponent
    float Cutoff;               // Cutoff angle (between 0 and pi/2)
};

struct MaterialInfo {
    vec3 Ks;            // Specular reflectivity
    float Shininess;    // Specular shininess factor
};

in vec2 TexCoords;

// Data info from Framebuffers
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

// Num max light
const int NR_POINT_LIGHTS_MAX = 32;

// Lights
uniform MaterialInfo Material;
uniform int nr_point_lights;
uniform DirectionLightData dirLight;
uniform PointLightData pointLights[NR_POINT_LIGHTS_MAX];
uniform SpotLightData SpotLight;

uniform vec3 viewPos;

// Light functions
vec3 CalcDirLight( vec3 fragPos, vec3 n, vec3 texColor );
vec3 CalcPointLight( PointLightData pointLight, vec3 fragPos, vec3 n, vec3 texColor );
vec3 CalcSpotLight( vec3 position, vec3 n, vec3 texColor );

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedo, TexCoords).rgb; 

    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================

    // phase 1: directional lighting
    vec3 result = CalcDirLight(FragPos, Normal, Diffuse);

    // phase 2: point lights
    for(int i = 0; i < nr_point_lights; i++)
        result += CalcPointLight(pointLights[i], FragPos, Normal, Diffuse);  

    // phase 3: spot light
    result += CalcSpotLight(FragPos, Normal, Diffuse);    
    
    FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight( vec3 fragPos, vec3 n, vec3 texColor  ) {
    vec3 ambient = dirLight.AmbientIntensity * texColor;

    vec3 s = -dirLight.LightDirection;
    float sDotN = max( dot(s,n), 0.0 );

    vec3 diffuse = texColor * sDotN;
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 ) {
        vec3 v = normalize(-fragPos.xyz);
        vec3 h = normalize( v + s );
        spec = Material.Ks * pow( max( dot(h,n), 0.0 ), Material.Shininess );
    }

    return ambient + dirLight.DiffSpecIntensity * (diffuse + spec);
}

vec3 CalcPointLight( PointLightData pointLight, vec3 fragPos, vec3 n, vec3 texColor  ) {
    vec3 ambient = pointLight.AmbientIntensity * texColor;

    vec3 s = normalize( pointLight.Position.xyz - fragPos );
    float sDotN = max( dot(s,n), 0.0 );

    vec3 diffuse = texColor * sDotN;
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 ) {
        vec3 v = normalize(-fragPos.xyz);
        vec3 h = normalize( v + s );
        spec = Material.Ks * pow( max( dot(h,n), 0.0 ), Material.Shininess );
    }

    return ambient + pointLight.DiffSpecIntensity * (diffuse + spec);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight( vec3 fragPos, vec3 n, vec3 texColor ) { 
    vec3 ambient = SpotLight.AmbientIntensity *  texColor;
    vec3 diffuse = vec3(0);
    vec3 spec = vec3(0);

    vec3 s = normalize( SpotLight.Position - fragPos );

    float cosAng = dot(-s, normalize(SpotLight.Direction));
    float angle = acos( cosAng );

    float spotScale = 0.0;
    if(angle < SpotLight.Cutoff ) {
        spotScale = pow( cosAng, SpotLight.Exponent );
        float sDotN = max( dot(s,n), 0.0 );
        diffuse = texColor * sDotN;
        if( sDotN > 0.0 ) {
            vec3 v = normalize(-fragPos.xyz);
            vec3 h = normalize( v + s );
            spec = Material.Ks * pow( max( dot(h,n), 0.0 ), Material.Shininess );
        }
    }

    return ambient + spotScale * SpotLight.DiffSpecIntensity * (diffuse + spec);
}