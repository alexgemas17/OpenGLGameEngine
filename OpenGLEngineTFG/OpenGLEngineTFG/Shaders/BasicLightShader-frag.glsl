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

void main()
{
	// Ambient

	//vec3 n = (gl_FrontFacing) ? normalize(Normal) : normalize(-Normal);
	vec3 n = normalize( Normal );

	vec3 l = normalize( light.position - FragPos );
	vec3 v = normalize( -FragPos );
	vec3 r = reflect( -l, n );

    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * material.diffuse * max(dot(l,n), 0.0);
    vec3 specular = (dot(l, n) < 0.0) ? vec3(0.0) : (light.specular * material.specular * pow(max(dot(r, v), 0.0), material.shininess));
        
    //Resultado
    FragColor = vec4((ambient + diffuse + specular), 1.0);
}