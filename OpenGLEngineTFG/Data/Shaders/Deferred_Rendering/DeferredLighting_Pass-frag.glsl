#version 430

layout (location = 0) out vec4 FragColor;

const float PI = 3.14159265359;

//Light Data
struct DirectionalLight {
    vec3 LightDirection;     // Direction of the DirLight in cam coords.

    float Intensity;        // Intensity from the light
    vec3 LightColour;       // Color from the light
};

struct PointLight {
	vec3 Position;              // Position in cam coords

	float Intensity;            // Intensity from the light
	vec3 LightColour;           // Color from the light
	float AttenuationRadius;    
};

struct SpotLight {
	vec3 Position;              // Position in cam coords
	vec3 LightDirection;        // Direction of the spotlight in cam coords.

	float Intensity;            // Intensity from the light
	vec3 LightColour;           // Color from the light
	float AttenuationRadius;

	float CutOff;               // Cutoff angle (between 0 and pi/2
	float OuterCutOff;          // Angular attenuation exponent
};

in vec2 TexCoords;

// GBuffer data
//uniform sampler2D gPosition;        // Almacena la posición del fragmento
uniform sampler2D gNormal;          // Almacena la normal (Bump mapping o default normal)
uniform sampler2D gAlbedo;          // Almacena la información de la textura.
uniform sampler2D gMaterialInfo;    // Almacena la info PBR: R:metallic, G:roughness, B:ao
uniform sampler2D ssaoTexture;
uniform sampler2D shadowMap;
uniform sampler2D depthTexture;

// Información sobre las luces
const int NR_POINT_LIGHTS_MAX = 32;
//uniform int NumDirectionalLights;   Por ahora solo 1
uniform int NumPointLights;
//uniform int NumSpotLights;          Por ahora solo 1

uniform DirectionalLight DirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS_MAX];
uniform SpotLight spotLight;

uniform vec3 viewPosition;
uniform mat4 lightSpaceMatrix;    //Para los calculos de sombras
uniform mat4 viewInverse;
uniform mat4 projectionInverse;

// Funciones que encapsulan las ecucaciones de las luces
vec3 CalculateDirectionalLight(vec3 fragPos, vec3 normal, vec3 albedo, float metallic, float roughness, vec3 fragToView, vec3 baseReflectivity);
vec3 CalculatePointLight(PointLight pointLight, vec3 fragPos, vec3 normal, vec3 albedo, float metallic, float roughness, vec3 fragToView, vec3 baseReflectivity);
vec3 CalculateSpotLight(vec3 fragPos, vec3 normal, vec3 albedo, float metallic, float roughness, vec3 fragToView, vec3 baseReflectivity);

// Cook-Torrance BRDF functions adopted by Epic for UE4
float NormalDistributionGGX(vec3 normal, vec3 halfway, float roughness);
float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness);
float GeometrySchlickGGX(float cosTheta, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 baseReflectivity);

// Shadows
float CalculateShadow(vec3 fragPos, vec3 normal, vec3 fragToLight);
float shadowFactor(vec3 fragPos);
vec3 WorldPosFromDepth();

void main()
{
    // Recuperamos la información del GBuffer
    //vec3 fragPos = texture(gPosition, TexCoords).rgb;
	vec3 normal = texture(gNormal, TexCoords).rgb;
	vec3 albedo = texture(gAlbedo, TexCoords).rgb;
	float metallic = texture(gMaterialInfo, TexCoords).r;
	float unclampedRoughness = texture(gMaterialInfo, TexCoords).g; // Used for indirect specular (reflections)
	float roughness = max(unclampedRoughness, 0.04); // Used for calculations since specular highlights will be too fine, and will cause flicker
	float materialAO = texture(gMaterialInfo, TexCoords).b;
	float sceneAO = texture(ssaoTexture, TexCoords).r;
	float ao = min(materialAO, sceneAO);

    // Reconstruct fragPos
	vec3 fragPos = WorldPosFromDepth();

	vec3 fragToView = normalize(viewPosition - fragPos);
	vec3 reflectionVec = reflect(-fragToView, normal);

	// Dielectrics have an average base specular reflectivity around 0.04, and metals absorb all of their diffuse (refraction) lighting so their albedo is used instead for their specular lighting (reflection)
	vec3 baseReflectivity = vec3(0.04);
	baseReflectivity = mix(baseReflectivity, albedo, metallic);

	// Calculate per light radiance for all of the direct lighting
	vec3 directLightIrradiance = vec3(0.0);

    //for (int i = 0; i < NumDirectionalLights.x; i++)
	directLightIrradiance += CalculateDirectionalLight(fragPos, normal, albedo, metallic, roughness, fragToView, baseReflectivity);
    
    for (int i = 0; i < NumPointLights; i++)
        directLightIrradiance += CalculatePointLight(pointLights[i], fragPos, normal, albedo, metallic, roughness, fragToView, baseReflectivity);

    //for (int i = 0; i < NumSpotLights.x; i++)
	directLightIrradiance += CalculateSpotLight(fragPos, normal, albedo, metallic, roughness, fragToView, baseReflectivity);

	// Calcualte ambient IBL for both diffuse and specular
	vec3 ambient = vec3(0.05) * albedo * ao;

	FragColor = vec4(ambient + directLightIrradiance, 1.0);
}

// ----------------------------------------- ECUACIONES LUCES -------------------------------
// TODO: Need to also add multiple shadow support
vec3 CalculateDirectionalLight(vec3 fragPos, vec3 normal, vec3 albedo, float metallic, float roughness, vec3 fragToView, vec3 baseReflectivity) {
	vec3 lightDir = normalize(-DirLight.LightDirection);
    vec3 halfway = normalize(lightDir + fragToView);
    vec3 radiance = DirLight.Intensity * DirLight.LightColour;

    // Cook-Torrance Specular BRDF calculations
    float normalDistribution = NormalDistributionGGX(normal, halfway, roughness);
    vec3 fresnel = FresnelSchlick(max(dot(halfway, fragToView), 0.0), baseReflectivity);
    float geometry = GeometrySmith(normal, fragToView, lightDir, roughness);

    // Calculate reflected and refracted light respectively, and since metals absorb all refracted light, we nullify the diffuse lighting based on the metallic parameter
    vec3 specularRatio = fresnel;
    vec3 diffuseRatio = vec3(1.0) - specularRatio;
    diffuseRatio *= 1.0 - metallic;

    // Finally calculate the specular part of the Cook-Torrance BRDF (max 0.1 stops any visual artifacts)
    vec3 numerator = specularRatio * normalDistribution * geometry;
    float denominator = 4 * max(dot(fragToView, normal), 0.1) * max(dot(lightDir, normal), 0.0) + 0.001;  // Prevents any division by zero
    vec3 specular = numerator / denominator;

    // Also calculate the diffuse, a lambertian calculation will be added onto the final radiance calculation
    vec3 diffuse = diffuseRatio * albedo / PI;

    // Add the light's radiance to the irradiance sum
    return (diffuse + specular) * radiance * max(dot(normal, lightDir), 0.0) * (1.0 - CalculateShadow(fragPos, normal, lightDir));
}

vec3 CalculatePointLight(PointLight pointLight, vec3 fragPos, vec3 normal, vec3 albedo, float metallic, float roughness, vec3 fragToView, vec3 baseReflectivity) {
	vec3 fragToLight = normalize(pointLight.Position - fragPos);
    vec3 halfway = normalize(fragToView + fragToLight);
    float fragToLightDistance = length(pointLight.Position - fragPos);

    // Attenuation calculation (based on Epic's UE4 falloff model)
    float d = fragToLightDistance / pointLight.AttenuationRadius;
    float d2 = d * d;
    float d4 = d2 * d2;
    float falloffNumerator = clamp(1.0 - d4, 0.0, 1.0);
    float attenuation = (falloffNumerator * falloffNumerator) / ((fragToLightDistance * fragToLightDistance) + 1.0);
    vec3 radiance = pointLight.Intensity * pointLight.LightColour * attenuation;

    // Cook-Torrance Specular BRDF calculations
    float normalDistribution = NormalDistributionGGX(normal, halfway, roughness);
    vec3 fresnel = FresnelSchlick(max(dot(halfway, fragToView), 0.0), baseReflectivity);
    float geometry = GeometrySmith(normal, fragToView, fragToLight, roughness);

    // Calculate reflected and refracted light respectively, and since metals absorb all refracted light, we nullify the diffuse lighting based on the metallic parameter
    vec3 specularRatio = fresnel;
    vec3 diffuseRatio = vec3(1.0) - specularRatio;
    diffuseRatio *= 1.0 - metallic;

    // Finally calculate the specular part of the Cook-Torrance BRDF (max 0.1 stops any visual artifacts)
    vec3 numerator = specularRatio * normalDistribution * geometry;
    float denominator = 4 * max(dot(fragToView, normal), 0.1) * max(dot(fragToLight, normal), 0.0) + 0.001; // Prevents any division by zero
    vec3 specular = numerator / denominator;

    // Also calculate the diffuse, a lambertian calculation will be added onto the final radiance calculation
    vec3 diffuse = diffuseRatio * albedo / PI;

    // Add the light's radiance to the irradiance sum
    return (diffuse + specular) * radiance * max(dot(normal, fragToLight), 0.0);
}

vec3 CalculateSpotLight(vec3 fragPos, vec3 normal, vec3 albedo, float metallic, float roughness, vec3 fragToView, vec3 baseReflectivity) {
	vec3 fragToLight = normalize(spotLight.Position - fragPos);
    vec3 halfway = normalize(fragToView + fragToLight);
    float fragToLightDistance = length(spotLight.Position - fragPos);

    // Attenuation calculation (based on Epic's UE4 falloff model)
    float d = fragToLightDistance / spotLight.AttenuationRadius;
    float d2 = d * d;
    float d4 = d2 * d2;
    float falloffNumerator = clamp(1.0 - d4, 0.0, 1.0);

    // Check if it is in the spotlight's circle
    float theta = dot(normalize(spotLight.LightDirection), -fragToLight);
    float difference = spotLight.CutOff - spotLight.OuterCutOff;
    float intensity = clamp((theta - spotLight.OuterCutOff) / difference, 0.0, 1.0);
    float attenuation = intensity * (falloffNumerator * falloffNumerator) / ((fragToLightDistance * fragToLightDistance) + 1.0);
    vec3 radiance = spotLight.Intensity * spotLight.LightColour * attenuation;

    // Cook-Torrance Specular BRDF calculations
    float normalDistribution = NormalDistributionGGX(normal, halfway, roughness);
    vec3 fresnel = FresnelSchlick(max(dot(halfway, fragToView), 0.0), baseReflectivity);
    float geometry = GeometrySmith(normal, fragToView, fragToLight, roughness);

    // Calculate reflected and refracted light respectively, and since metals absorb all refracted light, we nullify the diffuse lighting based on the metallic parameter
    vec3 specularRatio = fresnel;
    vec3 diffuseRatio = vec3(1.0) - specularRatio;
    diffuseRatio *= 1.0 - metallic;

    // Finally calculate the specular part of the Cook-Torrance BRDF (max 0.1 stops any visual artifacts)
    vec3 numerator = specularRatio * normalDistribution * geometry;
    float denominator = 4 * max(dot(fragToView, normal), 0.1) * max(dot(fragToLight, normal), 0.0) + 0.001; // Prevents any division by zero
    vec3 specular = numerator / denominator;

    // Also calculate the diffuse, a lambertian calculation will be added onto the final radiance calculation
    vec3 diffuse = diffuseRatio * albedo / PI;

    // Add the light's radiance to the irradiance sum
    return (diffuse + specular) * radiance * max(dot(normal, fragToLight), 0.0);
}

// ----------------------------------------- PBR -------------------------------
// Approximates the amount of microfacets that are properly aligned with the halfway vector, thus determines the strength and area for specular light
float NormalDistributionGGX(vec3 normal, vec3 halfway, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float normDotHalf = dot(normal, halfway);
	float normDotHalf2 = normDotHalf * normDotHalf;

	float numerator = a2;
	float denominator = normDotHalf2 * (a2 - 1.0) + 1.0;
	denominator = PI * denominator * denominator;

	return numerator / denominator;
}

// Approximates the geometry obstruction and geometry shadowing respectively, on the microfacet level
float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness) {
	return GeometrySchlickGGX(max(dot(normal, viewDir), 0.0), roughness) * GeometrySchlickGGX(max(dot(normal, lightDir), 0.0), roughness);
}

float GeometrySchlickGGX(float cosTheta, float roughness) {
	float r = (roughness + 1.0);
	float k = (roughness * roughness) / 8.0;

	float numerator = cosTheta;
	float denominator = cosTheta * (1.0 - k) + k;

	return numerator / max(denominator, 0.001);
}

// Calculates the amount of specular light. Since diffuse(refraction) and specular(reflection) are mutually exclusive, 
// we can also use this to determine the amount of diffuse light
// Taken from UE4's implementation which is faster and basically identical to the usual Fresnel calculations: https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
vec3 FresnelSchlick(float cosTheta, vec3 baseReflectivity) {
	return max(baseReflectivity + (1.0 - baseReflectivity) * pow(2, (-5.55473 * cosTheta - 6.98316) * cosTheta), 0.0);
}

// ----------------------------------------- SHADOW -------------------------------
float CalculateShadow(vec3 fragPos, vec3 n, vec3 fragToLight) {
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);

    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(n);
    vec3 lightDir = normalize(fragToLight - fragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }

    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

/*
float shadowFactor(vec3 fragPos) {
    //float shadow = textureProj(ShadowMap, shadowCoord);
    //float shadowFactor = clamp(shadow, shadowMin, 1.0);
    //return shadowFactor;

    vec4 shadowCoord = mShadowMatrix * vec4(fragPos, 1.0);

    // perform perspective divide
    vec3 projCoords = shadowCoord.xyz / shadowCoord.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    float sum = 0;
    sum += textureProjOffset(shadowMap, projCoords, ivec2(-1,-1)).r;
    sum += textureProjOffset(shadowMap, projCoords, ivec2(-1,1)).r;
    sum += textureProjOffset(shadowMap, projCoords, ivec2(1,1)).r;
    sum += textureProjOffset(shadowMap, projCoords, ivec2(1,-1)).r;
    sum += textureProjOffset(shadowMap, projCoords, ivec2(0,0)).r;

    float shadowFactor = sum * (1.0/5.0);
    shadowFactor = clamp(shadowFactor, shadowMin, 1.0);
    return shadowFactor;
}
*/

vec3 WorldPosFromDepth() {
    float z = 2.0 * texture(depthTexture, TexCoords).r - 1.0; // [-1, 1]
    vec4 clipSpacePos = vec4(TexCoords * 2.0 - 1.0 , z, 1.0);
    vec4 viewSpacePos = projectionInverse * clipSpacePos;

    viewSpacePos /= viewSpacePos.w; // Perspective division

    vec4 worldSpacePos = viewInverse * viewSpacePos;

    return worldSpacePos.xyz;
}