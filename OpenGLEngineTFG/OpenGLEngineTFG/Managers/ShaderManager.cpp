#include "ShaderManager.h"

#include "../Application.h"

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
	basicLightShader(new PagShaderProgram()), 
	gBufferPass(new PagShaderProgram()),
	deferredLightingPass(new PagShaderProgram()),
	shadowMap(new PagShaderProgram()),
	copyDataPass(new PagShaderProgram()),
	ShaderPath(Application::getInstance()->getPath().append("Data\\Shaders\\")),
	typeShader(TYPE_TEXTURE)
{
	std::string urlBasicShader = ShaderPath + "Lights\\Light";
	std::string urlgBufferPass = ShaderPath + "Lights\\Deferred_Rendering\\GBuffer_Pass";
	std::string urldeferredLightingPass = ShaderPath + "Lights\\Deferred_Rendering\\Deferred_Lighting_Pass";
	std::string urlshadowMap = ShaderPath+ "Shadow\\shadpwmap";
	std::string urlCopy = ShaderPath + "Postprocessefects\\Copy";

	this->basicLightShader->createShaderProgram(urlBasicShader.c_str() ); //CAMBIAR!

	this->gBufferPass->createShaderProgram(urlgBufferPass.c_str() );
	this->deferredLightingPass->createShaderProgram(urldeferredLightingPass.c_str() );

	this->shadowMap->createShaderProgram(urlshadowMap.c_str());

	this->copyDataPass->createShaderProgram(urlCopy.c_str());
}

ShaderManager::~ShaderManager()
{
	delete basicLightShader;
	delete gBufferPass;
	delete shadowMap;
	delete deferredLightingPass;
	delete copyDataPass;
	delete instance;
}

PagShaderProgram* ShaderManager::getGBuffer()
{
	return this->gBufferPass;
}

PagShaderProgram* ShaderManager::getShadowMap()
{
	return this->shadowMap;
}

PagShaderProgram* ShaderManager::getCopyDataPass()
{
	return this->copyDataPass;
}


PagShaderProgram* ShaderManager::getDeferredShading()
{
	return this->deferredLightingPass;
}

PagShaderProgram* ShaderManager::getBasicLightShader()
{
	return this->basicLightShader;
}

void ShaderManager::setTypeRender()
{
	(typeShader == TYPE_TEXTURE) ? typeShader == TYPE_WIREFRAME : typeShader == TYPE_TEXTURE;
}
