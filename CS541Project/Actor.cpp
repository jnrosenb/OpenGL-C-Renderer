//////////////////////////////////////////
// HEADER								//
//										//
// Name: Jose Rosenbluth Chiu			//
// Course Number: CS541					//
// Programming Assignment number: #3	//
// Task #2								//
// Due date: 10/25/2018       			//
//                  					//
//////////////////////////////////////////

#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include "Actor.h"

using namespace std;

namespace cs541 
{
	int ID = 0;

	// This will return the inverse for a non uniform or uniformly
	// scaled matrix
	glm::mat4 inverseH(const glm::mat4 &A)
	{
		glm::mat4 B(0.0f);

		B[0][0] = 1 / A[0][0];
		B[1][1] = 1 / A[1][1];
		B[2][2] = 1 / A[2][2];
		B[3][3] = 1;

		return B;
	}


	Actor::Actor(int animated) : time(0.0f), id(ID), animated(animated)
	{
		++ID;
		material = 0;
	}


	Actor::~Actor() 
	{
	}


	/* This guy should not handle the shader params in the future */
	//void Actor::init(GLuint program, GLuint vao, int faceCount, glm::vec3 diffColor, float ks, int specPwr, float roughness, float perm, float normalRefCoeff)
	void Actor::init(GLuint vao, int faceCount, Material *mat)
	{
		this->vao = vao;
		this->faceCount = faceCount;
		this->material = mat;
	}


	void Actor::setupMatrices(glm::vec4& initPos, float angle, glm::vec4& rotAxis, float initScale)
	{
		this->H = cs541::scale(initScale);
		this->R = cs541::rotate(angle, rotAxis);
		this->T = cs541::translate(initPos);
	}


	void Actor::updateUniformValues(glm::mat4 const& lightTransformation)
	{
		Model = T * R * H;
		normalTransformation = R * cs541::transpose3x3(inverseH(H));

		this->material->sendUniformValues(Model, normalTransformation, lightTransformation); /*GET V AND P*/
	}


	void Actor::updateShadowPassUniformValues(GLint shadowProgram)
	{
		Model = T * R * H;

		//TODO: CHANGE LATER, INNEFICIENT
		GLuint umodel = glGetUniformLocation(shadowProgram, "M");

		//Use program and pass data to gpu
		glUseProgram(shadowProgram);
		glUniformMatrix4fv(umodel, 1, GL_FALSE, &Model[0][0]);
	}


	void Actor::update(float dt) 
	{
		time += dt;

		if (animated == 1)
		{
			this->R = cs541::rotate(360 * sin(time / 2.0f), { 0, 1, 0, 0 });
		}
		if (animated == 2)
		{
			this->R = cs541::rotate(360 * sin(time / 2.0f), { 0, 0, 1, 0 });
		}
	}


	void Actor::render()
	{
		unsigned int prgm = material->getProgram();
		glUseProgram(prgm);

		//cout << "FACE COUNT: " << faceCount << endl;
		glBindVertexArray(this->vao);

		//bind the texture
		this->material->bindTexture();

		glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_INT, 0);
		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
		{
			switch (err)
			{
			case GL_INVALID_ENUM:
				std::cout << "ERROR: GL_INVALID_ENUM" << std::endl;
				break;
			case GL_INVALID_VALUE:
				std::cout << "ERROR: GL_INVALID_VALUE -. Program: " << prgm << std::endl;
				break;
			case GL_INVALID_OPERATION:
				std::cout << "ERROR: GL_INVALID_OPERATION" << std::endl;
				break;
			default:
				std::cout << "ERROR: None of the previous" << std::endl;
				break;
			}
		}

		glBindVertexArray(0);
	}


	void Actor::depthRender(GLint ShadowMapProgram)
	{
		glUseProgram(ShadowMapProgram);
		glBindVertexArray(this->vao);
		glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_INT, 0);
		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
		{
			switch (err)
			{
			case GL_INVALID_ENUM:
				std::cout << "(Actor::depthRender)- ERROR: GL_INVALID_ENUM" << std::endl;
				break;
			case GL_INVALID_VALUE:
				std::cout << "(Actor::depthRender)- ERROR: GL_INVALID_VALUE. Program: " << ShadowMapProgram << std::endl;
				break;
			case GL_INVALID_OPERATION:
				std::cout << "(Actor::depthRender)- ERROR: GL_INVALID_OPERATION" << std::endl;
				break;
			default:
				std::cout << "(Actor::depthRender)- ERROR: None of the previous" << std::endl;
				break;
			}
		}
		glBindVertexArray(0);
	}
}