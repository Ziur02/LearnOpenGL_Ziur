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
	Normal = mat3(model) * aNormal;
};

#shader fragment
#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 objColor;
uniform vec3 ambientColor;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main()
{
	// ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * ambientColor;

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = dot(norm, lightDir) * 0.5 + 0.5;
	vec3 diffuse = diff * lightColor;

	// specular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(dot(viewDir, reflectDir) * 0.5 + 0.5, 32);
	vec3 specular = specularStrength * spec * lightColor;

	FragColor = vec4((ambient + diffuse + specular) * objColor, 1.0);
};