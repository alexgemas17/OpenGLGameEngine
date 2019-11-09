#pragma once

#include <iostream>
#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Render/NodoScene.h"
#include "../Render/SceneObj.h"

struct AssimpData {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normales;
	std::vector<GLuint> indices;
	std::vector<glm::vec2> coord_textura;
	std::vector<glm::vec3> coord_textura_3;
};

class AssimpLoader
{
public:
	// Devuelve el nodo root del modelo
	NodoScene* loadModelAssimp(std::string modelURL);

private:
	/* Recorremos recursivamente los nodos que compone la escena (el modelo) */
	void loadRecursivo(aiNode* node, const aiScene* scene, NodoScene* nodo);

	/* Nos devuelve el AssimpData que contiene esa mesh */
	SceneObj* processMeshAssimp(aiMesh* mesh, const aiScene* scene);
};