#version 330 core

uniform samplerCube skybox;

in vec3 uv;

out vec4 frag_color;

void main()
{
	frag_color = texture(skybox, uv);
}