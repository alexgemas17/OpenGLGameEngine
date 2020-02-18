#version 400

layout (location = 0) out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    vec3 specular;    
    float shininess;
}; 

struct DirLight {
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define NR_POINT_LIGHTS 4

in vec3 Normal;  
in vec3 FragPos; 
in vec2 TexCoords;

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;

// function prototypes
vec3 CalcDirLight(DirLight light);
vec3 CalcPointLight(PointLight light, vec3 fragPos);
vec3 CalcSpotLight(SpotLight light, vec3 fragPos);

// -------------- MAIN SHADER --------------
void main()
{    
    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================

    // phase 1: directional lighting
    vec3 result = CalcDirLight(dirLight);

    // phase 2: point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], FragPos);   

    // phase 3: spot light
    result += CalcSpotLight(spotLight, FragPos);    
    
    FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light)
{
    vec3 n = (gl_FrontFacing) ? normalize(Normal) : normalize(-Normal);

    vec3 l = normalize(-light.direction);
    vec3 v = normalize( -FragPos );
    vec3 r = reflect( -l, n );

    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = light.diffuse * texture(material.diffuse, TexCoords).rgb * max(dot(l,n), 0.0);
    vec3 specular = (dot(l, n) < 0.0) ? vec3(0.0) : (light.specular * material.specular * pow(max(dot(r, v), 0.0), material.shininess));

    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 fragPos)
{
    vec3 n = (gl_FrontFacing) ? normalize(Normal) : normalize(-Normal);

    vec3 l = normalize( light.position - FragPos );
    vec3 v = normalize( -FragPos );
    vec3 r = reflect( -l, n );

    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = light.diffuse * texture(material.diffuse, TexCoords).rgb * max(dot(l,n), 0.0);
    vec3 specular = (dot(l, n) < 0.0) ? vec3(0.0) : (light.specular * material.specular * pow(max(dot(r, v), 0.0), material.shininess));
        
    //----
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    //----
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 fragPos)
{
    vec3 l = normalize( light.position - FragPos );
    vec3 d = normalize( light.direction );

    //Vemos si es una cara externa o interna para ilumunarla de una forma determinada.
    vec3 n = (gl_FrontFacing) ? normalize(Normal) : normalize(-Normal);
    vec3 v = normalize( -FragPos );
    vec3 r = reflect( -l, n );

    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = light.diffuse * texture(material.diffuse, TexCoords).rgb * max(dot(l,n), 0.0);
    vec3 specular = (dot(l, n) < 0.0) ? vec3(0.0) : (light.specular * material.specular * pow(max(dot(r, v), 0.0), material.shininess));
        
    //----
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    // spotlight intensity
    float theta = dot(l, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    //----

    return (ambient + diffuse + specular);
}