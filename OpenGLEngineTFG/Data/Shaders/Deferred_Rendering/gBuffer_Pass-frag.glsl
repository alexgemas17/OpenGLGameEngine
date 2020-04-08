#version 430

layout (location = 0) out vec3 gPosition;       // Almacena la posición del fragmento
layout (location = 1) out vec3 gNormal;         // Almacena la normal (Bump mapping o default normal)
layout (location = 2) out vec4 gAlbedo;         // Almacena la información de la textura.
layout (location = 3) out vec4 gMaterialInfo;   // Almacena la info PBR: R:metallic, G:roughness, B:ao

in vec3 FragPos;
in vec2 TexCoords;
in vec3 VertexNormal;
in mat3 TBN;

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
    vec3 albedo = texture(texture_albedo, TexCoords).rgb;
    vec3 normal = texture(texture_normal, TexCoords).rgb;
    float metallic = texture(texture_metallic, TexCoords).r;
	float roughness = max(texture(texture_roughness, TexCoords).r, 0.04);
	float ao = texture(texture_ao, TexCoords).r;

    //float ao = texture(texture_AO_Met_Rough, TexCoords).r;
    //float roughness = max(texture(texture_AO_Met_Rough, TexCoords).g, 0.04);
    //float metallic = texture(texture_AO_Met_Rough, TexCoords).b;

    // Comprobamos si se realiza el bump mapping o no.
    if(hasNormalTexture){
        normal = normalize(TBN *  normalize(normal * 2.0 - 1.0));
    }else{
        normal = (gl_FrontFacing) ? normalize(VertexNormal) : normalize(-VertexNormal);
    }

    // Guardamos la información en el buffer
    gPosition = FragPos;
    gAlbedo.rgb = albedo;
    gNormal = normal;
    gMaterialInfo = vec4(metallic, roughness, ao, 1.0);
}