#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class NodoScene;
class SceneObj;

struct AssimpData {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normales;
	std::vector<glm::vec3> tangentes;
	std::vector<glm::vec3> bitangentes;
	std::vector<GLuint> indices;
	std::vector<glm::vec2> coord_textura;
};

class AssimpLoader
{
public:
	// Devuelve el nodo root del modelo
	NodoScene *loadModelAssimpNode(std::string modelURL, std::string texturasPath);

	// Devuelve el nodo root del modelo
	SceneObj *loadModelAssimpObj(std::string modelURL, std::string texturasPath);

private:
	/* Recorremos recursivamente los nodos que compone la escena (el modelo) */
	void loadRecursivo(aiNode* node, const aiScene* scene, NodoScene* nodo, std::string texturasPath);

	/* Nos devuelve el AssimpData que contiene esa mesh */
	SceneObj *processMeshAssimp(aiMesh* mesh, const aiScene* scene, std::string texturasPath);

	/* Nos devuelve las textuas asociadas al objeto*/
	std::vector<std::string> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::string path);
};