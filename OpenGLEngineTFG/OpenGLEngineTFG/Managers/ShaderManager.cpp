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
	skybox(new PagShaderProgram()),
	ssao(new PagShaderProgram()),
	ssao_blur(new PagShaderProgram()),
	godRays(new PagShaderProgram()),
	ShaderPath(Application::getInstance()->getPath().append("Data\\Shaders\\")),
	typeShader(TYPE_TEXTURE)
{
	//std::string urlBasicShader = ShaderPath + "Lights\\Light";

	std::string urlgBufferPass = ShaderPath + "Deferred_Rendering\\gBuffer_Pass";
	std::string urldeferredLightingPass = ShaderPath + "Deferred_Rendering\\DeferredLighting_Pass";

	std::string urlshadowMap = ShaderPath+ "Shadow\\shadpwmap";

	std::string urlskybox = ShaderPath+ "Skybox\\skybox";

	std::string urlssao = ShaderPath+ "Postprocessefects\\SSAO\\ssao";
	std::string urlssao_blur = ShaderPath+ "Postprocessefects\\SSAO\\ssao_blur";

	std::string urlCopy = ShaderPath + "Postprocessefects\\Copy";
	std::string urlGodRays = ShaderPath + "Postprocessefects\\God_rays\\God_rays";

	//this->basicLightShader->createShaderProgram(urlBasicShader.c_str() ); //CAMBIAR!

	this->gBufferPass->createShaderProgram(urlgBufferPass.c_str() );
	this->deferredLightingPass->createShaderProgram(urldeferredLightingPass.c_str() );

	this->shadowMap->createShaderProgram(urlshadowMap.c_str());
	this->skybox->createShaderProgram(urlskybox.c_str());

	this->ssao->createShaderProgram(urlssao.c_str());
	this->ssao_blur->createShaderProgram(urlssao_blur.c_str());

	this->copyDataPass->createShaderProgram(urlCopy.c_str());
	this->godRays->createShaderProgram(urlGodRays.c_str());
}

ShaderManager::~ShaderManager()
{
	delete basicLightShader;
	delete gBufferPass;
	delete shadowMap;
	delete skybox;
	delete ssao;
	delete ssao_blur;
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

PagShaderProgram* ShaderManager::getGodRays()
{
	return this->godRays;
}

PagShaderProgram* ShaderManager::getSSAO()
{
	return this->ssao;
}

PagShaderProgram* ShaderManager::getSSAOBlur()
{
	return this->ssao_blur;
}

PagShaderProgram* ShaderManager::getDeferredShading()
{
	return this->deferredLightingPass;
}

PagShaderProgram* ShaderManager::getBasicLightShader()
{
	return this->basicLightShader;
}

PagShaderProgram* ShaderManager::getSkyBox()
{
	return this->skybox;
}

void ShaderManager::setTypeRender()
{
	(typeShader == TYPE_TEXTURE) ? typeShader == TYPE_WIREFRAME : typeShader == TYPE_TEXTURE;
}
