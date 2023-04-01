#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../ShaderUni.h"
#include "../../Camera.h"
#include "../../Material.h"
#include "../../DirLight.h"
#include "../../PointLight.h"
#include "../../SpotLight.h"
#include "../../Model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../../stb_image.h"

#pragma region Camera Declare
// ���
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
Camera camera(cameraPos, 0.0f, -90.0f, cameraUp);
#pragma endregion

#pragma region Light Declare
DirLight lightD0(
	glm::vec3(10.0f, 10.0f, -5.0f),
	glm::vec3(glm::radians(45.0f),0, 0)
);

PointLight lightP0(
	glm::vec3(5.0f, 0, 2.0f)
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

// ��ʼ�����λ�õ���Ļ����
float lastX = 400;
float lastY = 300;

// ����һ������ֵ�ж�����Ƿ��ǵ�һ�ν��봰��
bool firstMouse = true;

// ��ʼ��ʱ��
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// ������ESC��ʱ�رմ���
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

// �����ڴ�С�����仯ʱ��ִ�����º����������ڴ�С����һ���ص�����
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	// �ж�����Ƿ��ǵ�һ�ν��봰��
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	// �������ÿ�ε��õ�ƫ����
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
	// ��ʼ��GLFW�����ð汾��
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// ���ڴ�С����
	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;

	// ����GLFW����
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// ���ع��
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// �������ô��ڴ�С�Ļص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// ������glfw��������ƶ��Ļص�����
	glfwSetCursorPosCallback(window, mouse_callback);
	// ���ü�������ֵĻص�����
	glfwSetScrollCallback(window, scroll_callback);

	// ��ʼ��GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		glfwTerminate();
		return -1;
	}

	// �����߿�ģʽ
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// ������Ȳ���
	glEnable(GL_DEPTH_TEST);
	#pragma endregion

	#pragma region Init Shader Program
	// ʹ��д�õ�Shader�����shader����
	Shader ourShader("3_model_loding/2.1_models/models.shader");
	#pragma endregion
	
	#pragma region Model Data
	//Model model0("E:/CppProgram/LearnOpenGL/LearnOpenGL/resources/models/backpack/backpack.obj");
	Model model0("E:/CppProgram/LearnOpenGL/LearnOpenGL/resources/models/nanosuit/nanosuit.obj");
	#pragma endregion

	#pragma region Prepare MVP matrices
	//model
	glm::mat4 model = glm::mat4(1.0f);
	//view
	glm::mat4 view;
	//projection
	glm::mat4 projection;
	#pragma endregion

	// ��Ⱦѭ��(Render Loop)
	while (!glfwWindowShouldClose(window))
	{
		// ����deltaTime
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// ����
		processInput(window);

		// ��ջ���
		//glClearColor(0.8f, 0, 0.4f, 1.0f);
		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ����shader
		ourShader.use();

		// ����projection����
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		// ����view����
		view = camera.GetViewMatrix();
		// ����model����
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

		// ͨ��uniform�����ݴ���shader
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
		glUniform3f(glGetUniformLocation(ourShader.ID, "ambientColor"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(ourShader.ID, "cameraPos"), camera.cameraPos.x, camera.cameraPos.y, camera.cameraPos.z);
		
		glUniform3f(glGetUniformLocation(ourShader.ID, "ambient"), 0.7f, 0.7f, 0.7f);
		glUniform1f(glGetUniformLocation(ourShader.ID, "shininess"), 32.0f);

		model0.Draw(ourShader);

		// ��鲢�����¼�����������
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// ����Ⱦѭ���������ͷ�֮ǰ����Դ
	glfwTerminate();
	return 0;
}