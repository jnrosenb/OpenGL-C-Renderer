#include "MaterialSkybox.h"
#include <iostream>

using namespace std;

MaterialSkybox::MaterialSkybox(GLuint prgm, GLuint tex) :
	program(prgm), texture(tex)
{
	cout << "Created new skybox material. (CONV CTOR)" << endl;

	init();
}

MaterialSkybox::MaterialSkybox(const MaterialSkybox& rhs) :
	program(rhs.program), texture(rhs.texture)
{
	cout << "Created new skybox material (CPY CTOR)." << endl;
}

MaterialSkybox::~MaterialSkybox()
{
	/*REMEMBER TO CHECK IF I NEED TO DESTROY BUFFERS OR ANYTHING HERE*/
	cout << "Destroying skybox material (DTOR)." << endl;
}

MaterialSkybox& MaterialSkybox::operator=(const MaterialSkybox& rhs)
{
	if (this != &rhs)
	{
		program = rhs.program;
		texture = rhs.texture;
	}
	return *this;
}

void MaterialSkybox::init()
{
	///Kinda hardcoded (each material has to know how uniforms are called on shader)
	uModel = glGetUniformLocation(program, "M");
	uNormalTransform = glGetUniformLocation(program, "NT");
	uView = glGetUniformLocation(program, "V");
	uProjection = glGetUniformLocation(program, "P");
}

void MaterialSkybox::passViewAndProj(glm::mat4& V, glm::mat4& P, glm::vec4& eye)
{
	glUseProgram(program);

	glUniformMatrix4fv(uView, 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &P[0][0]);
}

void MaterialSkybox::passLightData(glm::vec4 *lightPositions, glm::vec4 *lightColors, int lightNum)
{
}

void MaterialSkybox::sendUniformValues(glm::mat4& M, glm::mat4& NT, glm::mat4 const& lightT)
{
	glUseProgram(program);
	
	///First pass the shader the transform matrices
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &M[0][0]);
	glUniformMatrix4fv(uNormalTransform, 1, GL_FALSE, &NT[0][0]);
}

void MaterialSkybox::bindTexture()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	///TODO check for errors (GLERROR)
}

GLuint MaterialSkybox::getProgram()
{
	return program;
}