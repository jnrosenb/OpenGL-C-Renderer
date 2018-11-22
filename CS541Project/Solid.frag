//////////////////////////////////////////
// HEADER								//
//										//
// Name: Jose Rosenbluth Chiu			//
// Course Number: CS541					//
// Programming Assignment number: #3	//
// Task #1								//
// Due date: 10/25/2018       			//
//                  					//
//////////////////////////////////////////

#version 330 core

uniform vec3 light_color;
uniform vec3 diffuse_color;
uniform vec3 specular_k;
uniform int specExponent;

in vec4 normal_vector;
in vec4 light_vector;

out vec4 frag_color;

void main(void) 
{
  vec3 ambient = 0.15f * light_color * diffuse_color;

  vec4 m = normalize(normal_vector);
  vec4 L = normalize(light_vector);
  vec3 diffuse = max(dot(m,L),0) * diffuse_color * light_color;

  vec4 r = 2 * m * max(dot(L, m), 0) - L;
  r = normalize(r);
  vec3 specular = pow(max(dot(r,L),0), specExponent) * vec3(1,1,1) * light_color;

  vec3 finalColor = diffuse + specular + ambient;
  frag_color = vec4(finalColor ,1);
}

