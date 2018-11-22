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

#ifndef MATERIALSKYBOX_H
#define MATERIALSKYBOX_H

#include "GL/glew.h"
#include "GL/gl.h"
#include <glm/glm.hpp>
#include "Material.h"

class MaterialSkybox : public Material
{

public:
	MaterialSkybox(GLuint prgm, GLuint tex);
	MaterialSkybox(const MaterialSkybox& rhs);
	virtual ~MaterialSkybox();
	MaterialSkybox& operator=(const MaterialSkybox& rhs);

	virtual void init();
	virtual void sendUniformValues(glm::mat4& M, glm::mat4& NT, glm::mat4 const& lightT);
	virtual void passViewAndProj(glm::mat4& V, glm::mat4& P, glm::vec4& eye);
	virtual void passLightData(glm::vec4 *lightPositions, glm::vec4 *lightColors, int lightNum);

	virtual void bindTexture();

	virtual GLuint getProgram();

private:
	GLuint program;
	GLuint texture;

	GLuint uModel;
	GLuint uNormalTransform;
	GLuint uView;
	GLuint uProjection;
};

#endif