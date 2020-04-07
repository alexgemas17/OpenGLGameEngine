#include "AssimpLoader.h"

#include "../Application.h"

#include "../Render/NodoScene.h"
#include "../Render/SceneObj.h"


NodoScene* AssimpLoader::loadModelAssimpNode(ObjFile &modelData)
{
	// Leemmos los datos del archivo mediante el importer de assimp
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(modelData.obj, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return nullptr;
	}

	std::cout << "ASSIMP:  " + modelData.obj + "  LOADED SUCCESSFULLY" << std::endl;
	NodoScene* root = new NodoScene();
	loadRecursivo(scene->mRootNode, scene, root, modelData);

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
	//loadRecursivo(scene->mRootNode, scene, root, modelData);

	return root->getNode(0)->getObj(0);
}

//Recorre los distintos nodos que contiene la escena. O lo que es lo mismo, nuestro NodeScene
void AssimpLoader::loadRecursivo(aiNode* node, const aiScene* scene, NodoScene* nodo, ObjFile& modelData)
{

	// Nos recorremos la info de este nodo y la procesamos.
	// Añadimos el objeto a nuestro NodoScene
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		nodo->addObj(processMeshAssimp(mesh, scene, modelData));
	}

	// Una vez que hemos procesado el nodo actual, vamos hacia sus hijos
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		NodoScene* nodoHijo = new NodoScene;
		this->loadRecursivo(node->mChildren[i], scene, nodoHijo, modelData);
		nodo->addNodo(nodoHijo);
	}
}

// Procesa la mesh, lo que es lo mismo, nuestro SceneObj (puntos, normales, indices, etc...)
SceneObj* AssimpLoader::processMeshAssimp(aiMesh* mesh, const aiScene* scene, ObjFile& modelData)
{
	AssimpData* data = new AssimpData;
	SceneObj* obj;

	// Walk through each of the mesh's vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		glm::vec3 vector;

		// Añadimos la posición
		if (mesh->mVertices) {
			data->vertices.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
		}

		// Añadimos sus normales
		if (mesh->mNormals) {
			data->normales.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
		}

		// Añadimos sus tangentes
		if (mesh->mTangents) {
			data->tangentes.push_back(glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z));
		}

		// Añadimos sus bitangentes
		if (mesh->mBitangents) {
			data->bitangentes.push_back(glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z));
		}
		
		// Añadimos sus coordenadas de textura
		if (mesh->mTextureCoords[0])
		{
			data->coord_textura.push_back(glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
		}
		else
		{
			data->coord_textura.push_back(glm::vec2(0.0f, 0.0f));
		}
	}

	// Añadimos sus indices
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; j++)
		{
			if (face.mIndices) {
				data->indices.push_back(face.mIndices[j]);
			}
		}
		//Añadimos el primer índice para realizar bien el mapeo de las IBOs.
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
	std::vector<std::string> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, modelData.textureURL);

	// 2. specular maps
	std::vector<std::string> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, modelData.textureURL);

	// 3. normal maps
	std::vector<std::string> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, modelData.textureURL);

	// 4. height maps
	//std::vector<std::string> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "TEX_HEIGHT", texturasPath);

	addTextureToTextureManager(modelData.metallic_texture);
	addTextureToTextureManager(modelData.roughness_texture);
	addTextureToTextureManager(modelData.ao_texture);

	obj = new SceneObj(data, diffuseMaps, specularMaps, normalMaps, modelData.metallic_texture, modelData.roughness_texture, modelData.ao_texture);

	return obj;
}

std::vector<std::string> AssimpLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string path)
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
		std::string ID_Texture = filename.substr(filename.find_last_of('\\') + 1, filename.size());

		//Si es igual, es porque usa el sistema de asdf/asdf.asdf.
		if (ID_Texture == filename) {
			ID_Texture = filename.substr(filename.find_last_of('/') + 1, filename.size());
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

void AssimpLoader::addTextureToTextureManager(std::string texture)
{
	Application::getInstance()->getTextureManager()->addIDTexture(texture);
}