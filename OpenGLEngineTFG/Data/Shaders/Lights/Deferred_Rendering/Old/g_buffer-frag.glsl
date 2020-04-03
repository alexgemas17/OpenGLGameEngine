#version 400

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 gNormalSpec;

layout (location = 4) out vec3 gBinormal;
layout (location = 5) out vec3 gTangent;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

//BUMP MAPPING
in vec3 Binormal;
in vec3 Tangent;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;

    // also store the per-fragment normals into the gbuffer
    gNormal = Normal;

    // store the bump mapping data
    gBinormal = Binormal;
    gTangent = Tangent;

    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(texture_diffuse, TexCoords).rgb;

    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(texture_specular, TexCoords).r;

    // and the diffuse per-fragment color
    gNormalSpec = texture(texture_normal, TexCoords);
}