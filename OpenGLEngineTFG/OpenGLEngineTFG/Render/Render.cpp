#include "Render.h"

#include "../Loaders/lodepng.h"

Render::Render(float _vertices[], unsigned int _indices[]):
	VAO(0), VBO(0), EBO(0), texture(0), vertices(_vertices), indices(_indices),
	typeRender(Puntos)
{}

Render::Render(float _vertices[], unsigned int _indices[], std::string imgUrl) :
	VAO(0), VBO(0), EBO(0), texture(0), vertices(_vertices), indices(_indices),
	typeRender(PuntosTextura), urlImg(imgUrl)
{}

Render::~Render()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

//------------------------------- PUBLIC -------------------------------
void Render::Init() {
	InitVAO();
	InitPuntos();

	if (this->typeRender == PuntosTextura) InitTextura();

	//ImageData data = loadImage("..\\Data\\Texturas\\wall.png");
}

void Render::Draw(PagShaderProgram* shader)
{
	//Especificamos el shader program
	shader->use();

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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

	// La textura se carga del revés, así que vamos a darle la vuelta
	unsigned char* imgPtr = &data.image[0];
	int numColorComponents = 4;
	int wInc = data.width * 4; //width in char
	unsigned char* top = nullptr;
	unsigned char* bot = nullptr;
	unsigned char temp = 0;
	for (int i = 0; i < data.height / 2; i++)
	{
		top = imgPtr + i * wInc;
		bot = imgPtr + (data.height - i - 1) * wInc;
		for (int j = 0; j < wInc; j++)
		{
			temp = *top;
			*top = *bot;
			*bot = temp;
			++top;
			++bot;
		}
	}

	return data;
}

//------------------------------- PRIVATE -------------------------------
void Render::InitVAO() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
}

void Render::InitPuntos() {
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Render::InitTextura() {
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Cargamos la textura
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	this->data = loadImage(this->urlImg);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->data.width, this->data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->data.image.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}