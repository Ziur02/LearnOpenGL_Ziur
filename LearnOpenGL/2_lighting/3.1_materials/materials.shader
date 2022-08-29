#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);

	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
};

#shader fragment
#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 objColor;
uniform vec3 ambientColor;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 cameraPos;

uniform Material material;

void main()
{
	// ambient
	float ambientStrength = 0.1;
	vec3 ambient = material.ambient * ambientColor;

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = material.diffuse * diff * lightColor;

	// specular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = material.specular * spec * lightColor;

	FragColor = vec4((ambient + diffuse + specular) * objColor, 1.0);
};