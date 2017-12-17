#pragma once

#include <memory>
#include "Item.h"

class SkyBox :public Item
{
public:
	SkyBox();

	SkyBox(const SkyBox&) = delete;
	SkyBox& operator=(const SkyBox&) = delete;

	void draw(Shader shader) override;

	~SkyBox();
private:
	void initVertexAttributes();
	unsigned int m_VAO;
	unsigned int m_VBO;
};

