#version 430

layout (location = 0) out vec3 gPosition;       // Almacena la posición del fragmento
layout (location = 1) out vec3 gNormal;         // Almacena la normal (Bump mapping o default normal)
layout (location = 2) out vec4 gAlbedo;         // Almacena la información de la textura.

in vec3 FragPos;
in vec2 TexCoords;
in vec3 VertexNormal;
in mat3 TBN;

uniform sampler2D texture_albedo;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;
uniform bool has_texture_normal;

//Functions
vec3 CalcBumpedNormal();

void main()
{    
    // Extraemos información de las texturas
    vec3 albedo = texture(texture_albedo, TexCoords).rgb;
    gAlbedo.a = texture(texture_specular, TexCoords).a;

    // Comprobamos si se realiza el bump mapping o no.
    vec3 normal = vec3(0.0f);
    if(has_texture_normal){
        normal = texture(texture_normal, TexCoords).rgb;
        normal = normalize(TBN *  normalize(normal * 2.0 - 1.0));
    }else{
        normal = normalize(VertexNormal);
    }

    // Guardamos la información en el buffer
    gPosition = FragPos;
    gAlbedo.rgb = albedo;
    gNormal = normal;
}