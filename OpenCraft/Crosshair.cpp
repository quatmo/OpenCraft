#include "Crosshair.h"



Crosshair::Crosshair()
{
	initVertexAttributes();
}


void Crosshair::draw(Shader shader)
{
	shader.use();
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 12);
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}

Crosshair::~Crosshair()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}

void Crosshair::initVertexAttributes()
{
	static float crosshairVertices[3 * 6*2] = {
		// positions          
		-1.0f,  0.15f, -1.0f,
		-1.0f, -0.15f, -1.0f,
		1.0f, -0.15f, -1.0f,
		1.0f, -0.15f, -1.0f,
		1.0f,  0.15f, -1.0f,
		-1.0f,  0.15f, -1.0f,

		-0.15f,  1.0f, -1.0f,
		-0.15f, -1.0f, -1.0f,
		0.15f, -1.0f, -1.0f,
		0.15f, -1.0f, -1.0f,
		0.15f,  1.0f, -1.0f,
		-0.15f,  1.0f, -1.0f,
	};
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), &crosshairVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}
