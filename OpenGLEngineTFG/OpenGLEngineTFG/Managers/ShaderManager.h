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

	PagShaderProgram* getShaderType();

	static ShaderManager* getInstance();

private:
	// Instancia singleton.
	static ShaderManager* instance;

	PagShaderProgram* basicShader;
	PagShaderProgram* textureShader;

	TypeShader typeShader;
};

