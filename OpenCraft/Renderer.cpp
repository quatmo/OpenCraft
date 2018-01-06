#include <unordered_map>
#include <functional>
#include <iostream>
#include <memory>

#include "ext/glm/gtc/matrix_transform.hpp"

#include "TextureManager.h"
#include "Renderer.h"

using namespace std;


// 1~1024 for typical blocks

unordered_map < CubeType,
	function<void(const Shader, const unsigned int, const int,const int)>
>
renderDispatcher =
{
	{1,renderGrass},
	{2,renderDirt},
	{3,renderRedStoneBlock},
	{4,renderOak},
	{5,renderLeaf},
	{1026,renderFlower}
};


Renderer::Renderer()
{
}



void Renderer::renderCubes(CubeType cubeType, const Shader shader, glm::mat4* models, const int instanceCount)
{
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	unsigned int instanceVBO;  // set instanceVBO
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, instanceCount * sizeof(glm::mat4), &models[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	if (cubeType / 1000000 == 0)
	{
		renderDispatcher[cubeType](shader, VAO, instanceCount, cubeType/1000000); // render it
	}
	else
	{
		renderDispatcher[cubeType%1000000](shader, VAO, instanceCount, cubeType / 1000000); // render it
	}

	// never forget to release the resource
	glDeleteBuffers(1, &instanceVBO);

}

Renderer::~Renderer()
{
}

float cubeVertices[6 * 6 * 6] = {
	// positions          // normals          
	-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
	1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
	1.0f, 1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
	1.0f, 1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,

	1.0f, 1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	-1.0f,  -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	-1.0f,  -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	1.0f, 1.0f,  1.0f,  0.0f,  0.0f,  1.0f,

	-1.0f,  -1.0f,-1.0f, -1.0f,  0.0f,  0.0f,
	-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
	-1.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
	-1.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
	-1.0f, -1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
	-1.0f,  -1.0f, - 1.0f, -1.0f,  0.0f,  0.0f,

	1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
	1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
	1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
	1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
	1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
	1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,

	1.0f, -1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
	1.0f, -1.0f,  -1.0f,  0.0f, -1.0f,  0.0f,
	-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
	-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
	-1.0f, -1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
	1.0f, -1.0f, 1.0f,  0.0f, -1.0f,  0.0f,

	-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f
};

void renderBasicBlock(const Shader shader, const unsigned int VAO, const int instanceCount, const std::string name, const int durability)
{
	shader.use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glBindVertexArray(VAO);
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	shader.setInt("texture", 0);
	shader.setInt("durabilityTexture", 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureManager::getCubeTexture(name));

	if (durability == 0)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, TextureManager::getCubeTexture("destroy_stage_0"));
		glDrawArraysInstanced(GL_TRIANGLES, 0, 36, instanceCount);
	}
	else
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, TextureManager::getCubeTexture("destroy_stage_"+std::to_string(durability)));
		glDrawArraysInstanced(GL_TRIANGLES, 0, 36, instanceCount);
	}
	
	glDisable(GL_CULL_FACE);
	glBindVertexArray(0);
	// never forget to release the resource
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}


void renderGrass(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	renderBasicBlock(shader, VAO, instanceCount, "grass", durability);
}

void renderDirt(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	renderBasicBlock(shader, VAO, instanceCount, "dirt",durability);
}

void renderRedStoneBlock(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	renderBasicBlock(shader, VAO, instanceCount, "red_stone_block", durability);
}

void renderOak(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	renderBasicBlock(shader, VAO, instanceCount, "oak", durability);
}

void renderLeaf(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	renderBasicBlock(shader, VAO, instanceCount, "leaf", durability);
}


float flowerVertices[6*8] = {
	// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
	0.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,   1.0f,  1.0f,
	0.0f, -1.0f, -1.0f,  0.0f,  0.0f,  1.0f,   0.0f,  1.0f,
	0.0f, 1.0f,  -1.0f,  0.0f,  1.0f,   1.0f,  0.0f,   0.0f,

	0.0f,-1.0f,  1.0f,  0.0f,  0.0f,  0.0f,   1.0f,  1.0f,
	0.0f, 1.0f, -1.0f,  0.0f,  1.0f,  1.0f,   0.0f,  0.0f,
	0.0f, 1.0f,  1.0f,  0.0f,  1.0f,  0.0f,   1.0f,  0.0f
};

void renderFlower(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	//std::cerr << "render flower" << std::endl;
	shader.use();
	//glEnable(GL_DEPTH_TEST);
	glBindVertexArray(VAO);
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(flowerVertices), &flowerVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(1 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureManager::getFaceTexture("flower"));
	glDrawArraysInstanced(GL_TRIANGLES, 0,6, instanceCount);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instanceCount);

	glBindVertexArray(0);
	// never forget to release the resource
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}


