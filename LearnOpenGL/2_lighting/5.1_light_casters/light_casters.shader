#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);

	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoords = aTexCoords;
}

#shader fragment
#version 330 core

struct Material {
	vec3 ambient;
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct DirLight {
	vec3 direction;
	vec3 position;
	vec3 color;
};

struct PointLight {
	vec3 position;
	vec3 color;

	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	vec3 direction;
	vec3 position;
	vec3 color;

	float cosPhyInner;
	float cosPhyOuter;

	float constant;
	float linear;
	float quadratic;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 ambientColor;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 cameraPos;

uniform DirLight lightD0;
uniform PointLight lightP0;
uniform SpotLight lightS0;

uniform Material material;

vec3 CalDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir);
vec3 CalSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	vec3 result = vec3(0, 0, 0);
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 norm = normalize(Normal);

	// ambient
	vec3 ambient = texture(material.diffuse, TexCoords).rgb * ambientColor * material.ambient;

	result += CalDirLight(lightD0, norm, viewDir);
	result += CalPointLight(lightP0, norm, FragPos, viewDir);
	result += CalSpotLight(lightS0, norm, FragPos, viewDir);

	result += ambient;

	FragColor = vec4(result, 1.0);
}

vec3 CalDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	// diffuse
	float diff = max(dot(normal, light.direction), 0.0);
	vec3 diffuse = texture(material.diffuse, TexCoords).rgb * diff * light.color;

	// specular
	vec3 reflectDir = reflect(-light.direction, normal);
	float spec = pow(max((dot(viewDir, reflectDir)), 0.0), material.shininess);
	vec3 specular = texture(material.specular, TexCoords).rgb * spec * light.color;

	return (diffuse + specular);
}

vec3 CalPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

	// diffuse
	float diff = max((dot(normal, lightDir)), 0.0);
	vec3 diffuse = texture(material.diffuse, TexCoords).rgb * diff * light.color;

	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max((dot(viewDir, reflectDir)), 0.0), material.shininess);
	vec3 specular = texture(material.specular, TexCoords).rgb * spec * light.color;

	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	diffuse *= attenuation;
	specular *= attenuation;

	return (diffuse + specular);
}

vec3 CalSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

	// diffuse
	float diff = max((dot(normal, lightDir)), 0.0);
	vec3 diffuse = texture(material.diffuse, TexCoords).rgb * diff * light.color;

	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max((dot(viewDir, reflectDir)), 0.0), material.shininess);
	vec3 specular = texture(material.specular, TexCoords).rgb * spec * light.color;

	// soft edge spotlight
	float cosTheta = dot(lightDir, -light.direction);
	float epsilon = light.cosPhyInner - light.cosPhyOuter;
	float intensity = clamp((cosTheta - light.cosPhyOuter) / epsilon, 0.0, 1.0);

	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return(diffuse + specular);
}