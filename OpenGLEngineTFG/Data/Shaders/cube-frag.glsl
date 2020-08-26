#version 430

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{             
    vec4 color = texture(texture1, TexCoords);

    const float gamma = 0.8f; //0.8
    const float exposure = 1.0f;
    vec4 result = vec4(1.0) - exp(-color * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec4(1.0 / gamma));
    
    FragColor = result;
}