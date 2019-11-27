#version 400

layout (location = 0) out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPosition; 
uniform vec3 viewPosition; 
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // Componente ambiental
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // Componente difuso 
    float intensity = 0.7;
	vec3 n = (gl_FrontFacing) ? normalize(Normal) : normalize(-Normal);

    vec3 lightDir = normalize(lightPosition - FragPos);
    vec3 v = normalize( -FragPos );
    vec3 r = reflect( -lightDir, n );
    //vec3 norm = normalize(Normal);
    //float diff = max(dot(norm, lightDir), 0.0);

    //vec3 diffuse = diff * lightColor;
    vec3 diffuse = (intensity * lightColor * max(dot(lightDir, n), 0.0));
    
    // Componente Especular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPosition - FragPos);
    //vec3 reflectDir = reflect(-lightDir, n);  
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    //vec3 specular = specularStrength * spec * lightColor;  

    vec3 specular = (dot(lightDir, n) < 0.0) ? vec3(0.0) : (specularStrength * lightColor * pow(max(dot(r, v), 0.0), 32));
        
    //Resultado
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
} 