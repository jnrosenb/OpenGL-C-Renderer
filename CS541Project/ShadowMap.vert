#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 M;
uniform mat4 lightT;

void main()
{
	gl_Position = lightT * M * vec4(position, 1);
}