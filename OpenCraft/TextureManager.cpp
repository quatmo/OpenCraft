#include "TextureLoader.h"
#include "TextureManager.h"

std::unordered_map<std::string, unsigned int> TextureManager::m_cubeTextureMap;

unsigned int TextureManager::getCubeTexture(const std::string textureName)
{
	if (m_cubeTextureMap.find(textureName) == m_cubeTextureMap.end()) // not in the map
	{
		loadCubeTexture(textureName); // load it 
	}
	return m_cubeTextureMap.at(textureName);
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
	faces[0] += (textureName + "/right.jpg");
	faces[1] += (textureName + "/left.jpg");
	faces[2] += (textureName + "/top.jpg");
	faces[3] += (textureName + "/bottom.jpg");
	faces[4] += (textureName + "/back.jpg");
	faces[5] += (textureName + "/front.jpg");
	m_cubeTextureMap[textureName] = loadCubemap(faces);
}
