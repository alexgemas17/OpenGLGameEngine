#version 400

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

//BUMP MAPPING
out vec3 Binormal;
out vec3 Tangent;

uniform mat4 mModelView;
uniform mat4 mMVP;

void main()
{
    //Position data
    FragPos = vec3( mModelView * vec4(vPosition, 1.0) );

    //Texture coord data.
    TexCoords = vTexCoord;

    //Bump mapping data
    Normal = vec3( mModelView * vec4(vNormal, 0.0) );
    Tangent = vec3( mModelView * vec4(vTangent, 0.0) );
    Binormal = normalize(cross(Normal, Tangent));

    gl_Position = mMVP * vec4(vPosition, 1.0);
}