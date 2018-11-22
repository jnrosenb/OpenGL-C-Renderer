#ifndef MATERIALPBR_H
#define MATERIALPBR_H

#include "GL/glew.h"
#include "GL/gl.h"
#include "Material.h"

class MaterialPBR : public Material
{
public:
	MaterialPBR(GLuint prgm, GLuint text, float rghss, float perm, float nrc, glm::vec3 diffuseClr);
	MaterialPBR(const MaterialPBR& rhs);
	virtual ~MaterialPBR();
	MaterialPBR& operator=(const MaterialPBR& rhs);

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
	GLuint uLightTransformation;

	GLuint uLightPositions;
	GLuint uLightColors;
	GLuint uEye;

	GLuint uRoughness;
	GLuint uPermeability;
	GLuint uNormalRefCoeff;
	GLuint uDiffuseColor;

	float roughness;
	float permeability;
	float normalRefCoeff;
	glm::vec3 diffuseColor;
};

#endif