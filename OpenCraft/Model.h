#pragma once

#include "glad.h"
#include "ext/glm/glm.hpp"
#include "ext/glm/gtc/matrix_transform.hpp"
#include "stb_image.h"
#include "ext/assimp/Importer.hpp"
#include "ext/assimp/scene.h"
#include "ext/assimp/postprocess.h"

#include "mesh.h"
#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model
{
public:
	// Model Data
	std::vector<Texture> m_texturesLoaded;	
	std::vector<Mesh> m_meshes;
	std::string m_directory;
	bool m_gammaCorrection;

	Model(std::string const &path, bool gamma = false);

	void draw(Shader shader);
	std::vector<Mesh>& getMesh();

private:
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(std::string const &path);

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};


