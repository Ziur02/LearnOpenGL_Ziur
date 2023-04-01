#include "Mesh.h"

Mesh::Mesh(float vertices[])
{
	this->vertices.resize(36);
	memcpy(&(this->vertices[0]), vertices, 36 * 8 * sizeof(float));

	setupMesh();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();
}

void Mesh::Draw(Shader& shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse") number = std::to_string(diffuseNr++);
		else if (name == "texture_specular") number = std::to_string(specularNr++);
		else if (name == "texture_normal") number = std::to_string(normalNr++);
		else if (name == "texture_height") number = std::to_string(heightNr++);

		glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);

		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
	// 创建顶点数组对象VAO和顶点缓冲对象VBO和索引数组对象EBO，分别绑定
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// 绑定VAO
	glBindVertexArray(VAO);
	// 绑定VBO到GL_ARRAY_BUFFER缓冲类型的缓冲上
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 将顶点数组复制到缓冲中供OpenGL使用
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	// 绑定EBO到GL_ELEMENT_ARRAY_BUFFER缓冲类型的缓冲上
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// 将索引数组复制到缓冲中供OpenGL使用
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}