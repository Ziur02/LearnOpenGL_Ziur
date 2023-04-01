#include "Model.h"

Model::Model(std::string const &path)
{
	loadModel(path);
}

void Model::Draw(Shader& shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(shader);
	}
}

void Model::loadModel(std::string const &path)
{
	// 通过Assimp::Importer读取文件
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// 检查错误
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	// 获得模型所在的包含纹理图片的目录
	directory = path.substr(0, path.find_last_of('/'));
	std::cout << directory << std::endl;

	// 递归地遍历root node
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	std::cout << node->mName.data << std::endl;
	// 遍历当前节点下的每一个网格
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// node->mMeshes是一个包含了当前节点所有网格的索引的数组
		// 这里使用node包含的索引遍历了所有aiMesh类的对象调用processMesh方法转换为我们自己的Mesh对象，push_back到vector<Mesh> meshes中
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		// 递归地遍历当前节点的所有子节点
		processNode(node->mChildren[i], scene);
	}
}
Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	// 声明我们自己的Mesh类需要的顶点、索引、纹理信息
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// 转换顶点信息
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;
		// 转换顶点位置信息
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// 转换顶点法线信息
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}
		// 转换纹理坐标信息
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			// 一个顶点最多拥有8个不同的纹理坐标，我们这里只使用一个所以索引为0
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
			//// tangent
			//vector.x = mesh->mTangents[i].x;
			//vector.y = mesh->mTangents[i].y;
			//vector.z = mesh->mTangents[i].z;
			//vertex.Tangent = vector;
			//// bitangent
			//vector.x = mesh->mBitangents[i].x;
			//vector.y = mesh->mBitangents[i].y;
			//vector.z = mesh->mBitangents[i].z;
			//vertex.Bitangent = vector;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}
	// 转换索引信息
	// 遍历网格的每个面（一个面是一个三角形的网格）检索相应的顶点索引
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// 一个面可能有多个顶点，由于我们使用了aiProcess_Triangulate选项，所以这里的面总是三角形
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
			
	}
	// 转换材质信息
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// 1. diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		//std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		//textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		//// 4. height maps
		//std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		//textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	//std::vector<Texture> textures;
	//for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	//{
	//	aiString str;
	//	mat->GetTexture(type, i, &str);
	//	Texture texture;
	//	texture.id = TextureFromFile(str.C_Str(), directory);
	//	texture.type = typeName;
	//	texture.path = str.C_Str();
	//	textures.push_back(texture);
	//}
	//return textures;
	
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// 判断纹理是否已经加载过，如果已经加载过则直接使用该纹理，如果没有则加载
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
			std::cout << "load texture: " << str.C_Str() << std::endl;
		}
	}
	return textures;
}

unsigned int Model::TextureFromFile(const char* path, const std::string& directory)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	// 生成buffer
	unsigned int textureID;
	glGenTextures(1, &textureID);

	// 获取图片信息
	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	std::cout << filename.c_str() << std::endl;
	
	if (data)
	{
		// 绑定纹理
		glBindTexture(GL_TEXTURE_2D, textureID);
		
		// 设置纹理格式
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// 将图片数据传递gpu
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// 释放内存
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}