//////////////////////////////////////////
// HEADER								//
//										//
// Name: Jose Rosenbluth Chiu			//
// Course Number: CS541					//
// Programming Assignment number: #1	//
// Task #1								//
// Due date: 09/20/2018					//
//										//
//////////////////////////////////////////

#include <iostream>
#include <cmath>
#include "Affine.h"

namespace cs541
{
	glm::mat4 scale(float r)
	{
		glm::mat4 result = { r, 0, 0, 0, 0, r, 0, 0, 0, 0, r, 0, 0, 0, 0, 1 };
		return result;
	}

	glm::mat4 scale(float rx, float ry, float rz)
	{
		glm::mat4 result = { rx, 0, 0, 0, 0, ry, 0, 0, 0, 0, rz, 0, 0, 0, 0, 1 };
		return result;
	}

	glm::mat4 translate(const glm::vec4& v)
	{
		glm::mat4 result = { 1, 0, 0, 0, 
							 0, 1, 0, 0, 
							 0, 0, 1, 0, 
							 v.x, v.y, v.z, 1 };
		return result;
	}

	glm::mat4 rotate(float t, const glm::vec4& v)
	{
		//check if angle is zero or if axis is zero vector
		if (t == 0 || v == glm::vec4{ 0, 0, 0, 0 })
			return glm::mat4{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

		//turn degrees into rads
		float PI = 4.0f * atan(1.0f);
		float rads = (PI * t) / 180.0f;

		//Calculations to derive the rotation matrix
		glm::mat3 I = { 1, 0, 0, 0, 1, 0, 0, 0, 1};
		glm::mat3 VVT =
		{
			v.x*v.x, v.x*v.y, v.x*v.z,
			v.y*v.x, v.y*v.y, v.y*v.z,
			v.z*v.x, v.z*v.y, v.z*v.z
		};
		glm::mat3 CrossProductM =
		{
			 0,   -v.z,  v.y,
			 v.z,  0,   -v.x,
			-v.y,  v.x,  0,
		};

		glm::mat3 R = cos(rads) * I + ((1 - cos(rads)) / (v.x*v.x + v.y*v.y + v.z*v.z)) * VVT + (sin(rads)/(sqrt((v.x*v.x + v.y*v.y + v.z*v.z)))) * CrossProductM;
		return 
		{
			R[0][0], R[1][0], R[2][0], 0,
			R[0][1], R[1][1], R[2][1], 0,
			R[0][2], R[1][2], R[2][2], 0,
			0, 0, 0, 1,
		};
	}

	glm::mat4 transpose3x3(const glm::mat4& A)
	{
		glm::mat4 B;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (i == 3 || j == 3)
					B[i][j] = A[i][j];
				else
					B[i][j] = A[j][i];
			}
		}

		return B;
	}

	glm::vec4 cross(const glm::vec4 &u, const glm::vec4 &v)
	{
		glm::vec4 result =
		{
			u.y*v.z - u.z*v.y,
			u.z*v.x - u.x*v.z,
			u.x*v.y - u.y*v.x,
			0
		};
		return result;
	}
}