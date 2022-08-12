#pragma once
#include <string>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	void checkCompileErrors(unsigned int ID, std::string type);
public:
	unsigned int ID;  //shader program id
	Shader(const std::string& filePath);
	void use();
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
};