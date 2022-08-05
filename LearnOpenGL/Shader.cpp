#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	std::ifstream vertexFile;
	std::ifstream fragmentFile;
	std::stringstream vertexStream;
	std::stringstream fragmentStream;

	// ���ļ�
	vertexFile.open(vertexPath);
	fragmentFile.open(fragmentPath);

	vertexFile.exceptions(std::ifstream::failbit || std::ifstream::badbit);
	fragmentFile.exceptions(std::ifstream::failbit || std::ifstream::badbit);

	try
	{
		if (!vertexFile.is_open() || !vertexFile.is_open())
		{
			throw std::exception("open file error");
		}
		// ��ȡ�ļ��Ļ������ݵ���������
		vertexStream << vertexFile.rdbuf();
		fragmentStream << fragmentFile.rdbuf();
		// ת����������string
		vertexString = vertexStream.str();
		fragmentString = fragmentStream.str();
		// ����ת��ΪOpengGL�ܹ�ʶ���const char*����
		vertexSource = vertexString.c_str();
		fragmentSource = fragmentString.c_str();

		unsigned int vertex, fragment;

		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertexSource, NULL);
		glCompileShader(vertex);
		// fragment shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragmentSource, NULL);
		glCompileShader(fragment);
		// ������shader����
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		//�ڱ��롢link��֮��ɾ��shader
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	catch (const std::exception& e)
	{
		std::cout << "EROOR::SHADER::FILE_NOT_SUCCESFULLY_READ:" << e.what() << std::endl;
	}
}

void Shader::use()
{
	glUseProgram(ID);
}