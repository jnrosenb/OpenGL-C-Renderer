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

#include "MaterialPBR.h"
#include <iostream>
#include "client.h"

using namespace std;

MaterialPBR::MaterialPBR(GLuint prgm, GLuint text, float rghss, float perm, float nrc, glm::vec3 diffuseClr) :
	program(prgm), texture(text), roughness(rghss), permeability(perm), normalRefCoeff(nrc), diffuseColor(diffuseClr)
{
	cout << "Created new PBR material. (CONV CTOR)" << endl;

	init();
}

MaterialPBR::MaterialPBR(const MaterialPBR& rhs) :
	program(rhs.program), texture(rhs.texture), roughness(rhs.roughness), permeability(rhs.permeability), 
	normalRefCoeff(rhs.normalRefCoeff), diffuseColor(rhs.diffuseColor)
{
	cout << "Created new PBR material. (COPY CTOR)" << endl;
}

MaterialPBR::~MaterialPBR() 
{
	/*REMEMBER TO CHECK IF I NEED TO DESTROY BUFFERS OR ANYTHING HERE*/
	cout << "Destroying PBR material (DTOR)." << endl;
}

MaterialPBR& MaterialPBR::operator=(const MaterialPBR& rhs) 
{
	if (this != &rhs)
	{
		program = rhs.program;
		texture = rhs.texture;

		roughness = rhs.roughness;
		permeability = rhs.permeability;
		normalRefCoeff = rhs.normalRefCoeff;
		diffuseColor = rhs.diffuseColor;
	}
	return *this;
}

void MaterialPBR::init()
{
	//Kinda hardcoded (each material has to know how uniforms are called on shader)
	uModel = glGetUniformLocation(program, "model_matrix");
	uNormalTransform = glGetUniformLocation(program, "normal_matrix");
	uView = glGetUniformLocation(program, "view_matrix");
	uProjection = glGetUniformLocation(program, "persp_matrix");

	uRoughness = glGetUniformLocation(program, "rgh");
	uPermeability = glGetUniformLocation(program, "perm");
	uNormalRefCoeff = glGetUniformLocation(program, "ks");
	uDiffuseColor = glGetUniformLocation(program, "diffuseColor");

	uLightColors = glGetUniformLocation(program, "lightColor");
	uLightPositions = glGetUniformLocation(program, "lightPosition");
	uEye = glGetUniformLocation(program, "eye");

	uLightTransformation = glGetUniformLocation(program, "lTransformation");
}

void MaterialPBR::passViewAndProj(glm::mat4& V, glm::mat4& P, glm::vec4& eye)
{
	glUseProgram(program);

	glUniformMatrix4fv(uView, 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &P[0][0]);
	glUniform3f(uEye, eye.x, eye.y, eye.z);
}

void MaterialPBR::passLightData(glm::vec4 *lightPositions, glm::vec4 *lightColors, int lightNum)
{
	glUseProgram(program);

	glUniform4fv(uLightPositions, lightNum, (float const *)lightPositions); /*LIGHT NUM IS HARDCODED AS 1*/
	glUniform4fv(uLightColors, lightNum, (float const *)lightColors); /*LIGHT NUM IS HARDCODED AS 1*/
}

void MaterialPBR::sendUniformValues(glm::mat4& M, glm::mat4& NT, glm::mat4 const& lightT)
{
	glUseProgram(program);

	//First pass the shader the transform matrices
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &M[0][0]);
	glUniformMatrix4fv(uNormalTransform, 1, GL_FALSE, &NT[0][0]);
	glUniformMatrix4fv(uLightTransformation, 1, GL_FALSE, &lightT[0][0]);

	//Then, the material gets the specific stuff
	glUniform1f(uRoughness, roughness);
	glUniform1f(uPermeability, permeability);
	glUniform1f(uNormalRefCoeff, normalRefCoeff);
	glUniform3f(uDiffuseColor, diffuseColor.r, diffuseColor.g, diffuseColor.b);
}

void MaterialPBR::bindTexture()
{
	if (texture != 0) 
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		GLint uTexture = glGetUniformLocation(program, "diffuseTexture");
		glUniform1i(uTexture, 0);
	}
}

GLuint MaterialPBR::getProgram() 
{
	return program; 
}