#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
out vec4 ourPos;
uniform float xOffset;

void main()
{
	gl_Position = vec4(aPos.x + xOffset, -aPos.y, aPos.z, 1.0);
	ourPos = vec4(aPos, 1.0);
};

#shader fragment
#version 330 core

out vec4 FragColor;
in vec4 ourPos;
void main()
{
	FragColor = ourPos;
};