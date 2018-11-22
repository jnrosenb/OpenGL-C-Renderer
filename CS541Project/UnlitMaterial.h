#ifndef UNLITMATERIAL_H
#define UNLITMATERIAL_H

#include "GL/glew.h"
#include "GL/gl.h"
#include <glm/glm.hpp>
#include "Material.h"

class UnlitMaterial : public Material
{

public:
	UnlitMaterial(GLuint prgm, GLuint tex);
	UnlitMaterial(const UnlitMaterial& rhs);
	virtual ~UnlitMaterial();
	UnlitMaterial& operator=(const UnlitMaterial& rhs);

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

	GLuint uEye;
};

#endif