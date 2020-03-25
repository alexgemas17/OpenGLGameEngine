#include "AssimpLoader.h"

#include "../Application.h"

#include "../Render/NodoScene.h"
#include "../Render/SceneObj.h"


NodoScene* AssimpLoader::loadModelAssimpNode(std::string modelURL, std::string texturasPath)
{
	// Leemmos los datos del archivo mediante el importer de assimp
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(modelURL, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return nullptr;
	}

	std::cout << "ASSIMP:  " + modelURL + "  LOADED SUCCESSFULLY" << std::endl;
	NodoScene* root = new NodoScene();
	loadRecursivo(scene->mRootNode, scene, root, texturasPath);

	return root;
}

SceneObj* AssimpLoader::loadModelAssimpObj(std::string modelURL, std::string texturasPath)
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
	loadRecursivo(scene->mRootNode, scene, root, texturasPath);

	return root->getNode(0)->getObj(0);
}

//Recorre los distintos nodos que contiene la escena. O lo que es lo mismo, nuestro NodeScene
void AssimpLoader::loadRecursivo(aiNode* node, const aiScene* scene, NodoScene* nodo, std::string texturasPath)
{

	// Nos recorremos la info de este nodo y la procesamos.
	// A�adimos el objeto a nuestro NodoScene
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		nodo->addObj(processMeshAssimp(mesh, scene, texturasPath));
	}

	// Una vez que hemos procesado el nodo actual, vamos hacia sus hijos
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		NodoScene* nodoHijo = new NodoScene;
		this->loadRecursivo(node->mChildren[i], scene, nodoHijo, texturasPath);
		nodo->addNodo(nodoHijo);
	}
}

// Procesa la mesh, lo que es lo mismo, nuestro SceneObj (puntos, normales, indices, etc...)
SceneObj* AssimpLoader::processMeshAssimp(aiMesh* mesh, const aiScene* scene, std::string texturasPath)
{
	AssimpData* data = new AssimpData;
	SceneObj* obj;

	// Walk through each of the mesh's vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		glm::vec3 vector;

		// A�adimos la posici�n
		if (mesh->mVertices) {
			data->vertices.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
		}

		// A�adimos sus normales
		if (mesh->mNormals) {
			data->normales.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
		}

		// A�adimos sus tangentes
		if (mesh->mTangents) {
			data->tangentes.push_back(glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z));
		}

		// A�adimos sus bitangentes
		if (mesh->mBitangents) {
			data->bitangentes.push_back(glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z));
		}
		
		// A�adimos sus coordenadas de textura
		if (mesh->mTextureCoords[0])
		{
			data->coord_textura.push_back(glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
		}
		else
		{
			data->coord_textura.push_back(glm::vec2(0.0f, 0.0f));
		}
	}

	// A�adimos sus indices
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; j++)
		{
			if (face.mIndices) {
				data->indices.push_back(face.mIndices[j]);
			}
		}
		//A�adimos el primer �ndice para realizar bien el mapeo de las IBOs.
		if (face.mIndices) {
			data->indices.push_back(face.mIndices[0]);
		}
		data->indices.push_back(0xFFFFFFFF);
	}

	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	std::vector<std::string> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", texturasPath);

	// 2. specular maps
	std::vector<std::string> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", texturasPath);

	// 3. normal maps
	std::vector<std::string> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "TEX_NORMAL", texturasPath);

	// 4. height maps
	//std::vector<std::string> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "TEX_HEIGHT", texturasPath);

	//obj = new SceneObj(data.vertices, data.indices, data.normales, data.coord_textura, albedoURL, normalURL, materialURL);
	//obj = new SceneObj(data.vertices, data.indices, data.normales, data.coord_textura, diffuseMaps, specularMaps, normalMaps);
	//Version buena??
	obj = new SceneObj(data, diffuseMaps, specularMaps, normalMaps);

	return obj;
}

std::vector<std::string> AssimpLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::string path)
{
	std::vector<std::string> texturesURL;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;

		//Extraemos el nombre del archivo.
		std::string filename = std::string(str.C_Str());
		std::string ID_Texture = filename.substr(filename.find_last_of('\\') + 1, filename.size() - 1);

		//Si es igual, es porque usa el sistema de asdf/asdf.asdf.
		if (ID_Texture == filename) {
			ID_Texture = filename.substr(filename.find_last_of('/') + 1, filename.size() - 1);
		}

		ID_Texture = path + ID_Texture;

		if (Application::getInstance()->getTextureManager()->getIDTexture(ID_Texture) != -1)
		{
			skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
			break;
		}

		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Application::getInstance()->getTextureManager()->addIDTexture(ID_Texture);
			texturesURL.push_back(ID_Texture);
		}
	}

	return texturesURL;
}