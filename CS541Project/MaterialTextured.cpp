//////////////////////////////////////////
// HEADER								//
//										//
// Name: Jose Rosenbluth Chiu			//
// Course Number: CS541					//
// Programming Assignment number: #5	//
// Task #1								//
// Due date: 12/06/2018       			//
//                  					//
//////////////////////////////////////////

#include "MaterialTextured.h"
#include <iostream>
#include "client.h"

using namespace std;

MaterialTextured::MaterialTextured(GLuint prgm, GLuint tex, float specI, float specP) : 
	program(prgm), texture(tex), specularIntensity(specI), specularPower(specP)
{ 
	cout << "Created new textured material. (CONV CTOR)" << endl;

	init();
}

MaterialTextured::MaterialTextured(const MaterialTextured& rhs) :
	program(rhs.program), texture(rhs.texture), specularIntensity(rhs.specularIntensity),
	specularPower(rhs.specularPower)
{
	cout << "Created new textured material (CPY CTOR)." << endl;
}

MaterialTextured::~MaterialTextured() 
{
	/*REMEMBER TO CHECK IF I NEED TO DESTROY BUFFERS OR ANYTHING HERE*/
	cout << "Destroying textured material (DTOR)." << endl;
}

MaterialTextured& MaterialTextured::operator=(const MaterialTextured& rhs)
{
	if (this != &rhs) 
	{
		program = rhs.program;
		texture = rhs.texture;

		specularIntensity = rhs.specularIntensity;
		specularPower = rhs.specularPower;
	}
	return *this;
}

void MaterialTextured::init() 
{
	//Kinda hardcoded (each material has to know how uniforms are called on shader)
	uModel = glGetUniformLocation(program, "M");
	uNormalTransform = glGetUniformLocation(program, "NT");
	uView = glGetUniformLocation(program, "V");
	uProjection = glGetUniformLocation(program, "P");

	uSpecularIntensity = glGetUniformLocation(program, "specIntensity");
	uSpecularPower = glGetUniformLocation(program, "specPower");

	uLightPositions = glGetUniformLocation(program, "lightPosition");
	uLightColors = glGetUniformLocation(program, "lightColor");
	uEye = glGetUniformLocation(program, "eye");

	uLightTransformation = glGetUniformLocation(program, "lTransformation");
}

void MaterialTextured::passViewAndProj(glm::mat4& V, glm::mat4& P, glm::vec4& eye)
{
	glUseProgram(program);

	glUniformMatrix4fv(uView, 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &P[0][0]);
}

void MaterialTextured::passLightData(glm::vec4 *lightPositions, glm::vec4 *lightColors, int lightNum)
{
	glUseProgram(program);

	glUniform4fv(uLightPositions, lightNum, (float const *)lightPositions);
	glUniform4fv(uLightColors, lightNum, (float const *)lightColors);
}

void MaterialTextured::sendUniformValues(glm::mat4& M, glm::mat4& NT, glm::mat4 const& lightT)
{
	glUseProgram(program);

	//First pass the shader the transform matrices
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &M[0][0]);
	glUniformMatrix4fv(uNormalTransform, 1, GL_FALSE, &NT[0][0]);
	glUniformMatrix4fv(uLightTransformation, 1, GL_FALSE, &lightT[0][0]);

	//Then, the material gets the specific stuff
	glUniform1f(uSpecularIntensity, specularIntensity);
	glUniform1f(uSpecularPower, specularPower);
}

void MaterialTextured::bindTexture()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	GLint uTexture = glGetUniformLocation(program, "diffuseTexture");
	glUniform1i(uTexture, 0); 
}

GLuint MaterialTextured::getProgram()
{
	return program;
}