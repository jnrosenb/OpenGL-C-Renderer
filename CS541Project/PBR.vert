#version 330 core

#define MAX_LIGHTS 1

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec2 uvcoords;

uniform mat4 persp_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform mat4 normal_matrix;
uniform vec4 lightPosition[MAX_LIGHTS];
uniform vec3 eye;
uniform int lightCount;
uniform mat4 lTransformation;

out vec4 normal_vector;
out vec4 view_vector;
out vec4 light_pos[MAX_LIGHTS];
out vec2 uv;
out vec4 shadowSpacePos;

void main() 
{
  vec4 P = model_matrix * position;
  gl_Position = persp_matrix * view_matrix * P;
  normal_vector = normal_matrix * normal;

  for (int i = 0; i < MAX_LIGHTS; ++i)
  {
    light_pos[i] = lightPosition[i] - P;
  }

  shadowSpacePos = lTransformation * P;
  
  view_vector = vec4(eye.xyz, 1) - P;
  uv = uvcoords;
}

