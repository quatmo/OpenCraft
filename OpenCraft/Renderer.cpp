#include <unordered_map>
#include <functional>
#include <iostream>
#include <memory>

#include <ext/glm/gtc/matrix_transform.hpp>

#include "TextureManager.h"
#include "Renderer.h"
#include "Model.h"

using namespace std;

std::unordered_map<std::string, Model> modelMap;

// 1~1024 for typical blocks
// 1025~999999 for transparent blocks

unordered_map < CubeType,
	function<void(const Shader, const unsigned int, const int, const int)>
>
renderDispatcher =
{
	{1,renderGrass},
	{2,renderDirt},
	{3,renderRedStoneBlock},
	{4,renderOak},
	{5,renderLeaf},
	{6,renderGlass},
	{7,renderBrick},
	{8,renderCraftTable},
	{9,renderFurnace},
	{10,renderQuartz},
	{11,renderMossyStone},
	{12,renderDiamond},
	{13,renderGlowStone},
	{14,renderPlanks},
	{15,renderWool},
	{16,render2D},
	{18,renderBlackWool},
	{19,renderWhiteWool},
	{20,renderYellowWool},
	{21,renderRedWool},
	{1026,renderPaeonia},
	{1027,renderRose},
	{1028,renderWheat},
	{17,renderTorch }
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
		renderDispatcher[cubeType](shader, VAO, instanceCount, cubeType / 1000000); // render it
	}
	else
	{
		renderDispatcher[cubeType % 1000000](shader, VAO, instanceCount, cubeType / 1000000); // render it
	}

	// never forget to release the resource
	glDeleteBuffers(1, &instanceVBO);
	glBindVertexArray(0);

}

void Renderer::renderModel(Shader shader, glm::mat4 * models, const int instanceCount)
{
	if (modelMap.find("chest") == modelMap.end())
	{
		modelMap.emplace("chest", Model("./models/chest/treasure_chest.obj"));
	}
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f));
	shader.use();
	for (int i = 0; i < instanceCount; i++)
	{
		shader.setMat4("model", models[i] * model);
		modelMap.at("chest").draw(shader);	// draw scene as normal
	}
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
	-1.0f,  -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,

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
	glBindVertexArray(VAO);
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	shader.setInt("texture1", 0);
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
		glBindTexture(GL_TEXTURE_CUBE_MAP, TextureManager::getCubeTexture("destroy_stage_" + std::to_string(durability)));
		glDrawArraysInstanced(GL_TRIANGLES, 0, 36, instanceCount);
	}

	glBindVertexArray(0);
	// never forget to release the resource
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}


void renderGrass(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	renderBasicBlock(shader, VAO, instanceCount, "grass", durability);
	glDisable(GL_CULL_FACE);
}

void renderDirt(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	renderBasicBlock(shader, VAO, instanceCount, "dirt", durability);
	glDisable(GL_CULL_FACE);
}

void renderRedStoneBlock(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	renderBasicBlock(shader, VAO, instanceCount, "red_stone_block", durability);
	glDisable(GL_CULL_FACE);
}

void renderOak(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	renderBasicBlock(shader, VAO, instanceCount, "oak", durability);
	glDisable(GL_CULL_FACE);
}

void renderLeaf(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	renderBasicBlock(shader, VAO, instanceCount, "leaf", durability);
	glDisable(GL_CULL_FACE);
}

float flowerVertices[6 * 8] = {
	// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
	0.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,   1.0f,  1.0f,
	0.0f, -1.0f, -1.0f,  0.0f,  0.0f,  1.0f,   0.0f,  1.0f,
	0.0f, 1.0f,  -1.0f,  0.0f,  1.0f,   1.0f,  0.0f,   0.0f,

	0.0f,-1.0f,  1.0f,  0.0f,  0.0f,  0.0f,   1.0f,  1.0f,
	0.0f, 1.0f, -1.0f,  0.0f,  1.0f,  1.0f,   0.0f,  0.0f,
	0.0f, 1.0f,  1.0f,  0.0f,  1.0f,  0.0f,   1.0f,  0.0f
};

void renderFlower(const Shader shader, const unsigned int VAO, const int instanceCount, const std::string name, const int durability)
{
	shader.use();
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
	glBindTexture(GL_TEXTURE_2D, TextureManager::getFaceTexture(name));
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instanceCount);

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




void renderPaeonia(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	renderFlower(shader, VAO, instanceCount, "paeonia", durability);
}

void renderRose(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	renderFlower(shader, VAO, instanceCount, "rose", durability);
}

void renderWheat(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	renderFlower(shader, VAO, instanceCount, "wheat", durability);
}

void renderMossyStone(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	renderBasicBlock(shader, VAO, instanceCount, "mossyStone", durability);
	glDisable(GL_CULL_FACE);
}

void renderDiamond(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	renderBasicBlock(shader, VAO, instanceCount, "diamond", durability);
	glDisable(GL_CULL_FACE);
}

void renderGlowStone(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	renderBasicBlock(shader, VAO, instanceCount, "glowStone", durability);
	glDisable(GL_CULL_FACE);
}

void renderPlanks(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	renderBasicBlock(shader, VAO, instanceCount, "planks", durability);
	glDisable(GL_CULL_FACE);
}

void renderWool(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	renderBasicBlock(shader, VAO, instanceCount, "wool", durability);
	glDisable(GL_CULL_FACE);
}

void renderBlackWool(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	renderBasicBlock(shader, VAO, instanceCount, "blackWool", durability);
	glDisable(GL_CULL_FACE);
}

void renderWhiteWool(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	renderBasicBlock(shader, VAO, instanceCount, "whiteWool", durability);
	glDisable(GL_CULL_FACE);
}

void renderYellowWool(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	renderBasicBlock(shader, VAO, instanceCount, "yellowWool", durability);
	glDisable(GL_CULL_FACE);
}

void renderRedWool(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	renderBasicBlock(shader, VAO, instanceCount, "redWool", durability);
	glDisable(GL_CULL_FACE);
}

void render2D(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	renderBasicBlock(shader, VAO, instanceCount, "2d", durability);
	glDisable(GL_CULL_FACE);
}


//float torchVertices[6 * 6 * 6] = {
//	// positions          // normals          
//	-0.25f, -1.0f, -0.25f,  0.0f,  0.0f, -1.0f,
//	0.25f, -1.0f, -0.25f,  0.0f,  0.0f, -1.0f,
//	0.25f, 1.0f, -0.25f,  0.0f,  0.0f, -1.0f,
//	0.25f, 1.0f, -0.25f,  0.0f,  0.0f, -1.0f,
//	-0.25f, 1.0f, -0.25f,  0.0f,  0.0f, -1.0f,
//	-0.25f, -1.0f, -0.25f,  0.0f,  0.0f, -1.0f,
//
//	0.25f, 1.0f,  0.25f,  0.0f,  0.0f,  1.0f,
//	0.25f, -1.0f,  0.25f,  0.0f,  0.0f,  1.0f,
//	-0.25f,  -1.0f,  0.25f,  0.0f,  0.0f,  1.0f,
//	-0.25f,  -1.0f,  0.25f,  0.0f,  0.0f,  1.0f,
//	-0.25f,  1.0f,  0.25f,  0.0f,  0.0f,  1.0f,
//	0.25f, 1.0f,  0.25f,  0.0f,  0.0f,  1.0f,
//
//	-0.25f,  -1.0f,-0.25f, -1.0f,  0.0f,  0.0f,
//	-0.25f,  1.0f, -0.25f, -1.0f,  0.0f,  0.0f,
//	-0.25f, 1.0f, 0.25f, -1.0f,  0.0f,  0.0f,
//	-0.25f, 1.0f, 0.25f, -1.0f,  0.0f,  0.0f,
//	-0.25f, -1.0f, 0.25f, -1.0f,  0.0f,  0.0f,
//	-0.25f,  -1.0f, -0.25f, -1.0f,  0.0f,  0.0f,
//
//	0.25f,  1.0f,  0.25f,  1.0f,  0.0f,  0.0f,
//	0.25f,  1.0f, -0.25f,  1.0f,  0.0f,  0.0f,
//	0.25f, -1.0f, -0.25f,  1.0f,  0.0f,  0.0f,
//	0.25f, -1.0f, -0.25f,  1.0f,  0.0f,  0.0f,
//	0.25f, -1.0f,  0.25f,  1.0f,  0.0f,  0.0f,
//	0.25f,  1.0f,  0.25f,  1.0f,  0.0f,  0.0f,
//
//	0.25f, -1.0f, 0.25f,  0.0f, -1.0f,  0.0f,
//	0.25f, -1.0f,  -0.25f,  0.0f, -1.0f,  0.0f,
//	-0.25f, -1.0f, -0.25f,  0.0f, -1.0f,  0.0f,
//	-0.25f, -1.0f, -0.25f,  0.0f, -1.0f,  0.0f,
//	-0.25f, -1.0f,0.25f,  0.0f, -1.0f,  0.0f,
//	0.25f, -1.0f, 0.25f,  0.0f, -1.0f,  0.0f,
//
//	-0.25f,  1.0f, -0.25f,  0.0f,  1.0f,  0.0f,
//	0.25f,  1.0f, -0.25f,  0.0f,  1.0f,  0.0f,
//	0.25f,  1.0f,  0.25f,  0.0f,  1.0f,  0.0f,
//	0.25f,  1.0f,  0.25f,  0.0f,  1.0f,  0.0f,
//	-0.25f,  1.0f,  0.25f,  0.0f,  1.0f,  0.0f,
//	-0.25f,  1.0f, -0.25f,  0.0f,  1.0f,  0.0f
//};

void renderTorch(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	renderBasicBlock(shader, VAO, instanceCount, "torch", durability);
	glDisable(GL_CULL_FACE);
}

void renderGlass(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	renderBasicBlock(shader, VAO, instanceCount, "glass", durability);
	glDisable(GL_BLEND);
}

void renderBrick(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	renderBasicBlock(shader, VAO, instanceCount, "brick", durability);
	glDisable(GL_CULL_FACE);
}

void renderCraftTable(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	renderBasicBlock(shader, VAO, instanceCount, "craft_table", durability);
}

void renderFurnace(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	renderBasicBlock(shader, VAO, instanceCount, "furnace", durability);
}

void renderQuartz(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability)
{
	renderBasicBlock(shader, VAO, instanceCount, "quartz", durability);
}


