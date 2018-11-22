#ifndef MATERIALTEXTURED_H
#define MATERIALTEXTURED_H

#include "GL/glew.h"
#include "GL/gl.h"
#include <glm/glm.hpp>
#include "Material.h"

class MaterialTextured : public Material
{

public:
	MaterialTextured(GLuint prgm, GLuint tex, float specI, float specP);
	MaterialTextured(const MaterialTextured& rhs);
	virtual ~MaterialTextured();
	MaterialTextured& operator=(const MaterialTextured& rhs);

	virtual void init();
	virtual void sendUniformValues(glm::mat4& M, glm::mat4& NT, glm::mat4 const& lightT);
	virtual void passViewAndProj(glm::mat4& V, glm::mat4& P, glm::vec4& eye);
	virtual void passLightData(glm::vec4 *lightPositions, glm::vec4 *lightColors, int lightNum);

	virtual void bindTexture();

	virtual GLuint getProgram();

private:
	GLuint program;
	GLuint texture;

	float specularIntensity;
	float specularPower;

	GLuint uModel;
	GLuint uNormalTransform;
	GLuint uView;
	GLuint uProjection;
	GLuint uLightTransformation;

	GLuint uSpecularIntensity;
	GLuint uSpecularPower;
	GLuint uLightPositions;
	GLuint uLightColors;
	GLuint uEye;
};

#endif