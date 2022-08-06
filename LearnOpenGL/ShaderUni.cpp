#include "ShaderUni.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

Shader::Shader(const std::string& shaderPath)
	:ID(0)
{
	// ����ö������
	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	try
	{
		// ���ļ�
		std::ifstream shaderFstream;
		shaderFstream.open(shaderPath);

		// �ж��ļ��Ƿ���������
		if (!shaderFstream.is_open())
		{
			throw std::exception("open file error");
		}

		// ���м������vertex shader��fragment shader����
		std::string line;
		std::stringstream shaderSstream[2];
		ShaderType type = ShaderType::NONE;
		while (getline(shaderFstream, line)) {
			if (line.find("#shader") != std::string::npos) {
				if (line.find("vertex") != std::string::npos) {
					type = ShaderType::VERTEX;
				}
				else if (line.find("fragment") != std::string::npos) {
					type = ShaderType::FRAGMENT;
				}
			}
			else {
				shaderSstream[(int)type] << line << '\n';
			}
		}

		// ת����������string
		std::string vertexString = shaderSstream[0].str();
		std::string fragmentString = shaderSstream[1].str();

		// stringת��ΪOpengGL�ܹ�ʶ���const char*����
		const char* vertexSource = vertexString.c_str();
		const char* fragmentSource = fragmentString.c_str();

		// ���롢link����
		unsigned int vertex, fragment;
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertexSource, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");
		// fragment shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragmentSource, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");
		// ������shader����
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");

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

void Shader::checkCompileErrors(unsigned int ID, std::string type)
{
	int success;
	char infoLog[512];

	if (type != "PROGRAM")
	{
		glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(ID, 512, NULL, infoLog);
			std::cout << "EOORO::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
		}
	}
};

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}