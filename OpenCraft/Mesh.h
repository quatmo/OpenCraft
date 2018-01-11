#pragma once

#include "glad.h" 
#include "ext/assimp/Importer.hpp"
#include "ext/glm/glm.hpp"
#include "ext/glm/gtc/matrix_transform.hpp"
#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

struct Vertex 
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

struct Texture 
{
	unsigned int id;
	std::string type;
	aiString path;
};

class Mesh 
{
public:

	// Mesh Data  
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
	std::vector<Texture> m_textures;
	unsigned int m_VAO;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

	// render the mesh
	void Draw(Shader shader);
	unsigned int getVAO(void)const;

private:
	//Render data 
	unsigned int m_VBO, m_EBO;
	// initializes all the buffer objects/arrays
	void setupMesh();
};


