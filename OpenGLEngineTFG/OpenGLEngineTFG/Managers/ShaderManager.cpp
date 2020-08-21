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
	forwardLightingShader(new PagShaderProgram()),
	gBufferShader(new PagShaderProgram()),
	deferredLightingShader(new PagShaderProgram()),
	depthShader(new PagShaderProgram()),
	lightingCulling(new PagShaderProgram()),
	forwardPlusLightingShader(new PagShaderProgram()),
	shadowMap(new PagShaderProgram()),
	skybox(new PagShaderProgram()),
	ssao(new PagShaderProgram()),
	ssao_blur(new PagShaderProgram()),
	ShaderPath(Application::getInstance()->getPath().append("Data\\Shaders\\")),
	typeShader(TYPE_TEXTURE)
{
	//std::string urlBasicShader = ShaderPath + "Lights\\Light";

	std::string urlForwardLightingShader = ShaderPath + "Forward_Rendering\\forward_shader";

	std::string urlGBufferShader = ShaderPath + "Deferred_Rendering\\gBuffer_Pass";
	std::string urlDeferredLightingShader = ShaderPath + "Deferred_Rendering\\DeferredLighting_Pass";

	std::string urlDepthShader = ShaderPath + "Forward_Plus_Rendering\\depth";
	std::string urlLightingCulling = ShaderPath + "Forward_Plus_Rendering\\light_culling";
	std::string urlForwardPlusLightingShader = ShaderPath + "Forward_Plus_Rendering\\light_accumulation";

	std::string urlshadowMap = ShaderPath+ "Shadow\\shadpwmap";

	std::string urlskybox = ShaderPath+ "Skybox\\skybox";

	std::string urlssao = ShaderPath+ "Postprocessefects\\SSAO\\ssao";
	std::string urlssao_blur = ShaderPath+ "Postprocessefects\\SSAO\\ssao_blur";

	//this->basicLightShader->createShaderProgram(urlBasicShader.c_str() ); //CAMBIAR!

	this->forwardLightingShader->createShaderProgram(urlForwardLightingShader.c_str() );

	this->gBufferShader->createShaderProgram(urlGBufferShader.c_str() );
	this->deferredLightingShader->createShaderProgram(urlDeferredLightingShader.c_str() );

	this->depthShader->createShaderProgram(urlDepthShader.c_str() );
	this->lightingCulling->createShaderCompProgram(urlLightingCulling.c_str() );
	this->forwardPlusLightingShader->createShaderProgram(urlForwardPlusLightingShader.c_str() );

	this->shadowMap->createShaderProgram(urlshadowMap.c_str());
	this->skybox->createShaderProgram(urlskybox.c_str());

	this->ssao->createShaderProgram(urlssao.c_str());
	this->ssao_blur->createShaderProgram(urlssao_blur.c_str());
}

ShaderManager::~ShaderManager()
{
	delete forwardLightingShader;
	delete gBufferShader;
	delete deferredLightingShader;
	delete forwardPlusLightingShader;
	delete shadowMap;
	delete skybox;
	delete ssao;
	delete ssao_blur;
	delete instance;
}

void ShaderManager::setTypeRender()
{
	(typeShader == TYPE_TEXTURE) ? typeShader == TYPE_WIREFRAME : typeShader == TYPE_TEXTURE;
}

/* -------------- RENDERS --------------  */
PagShaderProgram* ShaderManager::getForwardLighting()
{
	return this->forwardLightingShader;
}

PagShaderProgram* ShaderManager::getGBuffer()
{
	return this->gBufferShader;
}

PagShaderProgram* ShaderManager::getDeferredShading()
{
	return this->deferredLightingShader;
}

PagShaderProgram* ShaderManager::getDepthShader()
{
	return this->depthShader;
}

PagShaderProgram* ShaderManager::getLightingCulling()
{
	return this->lightingCulling;
}

PagShaderProgram* ShaderManager::getForwardPlusLighting()
{
	return this->forwardPlusLightingShader;
}

PagShaderProgram* ShaderManager::getShadowMap()
{
	return this->shadowMap;
}

PagShaderProgram* ShaderManager::getSkyBox()
{
	return this->skybox;
}

PagShaderProgram* ShaderManager::getSSAO()
{
	return this->ssao;
}

PagShaderProgram* ShaderManager::getSSAOBlur()
{
	return this->ssao_blur;
}
