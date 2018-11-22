#ifndef POLAR_H
#define POLAR_H

#include "Mesh3D.h"
#include "GLM/glm.hpp"
#include <vector>

class PolarPlane : public Mesh3D 
{
public:
	PolarPlane(int mesh_size);
	//virtual ~PolarPlane();
	
	virtual glm::vec4* vertexArray(void);
	virtual glm::vec4* normalArray(void); 

	virtual glm::vec2* uvArray(void);

	virtual int vertexCount(void) { return vertices.size(); }
	virtual int faceCount(void) { return faces.size(); }

	virtual Face* faceArray(void);

private:
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec4> normals;
	std::vector<Face> faces;
	std::vector<glm::vec2> textcoords;
};

#endif