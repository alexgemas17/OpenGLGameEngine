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
vec3 CalcDirLight( vec3 FragPos, vec3 Normal, vec4 ColorTexture  );
vec3 CalcPointLight(PointLightData pointLight, vec3 FragPos, vec3 Normal, vec4 ColorTexture);
vec3 CalcSpotLight( vec3 FragPos, vec3 Normal, vec4 ColorTexture );

float fatt(vec3 FragPos, vec3 LightPosition);

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec4 Diffuse = texture(gAlbedo, TexCoords).rgba; 

    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================

    // phase 1: directional lighting
    //vec3 result = CalcDirLight(FragPos, Normal, Diffuse);

    vec3 result = vec3(0.0);
    // phase 2: point lights
    for(int i = 0; i < nr_point_lights; i++)
        result += CalcPointLight(pointLights[i], FragPos, Normal, Diffuse);  

    // phase 3: spot light
    result += CalcSpotLight(FragPos, Normal, Diffuse);    
    
    FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight( vec3 FragPos, vec3 Normal, vec4 ColorTexture  ) {
    vec3 Kad = ColorTexture.rgb; //Info del color de la textura
    
    vec3 l = -dirLight.LightDirection;
    vec3 v = normalize( -FragPos );
    vec3 r = reflect( -l, Normal );

    //vec3 Ambient = dirLight.AmbientIntensity * Kad;

    float dotLN = dot(l, Normal);
    vec3 Diffuse = Kad * max( dotLN, 0.0);
    vec3 Specular = vec3(0.0);
    if(dotLN >= 0.0){
        Specular = Material.Ks * pow( max( dot(r,v), 0.0), Material.Shininess );
    }

    return  dirLight.DiffSpecIntensity * (Diffuse + Specular);
}

vec3 CalcPointLight(PointLightData pointLight, vec3 FragPos, vec3 Normal, vec4 ColorTexture)
{
    vec3 Kad = ColorTexture.rgb; //Info del color de la textura
    
    vec3 l = normalize( pointLight.Position - FragPos );
    vec3 v = normalize( -FragPos );
    vec3 r = reflect( -l, Normal );

    //vec3 Ambient = pointLight.AmbientIntensity * Kad;

    float dotLN = dot(l, Normal);
    vec3 Diffuse = pointLight.DiffSpecIntensity * Kad * max( dotLN, 0.0);
    vec3 Specular = vec3(0.0);
    if(dotLN >= 0.0){
        Specular = pointLight.DiffSpecIntensity * Material.Ks * pow( max( dot(r,v), 0.0), Material.Shininess );
    }

    return fatt(FragPos, pointLight.Position) * (Diffuse + Specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight( vec3 FragPos, vec3 Normal, vec4 ColorTexture ) { 
    vec3 Kad = ColorTexture.rgb; //Info del color de la textura
    
    vec3 l = normalize(SpotLight.Position - FragPos);
	vec3 d = normalize(SpotLight.Direction);

	float DotResult = dot(-l, d);
	//cosGamma se realiza fuera en opengl.
	float spotFactor = (DotResult < SpotLight.Cutoff) ? 0.0 : pow(DotResult, SpotLight.Exponent);

    vec3 v = normalize( -FragPos );
    vec3 r = reflect( -l, Normal );

    //vec3 Ambient = dirLight.AmbientIntensity * Kad;

    float dotLN = dot(l, Normal);
    vec3 Diffuse = Kad * max( dotLN, 0.0);
    vec3 Specular = vec3(0.0);
    if(dotLN >= 0.0){
        Specular = Material.Ks * pow( max( dot(r,v), 0.0), Material.Shininess );
    }

    vec3 result = SpotLight.DiffSpecIntensity * (Diffuse + Specular);

    return spotFactor * (fatt(FragPos, SpotLight.Position) * result) ;
}

float fatt(vec3 FragPos, vec3 LightPosition) {
	float c1 = 1.0;
	float c2 = 0.0;
	float c3 = 0.0;

	float distancia = distance(FragPos, LightPosition);

	return min(1.0 / (c1 + (c2 * distancia * distancia) + (c3 * pow(distancia, 2))), 1.0);
}