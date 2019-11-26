#include "Render.h"

#include "../Loaders/lodepng.h"

Render::Render(): VAO(0), VBO(0), IBO(0), texture(0) {}

Render::Render(float _vertices[], unsigned int _indices[]):
	VAO(0), VBO(0), IBO(0), texture(0), vertices(_vertices), indices(_indices),
	typeRender(Puntos)
{}

Render::Render(float _vertices[], unsigned int _indices[], std::string imgUrl) :
	VAO(0), VBO(0), IBO(0), texture(0), vertices(_vertices), indices(_indices),
	typeRender(PuntosTextura), urlImg(imgUrl)
{}

Render::Render(std::vector<glm::vec3> puntos, std::vector<GLuint> index, std::vector<glm::vec3> normales, std::vector<glm::vec2> coordenada_textura, std::string urlImg)
{
	this->model.puntos = puntos;
	this->model.normales = normales;
	this->model.index = index;
	this->model.coordenada_textura = coordenada_textura;
	this->model.urlImg = urlImg;
}

Render::~Render()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

//------------------------------- PUBLIC -------------------------------
void Render::Init()
{
	InitVAO();
	InitVBO();
	InitIBO();
	InitCoordTextura();
	if(this->model.urlImg != "")
		InitTextura();
}

void Render::Draw()
{
	//shader->use(); --> NOTA: Se llama antes en el DrawObj del SceneObj.

	if (this->model.urlImg != "")
		glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); 

	glDrawElements(GL_TRIANGLE_STRIP, this->model.index.size(), GL_UNSIGNED_INT, nullptr);
	//glDrawElements(GL_LINE_STRIP, this->model.index.size(), GL_UNSIGNED_INT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

ImageData Render::loadImage(std::string url) {
	ImageData data;

	unsigned error = lodepng::decode(data.image, data.width, data.height, url);
	if (error)
	{
		std::cout << url << " cannot be loaded" << std::endl;
		return data;
	}

	return data;
}

//------------------------------- PRIVATE -------------------------------
void Render::InitVAO() 
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &CoordTexturaBuffer);
	glGenBuffers(1, &IBO);
}

void Render::InitVBO() 
{
	//Enlazamos las siguientes funciones al VAO
	glBindVertexArray(VAO);

	// Enlazamos el VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, sizeof(glm::vec3) / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(glm::vec3), ((GLubyte*)NULL + (0)));

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * this->model.puntos.size(), this->model.puntos.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Render::InitIBO() 
{
	//Enlazamos las siguientes funciones al VAO
	glBindVertexArray(VAO);

	//Enlazamos el VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * this->model.index.size(), this->model.index.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Render::InitCoordTextura()
{
	//Enlazamos las siguientes funciones al VAO
	glBindVertexArray(VAO);

	// Enlazamos el CoordTexturaBuffer
	glBindBuffer(GL_ARRAY_BUFFER, CoordTexturaBuffer);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, sizeof(glm::vec2) / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(glm::vec2), ((GLubyte*)NULL + (0)));

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * this->model.coordenada_textura.size(), this->model.coordenada_textura.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Render::InitTextura() 
{
	//Enlazamos las siguientes funciones al VAO
	glBindVertexArray(VAO);

	//Generamos la texturaID
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//Opciones para las texturas
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Obtenemos los datos de la imagen
	this->data = loadImage(this->model.urlImg);

	//Generamos la imagen
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->data.width, this->data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->data.image.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}