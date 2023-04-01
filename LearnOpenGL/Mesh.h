#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "ShaderUni.h"
#include <gl/glew.h>
#include <iostream>

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	Mesh(float vertices[]); // Ϊ�˲���д��Mesh�࣬��һ��ֻ���붥��Ĺ��캯��
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	
	void Draw(Shader& shader);

private:
	unsigned int VAO, VBO, EBO;
	void setupMesh();
};