#include "Polar.h"
#include <iostream>
#include "Affine.h"

#define PI 3.14159265359

PolarPlane::PolarPlane(int mesh_size) :
	vertices((mesh_size + 1)*(mesh_size + 1)), normals(vertices.size()), 
	textcoords(vertices.size()), faces(2 * mesh_size*mesh_size)
{
	std::cout << "Calling PolarPlane Ctor" << std::endl;

	//Vertices initial setting
	float d = 2.0f / mesh_size;
	for (int j = 0; j <= mesh_size; ++j)
	{
		float y = j * d - 1.0f;
		for (int i = 0; i <= mesh_size; ++i)
		{
			float x = i * d - 1.0f;
			int index = (mesh_size + 1)*j + i;
			vertices[index].x = x;
			vertices[index].y = y;
			vertices[index].z = 0;
			vertices[index].w = 1;
		}
	}

	//Faces setup
	for (int n = 0, j = 0; j < mesh_size; ++j)
	{		
		for (int i = 0; i < mesh_size; ++i)
		{
			faces[n][0] = (mesh_size + 1)*j + i;
			faces[n][1] = (mesh_size + 1)*j + i + 1;
			faces[n][2] = (mesh_size + 1)*(j + 1) + i + 1;
			++n;
			faces[n][0] = (mesh_size + 1)*j + i;
			faces[n][1] = (mesh_size + 1)*(j + 1) + i + 1;
			faces[n][2] = (mesh_size + 1)*(j + 1) + i;
			++n;
		}
	}
	
	//Set UV coords
	int vertexNum = vertices.size();
	glm::mat4 Std2Unit = cs541::scale(0.5f, 0.5f, 1) * cs541::translate(glm::vec4(1, 1, 0, 0));
	for (int i = 0; i < vertexCount(); ++i)
	{
		glm::vec4 uv = Std2Unit * vertices[i];
		textcoords[i].x = uv[0];
		textcoords[i].y = uv[1];
	}

	//Transform vertices
	glm::mat4 Std2Plane = cs541::scale(PI, PI / 2.0f, 1) * cs541::translate(glm::vec4(1, 1, 0, 0)) * cs541::scale(1, -1, 1);
	for (int i = 0; i < vertexNum; ++i)
	{
		glm::vec4 tempVertex = Std2Plane * vertices[i];
		glm::vec4 newVertex;
		newVertex.x = 1 * sin(tempVertex.y) * sin(tempVertex.x);
		newVertex.y = 1 * cos(tempVertex.y);
		newVertex.z = 1 * sin(tempVertex.y) * cos(tempVertex.x);
		newVertex.w = 1;
		vertices[i] = newVertex;
	}

	//Transform normals
	for (int i = 0; i < vertexNum; ++i)
	{
		glm::vec4 newNormals = glm::normalize(vertices[i]);
		normals[i] = newNormals;
	}
}

glm::vec4* PolarPlane::vertexArray(void)
{
	return &(vertices[0]);
}

glm::vec2* PolarPlane::uvArray(void)
{
	return &(textcoords[0]);
}

glm::vec4* PolarPlane::normalArray(void)
{
	return &(normals[0]);
}

Mesh3D::Face* PolarPlane::faceArray(void) {
	return &(faces[0]);
}