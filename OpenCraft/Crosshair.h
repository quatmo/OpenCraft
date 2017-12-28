#pragma once
#include "Item.h"
class Crosshair :public Item
{
public:
	Crosshair();

	Crosshair(const Crosshair&) = delete;
	Crosshair& operator=(const Crosshair&) = delete;

	void draw(Shader shader) override;
	~Crosshair();
private:
	void initVertexAttributes();
	unsigned int m_VAO;
	unsigned int m_VBO;
};

