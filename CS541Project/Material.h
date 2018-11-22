#ifndef MATERIAL_H
#define MATERIAL_H

#include "GL/glew.h"
#include "GL/gl.h"
#include "glm/glm.hpp"

class Material 
{
public:
	Material() {}
	Material(const Material& rhs) {}
	virtual ~Material() {}
	Material& operator=(const Material& rhs) {}

	virtual void init() = 0; 
	virtual void sendUniformValues(glm::mat4& M, glm::mat4& NT, glm::mat4 const& lightT) = 0;
	virtual void passViewAndProj(glm::mat4& V, glm::mat4& P, glm::vec4& eye) = 0;
	virtual void passLightData(glm::vec4 *lightPositions, glm::vec4 *lightColors, int lightNum) = 0;

	virtual void bindTexture() {}

	virtual GLuint getProgram() = 0;

private:
	GLuint program;
};

#endif