#include "UnlitMaterial.h"
#include <iostream>

using namespace std;

UnlitMaterial::UnlitMaterial(GLuint prgm, GLuint tex) :
	program(prgm), texture(tex)
{
	cout << "Created new unlit material. (CONV CTOR)" << endl;
	init();
}

UnlitMaterial::UnlitMaterial(const UnlitMaterial& rhs) :
	program(rhs.program), texture(rhs.texture)
{
	cout << "Created new unlit material (CPY CTOR)." << endl;
}

UnlitMaterial::~UnlitMaterial()
{
	/*REMEMBER TO CHECK IF I NEED TO DESTROY BUFFERS OR ANYTHING HERE*/
	cout << "Destroying unlit material (DTOR)." << endl;
}

UnlitMaterial& UnlitMaterial::operator=(const UnlitMaterial& rhs)
{
	if (this != &rhs)
	{
		program = rhs.program;
		texture = rhs.texture;
	}
	return *this;
}

void UnlitMaterial::init()
{
	///Kinda hardcoded (each material has to know how uniforms are called on shader)
	uModel = glGetUniformLocation(program, "M");
	uView = glGetUniformLocation(program, "V");
	uProjection = glGetUniformLocation(program, "P");
}

void UnlitMaterial::passViewAndProj(glm::mat4& V, glm::mat4& P, glm::vec4& eye)
{
	glUseProgram(program);
	glUniformMatrix4fv(uView, 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &P[0][0]);
}

void UnlitMaterial::passLightData(glm::vec4 *lightPositions, glm::vec4 *lightColors, int lightNum)
{ }

void UnlitMaterial::sendUniformValues(glm::mat4& M, glm::mat4& NT, glm::mat4 const& lightT)
{
	glUseProgram(program);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &M[0][0]);
}

void UnlitMaterial::bindTexture()
{ }

GLuint UnlitMaterial::getProgram()
{
	return program;
}