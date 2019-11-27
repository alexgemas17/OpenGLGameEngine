#version 400

layout (location = 0) out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
  
in vec3 Normal;  
in vec3 FragPos; 

uniform Material material;
uniform Light light;

uniform vec3 viewPos; 

void main()
{
	// Ambient
    vec3 ambient = light.ambient * material.ambient;

    // diff
	vec3 n = (gl_FrontFacing) ? normalize(Normal) : normalize(-Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 diffuse = light.diffuse * (material.diffuse * max(dot(n, lightDir), 0.0));
 
 	// specular
 	vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir  = reflect( -viewDir, n );
    
    //vec3 specular = (dot(lightDir, n) < 0.0) ? vec3(0.0) : (specularStrength * lightColor * pow(max(dot(r, v), 0.0), 32));
    vec3 specular = light.specular * (material.specular * pow(max(dot(viewDir, reflectDir), 0.0), material.shininess));
        
    //Resultado
    FragColor = vec4(ambient + diffuse + specular, 1.0);
} 