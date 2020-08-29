// ------------------ DEFERRED RENDERING ------------------
#version 430 core

layout (location = 0) out vec3 gPosition;       // Almacena la posición del fragmento
layout (location = 1) out vec3 gNormal;         // Almacena la normal (Bump mapping o default normal)
layout (location = 2) out vec4 gAlbedo;         // Almacena la información de la textura.

in VS_OUT {
    in vec3 FragPos; 
    in vec3 VertexNormal; 
    in vec2 TexCoords;
    in mat3 TBN;
} fragment_in;

uniform sampler2D texture_albedo;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;
uniform bool has_texture_normal;

void main()
{    
    // Comprobamos si se realiza el bump mapping o no.
    vec3 Normal = vec3(0.0f);
    if(has_texture_normal){
        Normal = texture(texture_normal, fragment_in.TexCoords).rgb;
        Normal = Normal * 2.0 - 1.0;
        Normal = normalize(fragment_in.TBN * Normal);
    }else{
        Normal = (gl_FrontFacing) ? normalize(fragment_in.VertexNormal) : normalize(-fragment_in.VertexNormal);
    }

    // Guardamos la información en el buffer
    gPosition = fragment_in.FragPos;
    gAlbedo.rgb = texture(texture_albedo, fragment_in.TexCoords).rgb;
    gAlbedo.a = texture(texture_specular, fragment_in.TexCoords).a;
    gNormal = Normal;
}