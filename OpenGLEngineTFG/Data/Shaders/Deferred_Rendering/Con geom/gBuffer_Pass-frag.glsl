#version 430

layout (location = 0) out vec3 gPosition;       // Almacena la posición del fragmento
layout (location = 1) out vec3 gNormal;         // Almacena la normal (Bump mapping o default normal)
layout (location = 2) out vec4 gAlbedo;         // Almacena la información de la textura.
layout (location = 3) out vec4 gMaterialInfo;   // Almacena la info PBR: R:metallic, G:roughness, B:ao

in vec3 GFragPos;
in vec2 GTexCoords;
in vec3 GVertexNormal;
in mat3 GTBN;

uniform bool hasNormalTexture;

uniform sampler2D texture_albedo;
uniform sampler2D texture_normal;
uniform sampler2D texture_metallic;
uniform sampler2D texture_roughness;
uniform sampler2D texture_ao; 
uniform sampler2D texture_AO_Met_Rough;

//Functions
vec3 CalcBumpedNormal();

void main()
{    
    // Extraemos información de las texturas
    vec3 albedo = texture(texture_albedo, GTexCoords).rgb;
    vec3 normal = texture(texture_normal, GTexCoords).rgb;
    float metallic = texture(texture_metallic, GTexCoords).r;
	float roughness = max(texture(texture_roughness, GTexCoords).r, 0.04);
	float ao = texture(texture_ao, GTexCoords).r;

    //float ao = texture(texture_AO_Met_Rough, GTexCoords).r;
    //float roughness = max(texture(texture_AO_Met_Rough, GTexCoords).g, 0.04);
    //float metallic = texture(texture_AO_Met_Rough, GTexCoords).b;

    // Comprobamos si se realiza el bump mapping o no.
    if(hasNormalTexture){
        normal = normalize(GTBN *  normalize(normal * 2.0 - 1.0));
    }else{
        normal = (gl_FrontFacing) ? normalize(GVertexNormal) : normalize(-GVertexNormal);
    }

    // Guardamos la información en el buffer
    gPosition = GFragPos;
    gAlbedo.rgb = albedo;
    gNormal = normal;
    gMaterialInfo = vec4(metallic, roughness, ao, 1.0);
}