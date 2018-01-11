#pragma once

#include <unordered_map>
#include <string>

class TextureManager
{
public:
	TextureManager() = delete;

	static unsigned int getCubeTexture(const std::string textureName);
	static unsigned int getFaceTexture(const std::string textureName);
	static unsigned int getNormTexture(const std::string textureName);

	~TextureManager() = delete;
private:
	static std::unordered_map<std::string, unsigned int> m_cubeTextureMap;
	static std::unordered_map<std::string, unsigned int> m_faceTextureMap;
	static std::unordered_map<std::string, unsigned int> m_normTextureMap;
	static void loadCubeTexture(const std::string textureName);
	static void loadFaceTexture(const std::string textureName);
	static void loadNormTexture(const std::string textureName);
};

