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
	NodoScene* loadModelAssimpNode(std::string modelURL, std::string albedoURL, std::string normalURL, std::string materialURL);

	// Devuelve el nodo root del modelo
	SceneObj* loadModelAssimpObj(std::string modelURL, std::string albedoURL, std::string normalURL, std::string materialURL);

private:
	/* Recorremos recursivamente los nodos que compone la escena (el modelo) */
	void loadRecursivo(aiNode* node, const aiScene* scene, NodoScene* nodo, std::string albedoURL, std::string normalURL, std::string materialURL);

	/* Nos devuelve el AssimpData que contiene esa mesh */
	SceneObj* processMeshAssimp(aiMesh* mesh, const aiScene* scene, std::string albedoURL, std::string normalURL, std::string materialURL);

	/* Nos devuelve las textuas asociadas al objeto*/
	std::vector<std::string> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::string path);
};