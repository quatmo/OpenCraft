#include <vector>
#include <iostream>
#include "OpenCraft.h"
#include "TextureManager.h"
#include "SkyBox.h"
#include "Block.h"
#include "Crosshair.h"


Camera OpenCraft::m_camera = glm::vec3(-0.0f, 0.625f, -0.0f);
ChunkManager OpenCraft::m_chunkManager(glm::vec3(-10.0f, 0.0f, -5.0f));
bool OpenCraft::m_firstMouse = true;
float OpenCraft::m_lastX = 0.0;
float OpenCraft::m_lastY = 0.0;
CubeType OpenCraft::m_blockOnHand = 2;
int OpenCraft::m_handMoveStage = 0;
glm::vec3 OpenCraft::m_sunPos(3.5f, -4.0f, 5.0f);


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

	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// do the basic initializations
	initShaders();
	initItems();
	initModels();

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
	m_shaderMap.emplace("flower", Shader("./shaders/flower.vs", "./shaders/flower.fs"));
	m_shaderMap.emplace("depthMap", Shader("./shaders/depthMap.vs", "./shaders/depthMap.fs"));
	m_shaderMap.emplace("model", Shader("./shaders/model.vs", "./shaders/model.fs"));
	m_shaderMap.emplace("debugDepth", Shader("./shaders/debugDepth.vs", "./shaders/debugDepth.fs"));
	
}

void OpenCraft::initItems(void)
{
	m_itemMap.emplace("skybox", new SkyBox);
	m_itemMap.emplace("block", new Block);
	m_itemMap.emplace("crosshair", new Crosshair);
}

void OpenCraft::initModels(void)
{
	//m_modelMap.emplace("sun", Model("./models/sun/sun.obj"));
}

void OpenCraft::startRenderLoop(void)
{
	while (!glfwWindowShouldClose(m_window))
	{
		// per-frame time logic
		updateTime();
		m_camera.step(m_deltaTime,m_chunkManager);
		m_chunkManager.updateChunks(m_camera.getPosition());

		// input
		processInput();

		// clear the window
		glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// render items

		//renderTestBlock();

		renderBlocks();
		renderSkyBox();
		renderCrossair();
		renderHand();
		//renderModel();
		

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
	if (glfwGetKey(m_window, GLFW_KEY_1) == GLFW_PRESS)
	{
		m_handMoveStage = 16;
		m_blockOnHand = 1;
	}
	if (glfwGetKey(m_window, GLFW_KEY_2) == GLFW_PRESS)
	{
		m_handMoveStage = 16;
		m_blockOnHand = 2;
	}
	if (glfwGetKey(m_window, GLFW_KEY_3) == GLFW_PRESS)
	{
		m_handMoveStage = 16;
		m_blockOnHand = 3;
	}
	if (glfwGetKey(m_window, GLFW_KEY_4) == GLFW_PRESS)
	{
		m_handMoveStage = 16;
		m_blockOnHand = 4;
	}
	if (glfwGetKey(m_window, GLFW_KEY_5) == GLFW_PRESS)
	{
		m_handMoveStage = 16;
		m_blockOnHand = 1026;
	}
	if (glfwGetKey(m_window, GLFW_KEY_6) == GLFW_PRESS)
	{
		m_handMoveStage = 16;
		m_blockOnHand = 1025;
	}
	if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		m_camera.jump();
	}
	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
	{
		m_camera.processKeyboard(CameraMovement::FORWARD, m_deltaTime,m_chunkManager);
	}
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_camera.processKeyboard(CameraMovement::BACKWARD, m_deltaTime, m_chunkManager);
	}
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
	{
		m_camera.processKeyboard(CameraMovement::LEFT, m_deltaTime, m_chunkManager);
	}
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
	{
		m_camera.processKeyboard(CameraMovement::RIGHT, m_deltaTime, m_chunkManager);
	}
	if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		m_camera.processKeyboard(CameraMovement::UP, m_deltaTime, m_chunkManager);
	}
	if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		m_camera.processKeyboard(CameraMovement::DOWN, m_deltaTime, m_chunkManager);
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


unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void OpenCraft::renderTestBlock(void)
{
	glEnable(GL_FRAMEBUFFER_SRGB);
	static glm::mat4 models[16 * 16 * 256 * 4];

	auto flowerShader = m_shaderMap.at("flower");
	auto blockShader = m_shaderMap.at("cube");
	glm::mat4 view = m_camera.getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_camera.getZoom()), (float)m_screenWidth / (float)m_screenHeight, 0.1f, 20.0f);
	
	flowerShader.use();
	flowerShader.setMat4("view", view);
	flowerShader.setMat4("projection", projection);
	
	blockShader.use();
	blockShader.setMat4("view", view);
	blockShader.setMat4("projection", projection);
	blockShader.setVec3("viewPos", m_camera.getPosition());
	//blockShader.setVec3("dirLight.direction", 0.0f, 1.0f, -1.0f);
	blockShader.setVec3("dirLight.direction", 3.5f, -4.0f, 5.0f);
	blockShader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
	blockShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	blockShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

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
		auto realPos = m_chunkManager.calRealPos(m_camera.getPosition());
		auto posP = std::get<0>(realPos);
		auto posQ = std::get<1>(realPos);
		for (auto chunk : chunks)
		{
			if (abs(chunk->p - posP) > 1 && abs(chunk->q - posQ) > 1)
			{
				auto viewDir = glm::vec2(m_camera.getFront()[0], m_camera.getFront()[1]);
				auto chunkDir = glm::vec2(chunk->p - posP, chunk->q - posQ);
				if (glm::dot(viewDir, chunkDir) <= 0)
				{
					continue;
				}
			}
			for (int x = 0; x < 16; x++)
			{
				for (int z = 0; z < 16; z++)
				{
					for (int y = 0; y < 256; y++)
					{
						if (chunk->cubes[x * 256 * 16 + z * 256 + y].type != 0) // it's not the air block
						{
							if (m_chunkManager.testSurrounding(chunk->p, chunk->q, x, y, z) == true)
							{
								continue;
							}
							glm::mat4 model;
							model = glm::translate(model, glm::vec3(chunk->p * 4, 0, chunk->q * 4));
							model = glm::translate(model, glm::vec3(x*0.25, y*0.25, z*0.25));
							model = glm::translate(model, glm::vec3(0.125, 0.125,0.125));
							model = glm::scale(model, glm::vec3(0.125f));
							if (chunk->cubes[x * 256 * 16 + z * 256 + y].durability != 0)
							{
								auto durability = chunk->cubes[x * 256 * 16 + z * 256 + y].durability;
								cubeModelMatlMap[chunk->cubes[x * 256 * 16 + z * 256 + y].type+1000000*durability].push_back(model);
							}
							else
							{
								cubeModelMatlMap[chunk->cubes[x * 256 * 16 + z * 256 + y].type].push_back(model);
							}
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
			fastModelMatMap[cubeType].second = static_cast<int>(instanceCount);
			for (auto model : cubeModelMat.second)
			{
				models[index++] = model;
			}
		}
	}
	// render depthMap

	//Render depth map
	
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	const glm::vec3 lightPos(-3.5f, 4.0f, -5.0f);
	auto lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.01f, 20.0f);
	auto lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	auto lightSpaceMatrix = lightProjection * lightView;
	auto depthShader = m_shaderMap.at("depthMap");
	depthShader.use();
	depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	for (auto cubeModelMat : fastModelMatMap)
	{
		auto cubeType = cubeModelMat.first;
		auto start = cubeModelMat.second.first;
		auto instanceCount = cubeModelMat.second.second;
		fastModelMatMap[cubeType].second = instanceCount;
		if ((cubeType % 1000000) < 1024)
		{
			m_renderer.renderCubes(cubeType, depthShader, models + start, instanceCount);
		}
		else
		{
			m_renderer.renderCubes(cubeType, depthShader, models + start, instanceCount);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	
	//auto debugDepthQuad = m_shaderMap.at("debugDepth");
	//debugDepthQuad.use();
	//debugDepthQuad.setInt("depthMap", 0);
	//debugDepthQuad.setFloat("near_plane", 0.01f);
	//debugDepthQuad.setFloat("far_plane", 7.5f);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, depthMap);
	//renderQuad();
	
	// 2. then render scene as normal with shadow mapping (using depth map)
	
	glViewport(0, 0, DEFAULT_SCR_WIDTH, DEFAULT_SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	blockShader.use();
	blockShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	blockShader.setInt("shadowMap", 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	for (auto cubeModelMat : fastModelMatMap)
	{
		auto cubeType = cubeModelMat.first;
		auto start = cubeModelMat.second.first;
		auto instanceCount = cubeModelMat.second.second;
		fastModelMatMap[cubeType].second = instanceCount;
		if ((cubeType % 1000000) < 1024)
		{
			m_renderer.renderCubes(cubeType, blockShader, models + start, instanceCount);
		}
		else
		{
			m_renderer.renderCubes(cubeType, flowerShader, models + start, instanceCount);
		}
	}
}

void OpenCraft::renderBlocks(void)
{
	glEnable(GL_FRAMEBUFFER_SRGB);
	static glm::mat4 models[16 * 16 * 256 * 4];

	auto modelShader = m_shaderMap.at("model");
	auto flowerShader = m_shaderMap.at("flower");
	auto blockShader = m_shaderMap.at("cube");
	glm::mat4 view = m_camera.getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_camera.getZoom()), (float)m_screenWidth / (float)m_screenHeight, 0.1f, 20.0f);

	flowerShader.use();
	flowerShader.setMat4("view", view);
	flowerShader.setMat4("projection", projection);

	modelShader.use();
	modelShader.setMat4("view", view);
	modelShader.setMat4("projection", projection);

	blockShader.use();
	blockShader.setMat4("view", view);
	blockShader.setMat4("projection", projection);
	blockShader.setVec3("viewPos", m_camera.getPosition());

	//blockShader.setVec3("dirLight.direction", 0.0f, 1.0f, -1.0f);
	blockShader.setVec3("dirLight.direction", m_sunPos.x, m_sunPos.y, m_sunPos.z);
	blockShader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
	blockShader.setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
	blockShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

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
	auto renderUnits = m_chunkManager.getRenderUnit(&updated, m_camera.getPosition());
	static std::unordered_map<CubeType, std::list<glm::mat4>> cubeModelMatlMap;
	static std::unordered_map<CubeType, std::pair<int, int>> fastModelMatMap;
	if (updated == 0) //need to update model matrics
	{
		cubeModelMatlMap.clear();
		fastModelMatMap.clear();
		for (auto renderUnit : renderUnits)
		{
			glm::mat4 model;
			model = glm::translate(model, glm::vec3(renderUnit.p * 4, 0, renderUnit.q* 4));
			model = glm::translate(model, glm::vec3(renderUnit.x*0.25, renderUnit.y*0.25, renderUnit.z*0.25));
			model = glm::translate(model, glm::vec3(0.125, 0.125, 0.125));
			model = glm::scale(model, glm::vec3(0.125f));
			if (renderUnit.durability != 0)
			{
				cubeModelMatlMap[renderUnit.type + 1000000 * renderUnit.durability].push_back(model);
			}
			else
			{
				cubeModelMatlMap[renderUnit.type].push_back(model);
			}
			chunkCount++;
		}
		int index = 0;
		for (auto cubeModelMat : cubeModelMatlMap)
		{
			auto cubeType = cubeModelMat.first;
			auto instanceCount = cubeModelMat.second.size();
			fastModelMatMap[cubeType].first = index;
			fastModelMatMap[cubeType].second = static_cast<int>(instanceCount);
			for (auto model : cubeModelMat.second)
			{
				models[index++] = model;
			}
		}
	}

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	const glm::vec3 lightPos(-m_sunPos);
	auto lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.01f, 20.0f);
	auto lightView = glm::lookAt(lightPos, glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
	auto lightSpaceMatrix = lightProjection * lightView;
	auto depthShader = m_shaderMap.at("depthMap");
	depthShader.use();
	depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	for (auto cubeModelMat : fastModelMatMap)
	{
		auto cubeType = cubeModelMat.first;
		auto start = cubeModelMat.second.first;
		auto instanceCount = cubeModelMat.second.second;
		fastModelMatMap[cubeType].second = instanceCount;
		if (cubeType == 1025)
		{
			modelShader.use();
			modelShader.setMat4("view", lightView);
			modelShader.setMat4("projection", lightProjection);
			m_renderer.renderModel(modelShader, models + start, instanceCount);
		}
		else if ((cubeType % 1000000) < 1024)
		{
			m_renderer.renderCubes(cubeType, depthShader, models + start, instanceCount);
		}
		else
		{
			m_renderer.renderCubes(cubeType, depthShader, models + start, instanceCount);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, DEFAULT_SCR_WIDTH, DEFAULT_SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	blockShader.use();
	blockShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	blockShader.setInt("shadowMap", 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	for (auto cubeModelMat : fastModelMatMap)
	{
		auto cubeType = cubeModelMat.first;
		auto start = cubeModelMat.second.first;
		auto instanceCount = cubeModelMat.second.second;
		fastModelMatMap[cubeType].second = instanceCount;
		if (cubeType == 1025)
		{
			modelShader.use();
			modelShader.setMat4("view", view);
			modelShader.setMat4("projection",projection);
			m_renderer.renderModel(modelShader,models+start,instanceCount);
		}
		else if ((cubeType % 1000000) < 1024)
		{
			m_renderer.renderCubes(cubeType, blockShader, models + start, instanceCount);
		}
		else
		{
			m_renderer.renderCubes(cubeType, flowerShader, models + start, instanceCount);
		}
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

void OpenCraft::renderHand(void)
{
	glDisable(GL_DEPTH_TEST);
	auto blockShader = m_shaderMap.at("cube");
	auto modelShader = m_shaderMap.at("model");
	glm::mat4 view;
	glm::mat4 projection = glm::perspective(glm::radians(m_camera.getZoom()), (float)m_screenWidth / (float)m_screenHeight, 0.1f, 20.0f);
	blockShader.use();
	
	if (m_handMoveStage != 0)
	{
		m_handMoveStage -= 1;
		view = glm::rotate(view, (0.1f/16)*m_handMoveStage, glm::vec3(1.0f, 1.0f, 1.0f));
	}
	//(-1.0f, 1.5f,1.0f));
	blockShader.setMat4("view", view);
	blockShader.setMat4("projection", projection);
	blockShader.setVec3("viewPos", m_camera.getPosition());
	//blockShader.setVec3("dirLight.direction", 0.0f, 1.0f, -1.0f);
	blockShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	blockShader.setVec3("dirLight.ambient", 0.7f, 0.7f, 0.7f);
	modelShader.use();
	modelShader.setMat4("view", view);
	modelShader.setMat4("projection", projection);

	glm::mat4 model;

	if (m_blockOnHand == 1025)
	{
		model = glm::translate(model, glm::vec3(0.2f, -0.2f, -0.5f));
		model = glm::rotate(model, 25.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		m_renderer.renderModel(modelShader, &model, 1);
	}
	else
	{
		model = glm::translate(model, glm::vec3(0.2f, -0.2f, -0.5f));
		model = glm::rotate(model, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		m_renderer.renderCubes(m_blockOnHand, blockShader, &model, 1);
	}
	glEnable(GL_DEPTH_TEST);
}

void OpenCraft::renderTestModel(void)
{
	auto planetShader = m_shaderMap.at("sun");
	planetShader.use();
	glm::mat4 model;
	model = glm::rotate(model, static_cast<float>(glfwGetTime() / 10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f));
	glm::mat4 view = m_camera.getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_camera.getZoom()), (float)DEFAULT_SCR_WIDTH / (float)DEFAULT_SCR_HEIGHT, 0.1f, 3000.0f);
	planetShader.setMat4("model", model);
	planetShader.setMat4("view", view);
	planetShader.setMat4("projection", projection);
	planetShader.setVec3("cameraPos", m_camera.getPosition());
	m_modelMap.at("sun").draw(planetShader);	// draw scene as normal
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
			m_handMoveStage = 16;
			m_chunkManager.tryDestoryCube(m_camera.getPosition(), m_camera.getFront());
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			m_chunkManager.tryPlaceCube(m_camera.getPosition(), m_camera.getFront(),m_blockOnHand);
			break;
		default:
			break;
		}
	}
	return;
}
