#include "ShaderManager.h"

ShaderManager* ShaderManager::instance = nullptr;

// Acceder al singleton.
ShaderManager* ShaderManager::getInstance()
{
	if (!instance) {
		instance = new ShaderManager();
	}
	return instance;
}

ShaderManager::ShaderManager(): 
	basicShader(new PagShaderProgram()), textureShader(new PagShaderProgram()),
	basicLightShader(new PagShaderProgram()), typeShader(TYPE_TEXTURE)
{
	this->basicShader->createShaderProgram("Shaders/BasicShader");
	this->textureShader->createShaderProgram("Shaders/BasicShaderTexture");
	this->basicLightShader->createShaderProgram("Shaders/Lights/Light"); //CAMBIAR!
}

ShaderManager::~ShaderManager()
{
	delete basicShader;
	delete textureShader;
	delete instance;
}

PagShaderProgram* ShaderManager::getBasicShader()
{
	return this->basicShader;
}

PagShaderProgram* ShaderManager::getWireframeShader()
{
	return this->basicShader;
}

PagShaderProgram* ShaderManager::getTextureShader()
{
	return this->textureShader;
}


PagShaderProgram* ShaderManager::getBasicLightShader()
{
	return this->basicLightShader;
}

PagShaderProgram* ShaderManager::getShaderType()
{
	if (typeShader == TYPE_TEXTURE) return this->textureShader;
	return this->basicShader;
}

void ShaderManager::setTypeRender()
{
	(typeShader == TYPE_TEXTURE) ? typeShader == TYPE_WIREFRAME : typeShader == TYPE_TEXTURE;
}
