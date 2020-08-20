#version 430

layout (location = 0) out vec3 gPosition;       // Almacena la posición del fragmento
layout (location = 1) out vec3 gNormal;         // Almacena la normal (Bump mapping o default normal)
layout (location = 2) out vec4 gAlbedo;         // Almacena la información de la textura.

in vec3 FragPos;
in vec2 TexCoords;
in vec3 VertexNormal;
//in mat3 TBN;

uniform sampler2D texture_albedo;
uniform sampler2D texture_normal;

//Functions
vec3 CalcBumpedNormal();

void main()
{    
    // Extraemos información de las texturas
    vec3 albedo = texture(texture_albedo, TexCoords).rgb;
    vec3 normal = texture(texture_normal, TexCoords).rgb;

    // Comprobamos si se realiza el bump mapping o no.
    //if(hasNormalTexture){
        //normal = normalize(TBN *  normalize(normal * 2.0 - 1.0));
    //}else{
        //normal = (gl_FrontFacing) ? normalize(VertexNormal) : normalize(-VertexNormal);
    //}

    // Guardamos la información en el buffer
    gPosition = FragPos;
    gAlbedo.rgb = albedo;
    gNormal = normalize(VertexNormal);
}