#include "AssimpLoader.h"

NodoScene* AssimpLoader::loadModelAssimpNode(std::string modelURL, std::string albedoURL, std::string normalURL, std::string materialURL)
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
	loadRecursivo(scene->mRootNode, scene, root, albedoURL, normalURL, materialURL);

	return root;
}

SceneObj* AssimpLoader::loadModelAssimpObj(std::string modelURL, std::string albedoURL, std::string normalURL, std::string materialURL)
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
	loadRecursivo(scene->mRootNode, scene, root, albedoURL, normalURL, materialURL);

	return root->getNode(0)->getObj(0);
}

//Recorre los distintos nodos que contiene la escena. O lo que es lo mismo, nuestro NodeScene
void AssimpLoader::loadRecursivo(aiNode* node, const aiScene* scene, NodoScene* nodo, std::string albedoURL, std::string normalURL, std::string materialURL)
{

	// Nos recorremos la info de este nodo y la procesamos.
	// A�adimos el objeto a nuestro NodoScene
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		nodo->addObj(processMeshAssimp(mesh, scene, albedoURL, normalURL, materialURL));
	}

	// Una vez que hemos procesado el nodo actual, vamos hacia sus hijos
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		NodoScene* nodoHijo = new NodoScene;
		this->loadRecursivo(node->mChildren[i], scene, nodoHijo, albedoURL, normalURL, materialURL);
		nodo->addNodo(nodoHijo);
	}
}

// Procesa la mesh, lo que es lo mismo, nuestro SceneObj (puntos, normales, indices, etc...)
SceneObj* AssimpLoader::processMeshAssimp(aiMesh* mesh, const aiScene* scene, std::string albedoURL, std::string normalURL, std::string materialURL)
{
	AssimpData data;
	SceneObj* obj;

	// Walk through each of the mesh's vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		glm::vec3 vector;

		// A�adimos la posici�n
		if (mesh->mVertices) {
			data.vertices.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
		}

		// A�adimos sus normale
		if (mesh->mNormals) {
			data.normales.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
		}
		
		// A�adimos sus coordenadas de textura
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

	// A�adimos sus indices
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

	//Procesamos su material --> En principio se lo pasamos a pelo desde el inicio de la aplicaci�n
	//if (mesh->mMaterialIndex >= 0)
	//{
	//	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	//	// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
	//	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
	//	// Same applies to other texture as the following list summarizes:
	//	// Diffuse: texture_diffuseN
	//	// Specular: texture_specularN
	//	// Normal: texture_normalN

	//	std::vector<std::string> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	//	//textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	//	std::vector<std::string> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	//	//textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	//}

	obj = new SceneObj(data.vertices, data.indices, data.normales, data.coord_textura, albedoURL, normalURL, materialURL);

	return obj;
}

std::vector<std::string> AssimpLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<std::string> texturesURL;

	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		texturesURL.push_back(str.C_Str());
	}

	return texturesURL;
}