#include "TextureLoader.h"
#include "TextureManager.h"

std::unordered_map<std::string, unsigned int> TextureManager::m_cubeTextureMap;
std::unordered_map<std::string, unsigned int> TextureManager::m_faceTextureMap;
std::unordered_map<std::string, unsigned int> TextureManager::m_normTextureMap;

unsigned int TextureManager::getCubeTexture(const std::string textureName)
{
	if (m_cubeTextureMap.find(textureName) == m_cubeTextureMap.end()) // not in the map
	{
		loadCubeTexture(textureName); // load it 
	}
	return m_cubeTextureMap.at(textureName);
}

unsigned int TextureManager::getFaceTexture(const std::string textureName)
{
	if (m_faceTextureMap.find(textureName) == m_faceTextureMap.end()) // not in the map
	{
		loadFaceTexture(textureName); // load it 
	}
	return m_faceTextureMap.at(textureName);
}

unsigned int TextureManager::getNormTexture(const std::string textureName)
{
	if (m_normTextureMap.find(textureName) == m_normTextureMap.end()) // not in the map
	{
		loadNormTexture(textureName); // load it 
	}
	return m_normTextureMap.at(textureName);
}

void TextureManager::loadCubeTexture(const std::string textureName)
{
	std::vector<std::string> faces
	{
		"./textures/",
		"./textures/",
		"./textures/",
		"./textures/",
		"./textures/",
		"./textures/"
	};
	// assemble the path
	faces[0] += (textureName + "/right.png");
	faces[1] += (textureName + "/left.png");
	faces[2] += (textureName + "/top.png");
	faces[3] += (textureName + "/bottom.png");
	faces[4] += (textureName + "/back.png");
	faces[5] += (textureName + "/front.png");
	m_cubeTextureMap[textureName] = loadCubemap(faces);
}

void TextureManager::loadFaceTexture(const std::string textureName)
{
	std::string filePath = "./textures/";
	filePath += textureName + "/face.png";
	m_faceTextureMap[textureName] = loadTexture(filePath.c_str());
}

void TextureManager::loadNormTexture(const std::string textureName)
{
	std::string filePath = "./textures/";
	filePath += textureName + "/norm.png";
	m_normTextureMap[textureName] = loadTexture(filePath.c_str());
}
