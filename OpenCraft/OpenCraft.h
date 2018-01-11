#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include "glad.h"
#include <GLFW/glfw3.h>
#include "ext/glm/glm.hpp"
#include "ext/glm/gtc/matrix_transform.hpp"
#include "ext/glm/gtc/type_ptr.hpp"
#include "stb_image.h"

#include "Shader.h"
#include "Renderer.h"
#include "Camera.h"
#include "Item.h"
#include "ChunkManager.h"
#include "Model.h"

constexpr unsigned int DEFAULT_SCR_WIDTH = 800;
constexpr unsigned int DEFAULT_SCR_HEIGHT = 600;
constexpr unsigned int SHADOW_MAP_WIDTH = 1024;
constexpr unsigned int SHADOW_MAP_HEIGHT = 1024;

class OpenCraft
{
public:
	OpenCraft(const unsigned int SCR_WIDTH = DEFAULT_SCR_WIDTH,
		const unsigned int SCR_HEIGHT = DEFAULT_SCR_HEIGHT);

	OpenCraft(const OpenCraft&) = delete;
	OpenCraft& operator=(const OpenCraft&) = delete;

	void start(void);

	~OpenCraft();

private:
	const unsigned int m_screenWidth;
	const unsigned int m_screenHeight;
	unsigned int depthMapFBO;
	unsigned int depthMap;

	//C-style ptr
	GLFWwindow* m_window = NULL;
	
	//render
	Renderer m_renderer;

	// timing
	float m_deltaTime = 0.0f;
	float m_lastFrame = 0.0f;


	// last position for mouse cursor
	static float m_lastX;
	static float m_lastY;
	// camera
	static Camera m_camera;
	static bool m_firstMouse;
	static ChunkManager m_chunkManager;
	static CubeType m_blockOnHand;
	static int m_handMoveStage;
	static glm::vec3 m_sunPos;

	// basic facility collections
	std::unordered_map<std::string, Shader> m_shaderMap;
	std::unordered_map<std::string, std::unique_ptr<Item>> m_itemMap;
	std::unordered_map<std::string, Model> m_modelMap;

	// init functions
	int initWindowSettings(void);
	void initShaders(void);
	void initItems(void);
	void initModels(void);

	// render loop
	void startRenderLoop(void);

	// process the input
	void processInput(void);
	// update the time
	void updateTime(void);

	// render functions
	void renderSkyBox(void);
	void renderTestBlock(void);
	void renderBlocks(void);
	void renderCrossair(void);
	void renderHand(void);
	void renderTestModel(void);

	//call back functions
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
};

