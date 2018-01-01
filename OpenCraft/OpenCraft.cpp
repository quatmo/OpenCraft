#include <vector>
#include <iostream>
#include "OpenCraft.h"
#include "SkyBox.h"
#include "Block.h"
#include "Crosshair.h"


Camera OpenCraft::m_camera = glm::vec3(-10.0f, 10.0f, -5.0f);
ChunkManager OpenCraft::m_chunkManager(glm::vec3(-10.0f, 0.0f, -5.0f));
bool OpenCraft::m_firstMouse = true;
float OpenCraft::m_lastX = 0.0;
float OpenCraft::m_lastY = 0.0;

OpenCraft::OpenCraft(const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT)
	:m_screenWidth(SCR_WIDTH), m_screenHeight(SCR_HEIGHT)
{
	m_lastX = static_cast<float>(SCR_WIDTH / 2.0);
	m_lastY = static_cast<float>(SCR_HEIGHT / 2.0);
}

void OpenCraft::start(void)
{
	auto ret = initWindowSettings();
	assert(ret == 0);

	// do the basic initializations
	initShaders();
	initItems();

	startRenderLoop();
}

OpenCraft::~OpenCraft()
{
	glfwTerminate();
}

int OpenCraft::initWindowSettings(void)
{
	glfwInit();
	//OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	m_window = glfwCreateWindow(m_screenWidth, m_screenHeight, "Opencraft", NULL, NULL);
	if (m_window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(m_window, 100, 100); // init window position
	glfwMakeContextCurrent(m_window);

	// set callback functions
	glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
	glfwSetCursorPosCallback(m_window, mouse_callback);
	glfwSetScrollCallback(m_window, scroll_callback);
	glfwSetMouseButtonCallback(m_window, mouse_button_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);
	return 0;
}

void OpenCraft::initShaders(void)
{
	m_shaderMap.emplace("skybox", Shader("./shaders/skybox.vs", "./shaders/skybox.fs"));
	m_shaderMap.emplace("crosshair", Shader("./shaders/crosshair.vs", "./shaders/crosshair.fs"));
	m_shaderMap.emplace("cube", Shader("./shaders/cube.vs", "./shaders/cube.fs"));
}

void OpenCraft::initItems(void)
{
	m_itemMap.emplace("skybox", new SkyBox);
	m_itemMap.emplace("block", new Block);
	m_itemMap.emplace("crosshair", new Crosshair);
}

void OpenCraft::startRenderLoop(void)
{
	while (!glfwWindowShouldClose(m_window))
	{
		// per-frame time logic
		updateTime();
		m_chunkManager.updateChunks(m_camera.getPosition());
		// input
		processInput();

		// clear the window
		glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// render items

		renderTestBlock();
		renderSkyBox();
		renderCrossair();

		// final works
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
}

void OpenCraft::processInput(void)
{
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(m_window, true);
	}
	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
	{
		m_camera.processKeyboard(CameraMovement::FORWARD, m_deltaTime);
	}
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_camera.processKeyboard(CameraMovement::BACKWARD, m_deltaTime);
	}
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
	{
		m_camera.processKeyboard(CameraMovement::LEFT, m_deltaTime);
	}
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
	{
		m_camera.processKeyboard(CameraMovement::RIGHT, m_deltaTime);
	}
	if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		m_camera.processKeyboard(CameraMovement::UP, m_deltaTime);
	}
	if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		m_camera.processKeyboard(CameraMovement::DOWN, m_deltaTime);
	}
}

void OpenCraft::updateTime(void)
{
	static size_t counter = 0;
	counter += 1;
	if (counter % 100 == 0)
	{
		std::cerr << "FPS: " << 1 / m_deltaTime << std::endl;
	}
	auto currentFrame = static_cast<float>(glfwGetTime());
	m_deltaTime = currentFrame - m_lastFrame;
	m_lastFrame = currentFrame;
}

void OpenCraft::renderSkyBox(void)
{
	auto skyboxShader = m_shaderMap.at("skybox");
	glm::mat4 model;
	glm::mat4 view = glm::mat4(glm::mat3(m_camera.getViewMatrix()));// remove translation from the view matrix
	glm::mat4 projection = glm::perspective(glm::radians(m_camera.getZoom()), (float)m_screenWidth / (float)m_screenHeight, 0.1f, 100.0f);
	skyboxShader.use();
	skyboxShader.setMat4("model", model);
	skyboxShader.setMat4("view", view);
	skyboxShader.setMat4("projection", projection);
	m_itemMap.at("skybox")->draw(skyboxShader);
}

void OpenCraft::renderTestBlock(void)
{
	glEnable(GL_FRAMEBUFFER_SRGB);
	static glm::mat4 models[16 * 16 * 256 * 4];

	auto blockShader = m_shaderMap.at("cube");
	glm::mat4 view = m_camera.getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_camera.getZoom()), (float)m_screenWidth / (float)m_screenHeight, 0.1f, 64.0f);
	blockShader.use();
	blockShader.setMat4("view", view);
	blockShader.setMat4("projection", projection);

	blockShader.setVec3("viewPos", m_camera.getPosition());

	blockShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	blockShader.setVec3("dirLight.ambient", 0.4f, 0.4f, 0.4f);
	blockShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	blockShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

	blockShader.setVec3("spotLight.position", m_camera.getPosition());
	blockShader.setVec3("spotLight.direction", m_camera.getFront());
	blockShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	blockShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	blockShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	blockShader.setFloat("spotLight.constant", 1.0f);
	blockShader.setFloat("spotLight.linear", 0.09f);
	blockShader.setFloat("spotLight.quadratic", 0.032f);
	blockShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	blockShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

	int index = 0;
	int chunkCount = 0;
	int updated = 0;
	auto chunks = m_chunkManager.getChunks(&updated);
	static std::unordered_map<CubeType, std::list<glm::mat4>> cubeModelMatlMap;
	static std::unordered_map<CubeType, std::pair<int, int>> fastModelMatMap;
	if (updated == 0) //need to update model matrics
	{
		cubeModelMatlMap.clear();
		fastModelMatMap.clear();
		for (auto chunk : chunks)
		{
			for (int x = 0; x < 16; x++)
			{
				for (int z = 0; z < 16; z++)
				{
					for (int y = 0; y < 256; y++)
					{
						if (chunk->cubes[x * 256 * 16 + z * 256 + y].type != 0) // it's not the air block
						{
							glm::mat4 model;
							model = glm::translate(model, glm::vec3(chunk->p * 16, 0, chunk->q * 16));
							model = glm::translate(model, glm::vec3(x, y, z));
							model = glm::scale(model, glm::vec3(0.5f));
							cubeModelMatlMap[chunk->cubes[x * 256 * 16 + z * 256 + y].type].push_back(model);
						}
					}
				}
			}
			chunkCount++;
		}
		int index = 0;
		for (auto cubeModelMat : cubeModelMatlMap)
		{
			auto cubeType = cubeModelMat.first;
			auto instanceCount = cubeModelMat.second.size();
			fastModelMatMap[cubeType].first = index;
			fastModelMatMap[cubeType].second = instanceCount;
			for (auto model : cubeModelMat.second)
			{
				models[index++] = model;
			}

		}
	}

	for (auto cubeModelMat : fastModelMatMap)
	{
		auto cubeType = cubeModelMat.first;
		auto start = cubeModelMat.second.first;
		auto instanceCount = cubeModelMat.second.second;
		fastModelMatMap[cubeType].second = instanceCount;
		m_renderer.renderCubes(cubeType, blockShader, models + start, instanceCount);
	}
}

void OpenCraft::renderCrossair(void)
{
	auto crosshairShader = m_shaderMap.at("crosshair");
	glm::mat4 model;
	model = glm::scale(model, glm::vec3(0.025f));
	glm::mat4 view;
	glm::mat4 projection;
	crosshairShader.use();
	crosshairShader.setMat4("model", model);
	crosshairShader.setMat4("view", view);
	crosshairShader.setMat4("projection", projection);
	m_itemMap.at("crosshair")->draw(crosshairShader);
}

void OpenCraft::framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void OpenCraft::mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
	if (m_firstMouse) // the first time to move the mouse
	{
		m_lastX = static_cast<float>(xpos);
		m_lastY = static_cast<float>(ypos);
		m_firstMouse = false;
	}
	float xoffset = static_cast<float>(xpos - m_lastX);
	float yoffset = static_cast<float>(m_lastY - ypos); // reversed since y-coordinates go from bottom to top
	m_lastX = static_cast<float>(xpos);
	m_lastY = static_cast<float>(ypos);
	m_camera.processMouseMovement(xoffset, yoffset);
}

void OpenCraft::scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
	m_camera.processMouseScroll(static_cast<float>(yoffset));
}

void OpenCraft::mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cerr<< "Mosue left button clicked!"<<std::endl;
			m_chunkManager.tryDestoryCube(m_camera.getPosition(), m_camera.getFront());
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cerr << "Mosue middle button clicked!" << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cerr << "Mosue right button clicked!" << std::endl;
			break;
		default:
			break;
		}
	}
	return;
}
