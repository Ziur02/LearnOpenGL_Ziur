#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 transform;

out vec2 TexCoord;
out vec4 ourColor;

void main()
{
	gl_Position = transform * vec4(aPos, 1.0);
	ourColor = vec4(aColor, 1.0);
	TexCoord = aTexCoord;
};

#shader fragment
#version 330 core

in vec2 TexCoord;
in vec4 ourColor;

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
};