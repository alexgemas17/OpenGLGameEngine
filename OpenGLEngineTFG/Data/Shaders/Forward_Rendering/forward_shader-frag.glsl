#version 330

in vec3 FragPos; 
in vec3 Normal; 
in vec2 TexCoords;

struct AmbientLight {
    vec3  color;
    float intensity;
};

struct DirectionalLight {
    vec3  color;
    float intensity;
    vec3  direction;
};

struct Light {
    vec3 Position;
    vec3 Color;
    float Intensity;
    
    float Linear;
    float Quadratic;
    float Radius;
};

uniform sampler2D texture;
uniform samplerBuffer lightsTexBuffer;

uniform AmbientLight ambientL;
uniform DirectionalLight directionalL;
const int NR_LIGHTS = 5;
uniform Light lights[NR_LIGHTS];

void main()
{
    vec3 normal = normalize(Normal);

    //Ambient Light
    vec4 ambientColor = vec4(ambientL.color, 1.0) * ambientL.intensity;

    //Directional Light
    float dLightDiffuseFactor = dot(normal, -directionalL.direction);
    vec4 dLightDiffuseColor = ( dLightDiffuseFactor > 0.0 ) ? 
                                vec4(directionalL.color,1.0) * directionalL.intensity * dLightDiffuseFactor;
                                : vec4(0.0);

    //Point Lights
    vec4 pTotalLightDiffuseColor = vec4(0.0);

    for (int i = 0; i < NR_LIGHTS ; i++) {
        float lightRadius = lights[i].Radius;
        vec3 lightPosition = lights[i].Position;

        vec3 pDirection = FragPos - lPosition;
        float pDistance = length(pDirection);

        if (pDistance <= lRadius) {
            float lIntensity = lights[i].Intensity;
            vec3 lColor = texelFetch(lightsTexBuffer, i * 4 ).rgb;
            vec3 lAttenuation = texelFetch(lightsTexBuffer, i * 4 + 3).xyz;

            pDirection = normalize(pDirection);
            float pLightDiffuseFactor = dot(normal, -pDirection);

            vec4 pLightDiffuseColor = ( pLightDiffuseFactor > 0.0 ) ? 
                                        vec4(lColor,1.0) * lIntensity * pLightDiffuseFactor
                                        : vec4(0.0);

        
            float pAttenuation = lAttenuation.x + lAttenuation.y * pDistance + lAttenuation.z * pDistance * pDistance;
            pTotalLightDiffuseColor += pLightDiffuseColor/pAttenuation;
        }
    }
    
    gl_FragColor =  texture2D(texture, TexCoords) * ( ambientColor + dLightDiffuseColor + pTotalLightDiffuseColor);
}
