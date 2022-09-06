#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "ShaderUni.h"
#include <gl/glew.h>

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
	Mesh(float vertices[]);
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

	void draw(Shader& shader);

private:
	unsigned int VAO, VBO, EBO;
	void setupMesh();
};