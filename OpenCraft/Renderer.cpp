#include <unordered_map>
#include <functional>
#include <iostream>
#include <memory>

#include "ext/glm/gtc/matrix_transform.hpp"

#include "TextureManager.h"
#include "Renderer.h"

using namespace std;

unordered_map < CubeType,
	function<void(const Shader, const unsigned int, const size_t)>
>
renderDispatcher =
{
	{1,renderGrass},
	{2,renderDirt}
};


Renderer::Renderer()
{
}



void Renderer::renderCubes(CubeType cubeType, const Shader shader, glm::mat4* models, const size_t instanceCount)
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
	renderDispatcher[cubeType](shader, VAO, instanceCount); // render it

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

void renderGrass(const Shader shader, const unsigned int VAO, const size_t instanceCount)
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

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureManager::getCubeTexture("grass"));
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, instanceCount);
	glDisable(GL_CULL_FACE);
	glBindVertexArray(0);
	// never forget to release the resource
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void renderDirt(const Shader shader, const unsigned int VAO, const size_t instanceCount)
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

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureManager::getCubeTexture("dirt"));
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, instanceCount);
	glDisable(GL_CULL_FACE);
	glBindVertexArray(0);
	// never forget to release the resource
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}
