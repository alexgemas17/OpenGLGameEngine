#pragma once

#include "../PagShaderProgram.h"

enum TypeShader { TYPE_POINTS, TYPE_WIREFRAME, TYPE_TEXTURE };

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	void setTypeRender();

	PagShaderProgram* getBasicLightShader();

	/* DEFERRED RENDERING */
	PagShaderProgram* getGBuffer();
	PagShaderProgram* getDeferredShading();
	PagShaderProgram* getShadowMap();
	PagShaderProgram* getSkyBox();
	PagShaderProgram* getSSAO();
	PagShaderProgram* getSSAOBlur();
	PagShaderProgram* getCopyDataPass();

	static ShaderManager* getInstance();

private:
	// Instancia singleton.
	static ShaderManager* instance;

	std::string ShaderPath;

	PagShaderProgram* basicLightShader;

	/* DEFERRED RENDERING */
	PagShaderProgram* gBufferPass;
	PagShaderProgram* deferredLightingPass;

	/* SHADOW MAP */
	PagShaderProgram* shadowMap;

	/* SkyBox */
	PagShaderProgram* skybox;

	/* SSAO */
	PagShaderProgram* ssao;
	PagShaderProgram* ssao_blur;

	/* POSTPROCESS EFFECTS*/
	PagShaderProgram* copyDataPass;

	TypeShader typeShader;
};