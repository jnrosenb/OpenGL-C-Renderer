#version 330 core

#define LIGHT_NUM 1
#define SPECPOWER 10
#define SPECINTENSITY 1.0f

in vec3 N;
in vec2 UV;
in vec4 light[LIGHT_NUM];
in vec3 view;
in vec4 shadowSpacePos;

uniform float specPower;
uniform float specIntensity;
uniform vec4 lightColor[LIGHT_NUM];

uniform sampler2D diffuseTexture;
uniform sampler2D shadowTexture;

out vec4 fragColor;

float shadowCalculations(vec4 ShadowSpacePos);

void main()
{
    vec3 diffuse_color = texture(diffuseTexture, UV).xyz;
	//vec3 diffuse_color = vec3(0.4, 0.7, 1.0);

	float shadowed = shadowCalculations(shadowSpacePos);
	//float shadowed = textureProj( shadowTexture, shadowSpacePos );

    float lightContribution = 1.0f / LIGHT_NUM;  //NOT USING IT//
    vec3 v = normalize(view);

    //Ambient is going to be with one white light
    float intensity = 0.25f;
    vec3 ambient = intensity * diffuse_color * vec3(1, 1, 1);

    vec3 diffuse, specular;
    for (int i = 0; i < LIGHT_NUM; ++i)
    {
        //Diffuse component
        vec3 m = normalize(N);
        vec3 l = normalize(light[i].xyz);
        diffuse += max(dot(m, l), 0) * diffuse_color * lightColor[i].xyz;

        //Specular component
        vec3 r = (2 * max(dot(m, l), 0))*m - l;
        r = normalize(r); //Just in case//
        specular += specIntensity * (pow(max(dot(l, r), 0), specPower) * lightColor[i].xyz ); //NOT SURE ABOUT SPEC INTENSITY//
    }
	
    //vec3 finalColor = ambient + ( diffuse + specular );
    vec3 finalColor = ambient + shadowed * ( diffuse + specular );
	fragColor = vec4(finalColor.xyz, 1);
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
	//currentDepth = min(1.0f, currentDepth);
	//currentDepth = max(0.0f, currentDepth);

	//if (ndc.x < 0.0f || ndc.y < 0.0f)
	//	return 0.0f;
	//if (ndc.x >  1.0f || ndc.y > 1.0f)
	//	return 0.0f;
	if ((ndc.x - 0.5f)*(ndc.x - 0.5f) + (ndc.y - 0.5f)*(ndc.y - 0.5f) > 0.25f)
		return 0.0f;

	float bias = 0.0005f;
	float shadow = currentDepth - bias < depthTest  ? 1.0f : 0.0f;
	return (shadow);
}