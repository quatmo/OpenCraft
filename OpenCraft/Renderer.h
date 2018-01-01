#pragma once

#include <vector>
#include <string>

#include "CubeType.h"
#include "Shader.h"

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

	void renderCubes(CubeType cubeType, const Shader shader, glm::mat4* models, const size_t instanceCount);
	~Renderer();
	friend void renderGrass(const Shader shader, const unsigned int VAO, const size_t instanceCount);
	friend void renderDirt(const Shader shader, const unsigned int VAO, const size_t instanceCount);
	friend void renderRedStoneBlock(const Shader shader, const unsigned int VAO, const size_t instanceCount);
	friend void renderBasicBlock(const Shader shader, const unsigned int VAO, const size_t instanceCount,const std::string name);
private:
};

