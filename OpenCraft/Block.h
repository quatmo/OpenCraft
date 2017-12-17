#pragma once

#include <memory>
#include "Item.h"


class Block :public Item
{
public:
	Block();

	void draw(Shader shader) override;

	~Block();

private:
	void initVertexAttributes();
	unsigned int m_VAO;
	unsigned int m_VBO;
};

