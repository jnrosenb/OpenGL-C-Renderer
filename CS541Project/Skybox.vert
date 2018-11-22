#version 330 core

layout (location = 0) in vec3 pos;

uniform mat4 M;
uniform mat4 P;
uniform mat4 V;

out vec3 uv;

void main()
{
	uv = pos;
	gl_Position = (P * V * M * vec4(pos, 1)).xyww;
}