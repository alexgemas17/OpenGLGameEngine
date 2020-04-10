#version 430

layout (location = 0) in vec3 vPosition;

//uniform mat4 lightSpaceMatrix;
//uniform mat4 ModelMatrix;
uniform mat4 MVP; //projection * view * model

void main() {
	//gl_Position = lightSpaceMatrix * ModelMatrix * vec4(vPosition, 1.0f);
	gl_Position = MVP * vec4(vPosition, 1.0);
}
