#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord = aTexCoord;
};

#shader fragment
#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 objColor;
uniform vec3 ambientColor;

void main()
{
	//FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
	FragColor = vec4(objColor * ambientColor, 1.0);
};