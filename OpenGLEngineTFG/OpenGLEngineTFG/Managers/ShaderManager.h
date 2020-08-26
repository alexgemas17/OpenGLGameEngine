#pragma once

#include "../PagShaderProgram.h"

enum TypeShader { TYPE_POINTS, TYPE_WIREFRAME, TYPE_TEXTURE };

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	void setTypeRender();

	static ShaderManager* getInstance();

	/* RENDERS */
	PagShaderProgram* getForwardLighting();
	PagShaderProgram* getGBuffer();
	PagShaderProgram* getDeferredShading();
	PagShaderProgram* getDepthShader();
	PagShaderProgram* getLightingCulling();
	PagShaderProgram* getForwardPlusLighting();

	/* POSTPROCESS EFFECTS */
	PagShaderProgram* getShadowMap();
	PagShaderProgram* getSkyBox();
	PagShaderProgram* getTransparentObj();
	PagShaderProgram* getSSAO();
	PagShaderProgram* getSSAOBlur();


private:
	// Instancia singleton.
	static ShaderManager* instance;
	TypeShader typeShader;
	std::string ShaderPath;

	/* FORWARD RENDER */
	PagShaderProgram* forwardLightingShader;

	/* DEFERRED RENDERING */
	PagShaderProgram* gBufferShader;
	PagShaderProgram* deferredLightingShader;

	/* FORWARD PLUS RENDER */
	PagShaderProgram* depthShader;
	PagShaderProgram* lightingCulling;
	PagShaderProgram* forwardPlusLightingShader;

	// ---------- POSTPROCESS EFFECTS -----------------
	/* SHADOW MAP */
	PagShaderProgram* shadowMap;

	/* SkyBox */
	PagShaderProgram* skybox;

	PagShaderProgram* transparentObj;

	/* SSAO */
	PagShaderProgram* ssao;
	PagShaderProgram* ssao_blur;
};