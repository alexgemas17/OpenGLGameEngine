#include "AssimpLoader.h"

NodoScene* AssimpLoader::loadModelAssimp(std::string modelURL)
{
	// Leemmos los datos del archivo mediante el importer de assimp
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(modelURL, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return nullptr;
	}

	std::cout << "ASSIMP::Loaded successfully" << std::endl;
	NodoScene* root = new NodoScene();
	loadRecursivo(scene->mRootNode, scene, root);

	return root;
}

//Recorre los distintos nodos que contiene la escena. O lo que es lo mismo, nuestro NodeScene
void AssimpLoader::loadRecursivo(aiNode* node, const aiScene* scene, NodoScene* nodo)
{

	// Nos recorremos la info de este nodo y la procesamos.
	// Añadimos el objeto a nuestro NodoScene
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		nodo->addObj(processMeshAssimp(mesh, scene));
	}

	// Una vez que hemos procesado el nodo actual, vamos hacia sus hijos
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		NodoScene* nodoHijo = new NodoScene;
		this->loadRecursivo(node->mChildren[i], scene, nodoHijo);
		nodo->addNodo(nodoHijo);
	}
}

// Procesa la mesh, lo que es lo mismo, nuestro SceneObj (puntos, normales, indices, etc...)
SceneObj* AssimpLoader::processMeshAssimp(aiMesh* mesh, const aiScene* scene)
{
	AssimpData data;
	SceneObj* obj;

	// Walk through each of the mesh's vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		glm::vec3 vector;

		// Añadimos la posición
		if (mesh->mVertices) {
			data.vertices.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
		}

		// Añadimos sus normale
		if (mesh->mNormals) {
			data.normales.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
		}
		
		// Añadimos sus coordenadas de textura
		if (mesh->mTextureCoords[0])
		{
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			data.coord_textura.push_back(glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
			data.coord_textura_3.push_back(glm::vec3(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y, mesh->mTextureCoords[0][i].z));
		}
		else
		{
			data.coord_textura.push_back(glm::vec2(0.0f, 0.0f));
		}
	}

	// Añadimos sus indices
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; j++)
		{
			if (face.mIndices) {
				data.indices.push_back(face.mIndices[j]);
			}
		}
		data.indices.push_back(0xFFFFFFFF);
	}

	// Procesamos su material
	//if (mesh->mMaterialIndex >= 0)
	//{
	//	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	//	// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
	//	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
	//	// Same applies to other texture as the following list summarizes:
	//	// Diffuse: texture_diffuseN
	//	// Specular: texture_specularN
	//	// Normal: texture_normalN

	//	// 1. Diffuse maps
	//	vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	//	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	//	// 2. Specular maps
	//	vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	//	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	//}

	obj = new SceneObj(data.vertices, data.indices, data.normales, data.coord_textura, "C:\\Users\\Alex\\Desktop\\Japanese_Temple_Model\\Textures\\Japanese_Temple_Paint2_Japanese_Shrine_Mat_AlbedoTransparency.png");

	return obj;
}