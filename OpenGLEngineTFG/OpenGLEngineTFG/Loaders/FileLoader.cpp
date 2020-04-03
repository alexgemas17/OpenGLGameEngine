#include "FileLoader.h"

#include <iostream>
#include <fstream>

std::vector<ObjFile> FileLoader::getMainScene()
{
	return this->mainScene;
}

void FileLoader::readFromFile(std::string urlFile)
{
	// Read from the text file
	std::ifstream MyReadFile(urlFile);

	std::string textFromFile;
	// Use a while loop together with the getline() function to read the file line by line
	while (std::getline(MyReadFile, textFromFile)) {
		ObjFile obj;

		// Procesamos la posición
		obj.position = getVectorFromStr(textFromFile);
		std::getline(MyReadFile, textFromFile);

		//Procesamos su escala
		obj.scale = getVectorFromStr(textFromFile);
		std::getline(MyReadFile, textFromFile);

		//Procesamos su rotacion
		obj.angleRotation = strtof((textFromFile).c_str(), 0);
		std::getline(MyReadFile, textFromFile);
		
		obj.rotationDirection = getVectorFromStr(textFromFile);

		//Procesamos el obj y texturas
		std::getline(MyReadFile, textFromFile);
		obj.obj = textFromFile;

		std::getline(MyReadFile, textFromFile);
		obj.textureURL = textFromFile;

		std::getline(MyReadFile, textFromFile);
		obj.metallic_texture = textFromFile;

		std::getline(MyReadFile, textFromFile);
		obj.roughness_texture = textFromFile;

		std::getline(MyReadFile, textFromFile);
		obj.ao_texture = textFromFile;

		this->mainScene.push_back(obj);
		std::getline(MyReadFile, textFromFile);
	}

	// Close the file
	MyReadFile.close();
}

glm::vec3 FileLoader::getVectorFromStr(std::string str)
{
	int index = str.find(",");
	float x = strtof((str.substr(0, index)).c_str(), 0);
	str.erase(0, index + 1);

	index = str.find(",");
	float y = strtof((str.substr(0, index)).c_str(), 0);
	str.erase(0, index + 1);

	index = str.find(";");
	float z = strtof((str.substr(0, index)).c_str(), 0);

	return glm::vec3(x, y, z);
}