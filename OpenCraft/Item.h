#pragma once

#include "Shader.h"

class Item
{
public:
	virtual void draw(Shader shader) = 0;
	virtual ~Item() = default;
};
