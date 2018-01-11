#pragma once

#include <vector>
#include <string>

#include "ext/glm/glm.hpp"
#include "ext/glm/gtc/matrix_transform.hpp"
#include "ext/glm/gtc/type_ptr.hpp"
#include "CubeType.h"
#include "Shader.h"
#include "Model.h"

struct Position
{
	float x, y, z;
};

class Renderer
{
public:
	Renderer();

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	void renderCubes(CubeType cubeType, const Shader shader, glm::mat4* models, const int instanceCount);
	void renderModel(Shader shader, glm::mat4* models,const int instanceCount);
	~Renderer();

	friend void renderBasicBlock(const Shader shader, const unsigned int VAO, const int instanceCount, const std::string name,const int durability);
	friend void renderGrass(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability);
	friend void renderDirt(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability);
	friend void renderRedStoneBlock(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability);
	friend void renderOak(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability);
	friend void renderLeaf(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability);
	friend void renderModel(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability);
	friend void renderFlower(const Shader shader, const unsigned int VAO, const int instanceCount, const int durability);
private:
};

