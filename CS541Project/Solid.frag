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
in vec4 shadowSpacePos;

out vec4 frag_color;

uniform sampler2D shadowTexture;

float shadowCalculations(vec4 ShadowSpacePos);

void main(void) 
{
  float shadowed = shadowCalculations(shadowSpacePos);
  vec3 ambient = 0.15f * light_color * diffuse_color;

  vec4 m = normalize(normal_vector);
  vec4 L = normalize(light_vector);
  vec3 diffuse = max(dot(m,L),0) * diffuse_color * light_color;

  vec4 r = 2 * m * max(dot(L, m), 0) - L;
  r = normalize(r);
  vec3 specular = pow(max(dot(r,L),0), specExponent) * vec3(1,1,1) * light_color;

  //vec3 finalColor = diffuse + specular + ambient;
  vec3 finalColor = ambient + shadowed * ( diffuse + specular );
  frag_color = vec4(finalColor ,1);
}

float shadowCalculations(vec4 ShadowSpacePos)
{
	///////////////////////////////
	// CHANGE FOR EFFICIENT CODE //
	///////////////////////////////
	
	//if (ShadowSpacePos.w < 0.0f)
	//	return 1.0f;

	vec3 ndc = vec3(ShadowSpacePos.xyz / ShadowSpacePos.w);
	ndc = (ndc * 0.5f) + 0.5f;

	float depthTest = (texture(shadowTexture, ndc.xy)).x;
	float currentDepth = ndc.z;

	if ((ndc.x - 0.5f)*(ndc.x - 0.5f) + (ndc.y - 0.5f)*(ndc.y - 0.5f) > 0.25f)
		return 0.0f;

	float bias = 0.0005f;
	float shadow = currentDepth - bias < depthTest  ? 1.0f : 0.0f;
	return (shadow);
}

