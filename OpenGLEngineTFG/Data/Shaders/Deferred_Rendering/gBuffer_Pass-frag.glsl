#version 400

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

in vec3 VertexNormal;
in vec3 VertexTangent;
in vec2 TexCoords;
in vec3 FragPos;

uniform sampler2D Albedo_texture;
uniform sampler2D Normal_texture;
uniform bool hasNormalTexture;

vec3 CalcBumpedNormal();

void main()
{    
    // Guardamos su posición para luego poder hacer calculos.
    gPosition = FragPos;

    // Calcula la nueva normal modificada usando la textura de normales (Bump mapping)
    gNormal = ( hasNormalTexture ) ? CalcBumpedNormal() : VertexNormal;

    // Guardamos la textura del objeto
    gAlbedo.rgb = texture(Albedo_texture, TexCoords).rgb;
}

vec3 CalcBumpedNormal()
{
    vec3 Normal = normalize(VertexNormal);
    vec3 Tangent = normalize(VertexTangent);

    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);

    vec3 Bitangent = cross(Tangent, Normal);

    vec3 BumpMapNormal = texture(Normal_texture, TexCoords).xyz;

    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);

    vec3 NewNormal;

    mat3 TBN = mat3(Tangent, Bitangent, Normal);

    NewNormal = TBN * BumpMapNormal;

    NewNormal = normalize(NewNormal);

    return NewNormal;
}