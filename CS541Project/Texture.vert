#version 330 core

#define LIGHT_NUM 1

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvcoords;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 NT;
uniform vec3 eye;  //IN WORLD POS
uniform vec4 lightPosition[LIGHT_NUM];
uniform mat4 lTransformation;

out vec3 N;
out vec2 UV;
out vec4 light[LIGHT_NUM];
out vec3 view;
out vec4 shadowSpacePos;

void main()
{
    vec4 worldPos = M * vec4(position, 1);
    gl_Position = P * V * worldPos;
    N = (NT * vec4(normal.xyz, 1)).xyz;
    
	UV = uvcoords;
    for (int i = 0; i < LIGHT_NUM; i++)
    {
        light[i] = lightPosition[i] - worldPos;
    }

	shadowSpacePos = lTransformation * worldPos;

    view = eye - worldPos.xyz;
}
