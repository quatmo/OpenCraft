#pragma once

#include <unordered_map>
#include <string>

class TextureManager
{
public:
	TextureManager() = delete;

	static unsigned int getCubeTexture(const std::string textureName);

	~TextureManager() = delete;
private:
	static std::unordered_map<std::string, unsigned int> m_cubeTextureMap;
	static void loadCubeTexture(const std::string textureName);
};

