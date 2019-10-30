#include "Scene.h"

Scene::Scene() 
{
	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	std::vector<glm::vec3> puntos = {
		glm::vec3(0.5f,  0.5f, 0.0f),
		glm::vec3(0.5f, -0.5f, 0.0f),
		glm::vec3(-0.5f, -0.5f, 0.0f),
		glm::vec3(-0.5f,  0.5f, 0.0f)
	};

	std::vector<GLuint> index = {
		0,1,2,3
	};

	std::vector<glm::vec3> color = {
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 0.0f)
	};

	std::vector<glm::vec2> coordenada_textura = {
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f)
	};



	//this->triangulo = new Render(vertices, indices, "..\\Data\\Texturas\\wall.png");
	this->triangulo = new Render(puntos, index, color, coordenada_textura,"..\\Data\\Texturas\\wall.png");
}

Scene::~Scene() {}

void Scene::InitObjs()
{
	this->triangulo->Init();
}

/* Recorremos los objetos que necesiten actualizar su estado */
void Scene::UpdateObjs()
{
	//TODO
}

void Scene::DrawObjs(PagShaderProgram* shader)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	this->triangulo->Draw(shader);
}
