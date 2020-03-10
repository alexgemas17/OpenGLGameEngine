#pragma once

#include "../PagShaderProgram.h"

enum TypeShader { TYPE_POINTS, TYPE_WIREFRAME, TYPE_TEXTURE };

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	void setTypeRender();

	PagShaderProgram* getBasicShader();
	PagShaderProgram* getWireframeShader();
	PagShaderProgram* getTextureShader();
	PagShaderProgram* getBasicLightShader();

	/* DEFERRED RENDERING */
	PagShaderProgram* getGBuffer();
	PagShaderProgram* getDeferredShading();

	PagShaderProgram* getShaderType();

	static ShaderManager* getInstance();

private:
	// Instancia singleton.
	static ShaderManager* instance;

	PagShaderProgram* basicShader;
	PagShaderProgram* textureShader;
	PagShaderProgram* basicLightShader;

	/* DEFERRED RENDERING */
	PagShaderProgram* gBuffer;
	PagShaderProgram* deferredShading;

	TypeShader typeShader;
};

