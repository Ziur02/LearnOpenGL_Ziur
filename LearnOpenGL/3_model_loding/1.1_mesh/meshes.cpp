#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../../stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../ShaderUni.h"
#include "../../Camera.h"
#include "../../Material.h"
#include "../../DirLight.h"
#include "../../PointLight.h"
#include "../../SpotLight.h"
#include "../../Mesh.h"

// 整理出来一个加载texture到gpu的函数
unsigned int LoadImageToGPU(const char* filepath, GLint internalFormat, GLenum format, int textureSlot)
{
	// 创建纹理，绑定纹理目标
	unsigned int texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	glBindTexture(GL_TEXTURE_2D, texture);

	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// 加载并生成纹理
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	// 施放内存
	stbi_image_free(data);

	return texture;
}

#pragma region Camera Declare
// 相机
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
//Camera camera (cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), cameraUp);
Camera camera(cameraPos, 0.0f, -90.0f, cameraUp);
#pragma endregion

#pragma region Light Declare
DirLight lightD0(
	glm::vec3(10.0f, 10.0f, -5.0f),
	glm::vec3(glm::radians(45.0f),0, 0)
);

PointLight lightP0(
	glm::vec3(0, 1.0f, 0)
);

SpotLight lightS0(
	glm::vec3(0, 0, 0),
	glm::vec3(0, 0, 0),
	glm::vec3(1.0f, 1.0f, 1.0f)
);
#pragma endregion

#pragma region Input Declare
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// 初始化鼠标位置到屏幕中心
float lastX = 400;
float lastY = 300;

// 设置一个布尔值判断鼠标是否是第一次进入窗口
bool firstMouse = true;

// 初始化时间
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 当按下ESC键时关闭窗口
void processInput(GLFWwindow* window)
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//float cameraSpeed = static_cast<float>(2.5 * deltaTime);
	float cameraSpeed = camera.MovementSpeed;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
}

// 当窗口大小发生变化时，执行以下函数调整窗口大小，是一个回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	// 判断鼠标是否是第一次进入窗口
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	// 计算鼠标每次调用的偏移量
	float deltaX = xpos - lastX;
	float deltaY = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(deltaX, deltaY);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
#pragma endregion

int main()
{
	#pragma region Open a Window
	// 初始化GLFW，设置版本号
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 窗口大小设置
	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;

	// 创建GLFW窗口
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// 隐藏光标
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// 设置重置窗口大小的回调函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// 设置让glfw监听鼠标移动的回调函数
	glfwSetCursorPosCallback(window, mouse_callback);
	// 设置检测鼠标滚轮的回调函数
	glfwSetScrollCallback(window, scroll_callback);

	// 初始化GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		glfwTerminate();
		return -1;
	}

	// 设置线框模式
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 激活深度测试
	glEnable(GL_DEPTH_TEST);
	#pragma endregion

	#pragma region Init Shader Program
	// 使用写好的Shader类编译shader程序
	Shader ourShader("3_model_loding/1.1_mesh/meshes.shader");
	#pragma endregion

	#pragma region Init Material
	Material ourMaterial(
		ourShader,
		LoadImageToGPU("../resources/textures/container2.png" , GL_RGBA, GL_RGBA, 0),
		LoadImageToGPU("../resources/textures/container2_specular.png", GL_RGBA, GL_RGBA, 1),
		glm::vec3(1.0f, 1.0f, 1.0f),
		32.0f
	);
	#pragma endregion

	#pragma region Model Data
	// 设置顶点数据
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	#pragma endregion

	#pragma region Init and Load Models to VAO, VBO
	Mesh cube(vertices);

	//// 创建一个顶点数组对象VAO和一个顶点缓冲对象VBO
	//unsigned int VBO, VAO;
	//glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &VBO);
	//// 绑定VAO，绑定VBO到GL_ARRAY_BUFFER目标上
	//glBindVertexArray(VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//// 将顶点数组复制到缓冲中供OpenGL使用
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//// 设置顶点属性指针
	//// 位置属性
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	//// 法线属性
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	//// 纹理坐标属性
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	//glEnableVertexAttribArray(2);
	#pragma endregion

	#pragma region Init and Load Textures
	// 创建纹理
	//unsigned int texture1;
	//texture1 = LoadImageToGPU("../resources/textures/container.jpg" , GL_RGB, GL_RGB, 0);
	//unsigned int texture2;
	//texture2 = LoadImageToGPU("../resources/textures/awesomeface.png", GL_RGBA, GL_RGBA, 0);

	#pragma endregion

	#pragma region Prepare MVP matrices
	//model
	glm::mat4 model;
	//view
	glm::mat4 view;
	//projection
	glm::mat4 projection;
	#pragma endregion

	// 渲染循环(Render Loop)
	while (!glfwWindowShouldClose(window))
	{
		// 计算deltaTime
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 输入
		processInput(window);

		// 清空缓冲
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (unsigned int i = 0; i < 1; i++)
		{
			// 设置projection矩阵
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			// 设置view矩阵
			view = camera.GetViewMatrix();
			// 设置model矩阵
			model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

			// 激活shader
			ourShader.use();

			// 绑定纹理
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, texture1);
			//glActiveTexture(GL_TEXTURE1);
			//glBindTexture(GL_TEXTURE_2D, texture2);

			// 通过uniform将数据传入shader
			glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

			//glUniform3f(glGetUniformLocation(ourShader.ID, "lightD0.position"), lightD0.position.x, lightD0.position.y, lightD0.position.z);
			glUniform3f(glGetUniformLocation(ourShader.ID, "lightD0.color"), lightD0.color.x, lightD0.color.y, lightD0.color.z);
			glUniform3f(glGetUniformLocation(ourShader.ID, "lightD0.angle"), lightD0.angles.x, lightD0.angles.y, lightD0.angles.z);
			glUniform3f(glGetUniformLocation(ourShader.ID, "lightD0.direction"), lightD0.direction.x, lightD0.direction.y, lightD0.direction.z);

			glUniform3f(glGetUniformLocation(ourShader.ID, "lightP0.color"), lightP0.color.x, lightP0.color.y, lightP0.color.z);
			glUniform3f(glGetUniformLocation(ourShader.ID, "lightP0.position"), lightP0.position.x, lightP0.position.y, lightP0.position.z);
			glUniform1f(glGetUniformLocation(ourShader.ID, "lightP0.constant"), lightP0.constant);
			glUniform1f(glGetUniformLocation(ourShader.ID, "lightP0.linear"), lightP0.linear);
			glUniform1f(glGetUniformLocation(ourShader.ID, "lightP0.quadratic"), lightP0.quadratic);

			glUniform3f(glGetUniformLocation(ourShader.ID, "lightS0.color"), lightS0.color.x, lightS0.color.y, lightS0.color.z);
			glUniform3f(glGetUniformLocation(ourShader.ID, "lightS0.angle"), lightS0.angles.x, lightS0.angles.y, lightS0.angles.z);
			//glUniform3f(glGetUniformLocation(ourShader.ID, "lightS0.position"), lightS0.position.x, lightS0.position.y, lightS0.position.z);
			//glUniform3f(glGetUniformLocation(ourShader.ID, "lightS0.direction"), lightS0.direction.x, lightS0.direction.y, lightS0.direction.z);
			glUniform3f(glGetUniformLocation(ourShader.ID, "lightS0.position"), camera.cameraPos.x, camera.cameraPos.y, camera.cameraPos.z);
			glUniform3f(glGetUniformLocation(ourShader.ID, "lightS0.direction"), camera.cameraFront.x, camera.cameraFront.y, camera.cameraFront.z);
			glUniform1f(glGetUniformLocation(ourShader.ID, "lightS0.constant"), lightS0.constant);
			glUniform1f(glGetUniformLocation(ourShader.ID, "lightS0.linear"), lightS0.linear);
			glUniform1f(glGetUniformLocation(ourShader.ID, "lightS0.quadratic"), lightS0.quadratic);
			glUniform1f(glGetUniformLocation(ourShader.ID, "lightS0.cosPhyInner"), lightS0.cosPhyInner);
			glUniform1f(glGetUniformLocation(ourShader.ID, "lightS0.cosPhyOuter"), lightS0.cosPhyOuter);

			glUniform3f(glGetUniformLocation(ourShader.ID, "lightPos"), 10.0f, 10.0f, 0.0f);
			glUniform3f(glGetUniformLocation(ourShader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);
			glUniform3f(glGetUniformLocation(ourShader.ID, "ambientColor"), 0.2f, 0.2f, 0.2f);
			glUniform3f(glGetUniformLocation(ourShader.ID, "cameraPos"), camera.cameraPos.x, camera.cameraPos.y, camera.cameraPos.z);

			glUniform3f(glGetUniformLocation(ourShader.ID, "material.ambient"), ourMaterial.ambient.x, ourMaterial.ambient.y, ourMaterial.ambient.z);
			// glUniform3f(glGetUniformLocation(ourShader.ID, "material.diffuse"), ourMaterial.diffuse.x, ourMaterial.diffuse.y, ourMaterial.diffuse.z);
			glUniform1i(glGetUniformLocation(ourShader.ID, "material.diffuse"), 0);
			glUniform1i(glGetUniformLocation(ourShader.ID, "material.specular"), 1);
			glUniform1f(glGetUniformLocation(ourShader.ID, "material.shininess"), ourMaterial.shininess);

			// 绑定VAO
			//glBindVertexArray(VAO);

			//通过for循环画出10个经历不同model矩阵变换的立方体
			//glDrawArrays(GL_TRIANGLES, 0, 36);

			cube.draw(ourMaterial.shader);
		}

		// 检查并调用事件，交换缓冲
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// 当渲染循环结束后，释放之前的资源
	glfwTerminate();
	return 0;
}