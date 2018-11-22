//////////////////////////////////////////
// HEADER								//
//										//
// Name: Jose Rosenbluth Chiu			//
// Course Number: CS541					//
// Programming Assignment number: #2	//
// Task #1								//
// Due date: 10/04/2018					//
//										//
//////////////////////////////////////////

#include <cmath>
#include "Camera.h"
#include "Affine.h"
#include <iostream>

namespace cs541
{
	const float epsilon = 0.000001f;

	glm::mat4 view(const glm::vec4 &eye, const glm::vec4 &look, const glm::vec4 &rup) 
	{
		glm::vec4 right, up, u, v, n;
		
		right = cs541::cross(look, rup);
		up = cs541::cross(right, look);

		float rightLength = sqrt(pow(right.x, 2) + pow(right.y, 2) + pow(right.z, 2));
		float upLength = sqrt(pow(up.x, 2) + pow(up.y, 2) + pow(up.z, 2));
		float lookLength = sqrt(pow(look.x, 2) + pow(look.y, 2) + pow(look.z, 2));
		if (fabs(rightLength) <= epsilon || fabs(upLength) <= epsilon || fabs(lookLength) <= epsilon)
		{
			//ERROR, up or right are zero
			std::cout << "Error in view method. Look or rup have zero length" << std::endl;
			return glm::mat4();
		}
		u = {right.x/rightLength, right.y/rightLength, right.z/rightLength, 0};
		v = {up.x/upLength, up.y/upLength, up.z/upLength, 0};
		n = {-look.x/lookLength, -look.y/lookLength, -look.z/lookLength, 0};
		
		/*
		if (fabs(u.x) <= epsilon && fabs(u.y) <= epsilon && fabs(u.z) <= epsilon) 
		{
			//TODO
			std::cout << "ERROR when camera looks up, undefined" << std::endl;
		}
		else 
		{
			v = cs541::cross(n, u);
		}//*/

		glm::vec3 LtP = {
			-(u.x * eye.x) - (u.y * eye.y) - (u.z * eye.z),
			-(v.x * eye.x) - (v.y * eye.y) - (v.z * eye.z),
			-(n.x * eye.x) - (n.y * eye.y) - (n.z * eye.z),
		};

		return {u.x, v.x, n.x, 0,
				u.y, v.y, n.y, 0,
				u.z, v.z, n.z, 0,
				LtP.x, LtP.y, LtP.z, 1};
	}

	glm::mat4 perspective(float fov, float aspect, float near)
	{
		if (fabs(near) <= epsilon || fabs(aspect) <= epsilon)
		{
			//TODO see what to return in these cases
			std::cout << "Error in perspective method." << std::endl;
			return glm::mat4();
		}

		float PI = 4.0f * atan(1.0f);
		float radians = (fov * PI) / 180.0f;
		float W = 2 * near * (float)tan(radians / 2.0f);
		float H = W / aspect;

		float A = -1;
		float B = -2 * near;
		float C = (2 * near) / W;
		float D = (2 * near) / H;
		glm::mat4 persp = {
			C, 0, 0, 0,
			0, D, 0, 0,
			0, 0, A, -1,
			0, 0, B, 0
		};

		return persp;
	}
	

	glm::mat4 perspective(float fov, float aspect, float near, float far)
	{
		float epsilon = 0.0000001f;
		if (fabs(near) <= epsilon || fabs(aspect) <= epsilon || fabs(near - far) <= epsilon) 
		{
			//TODO see what to return in these cases
			std::cout << "Error in perspective method." << std::endl;
			return glm::mat4();
		}

		float PI = 4.0f * atan(1.0f);
		float radians = (fov * PI) / 180.0f;
		float W = 2 * near * (float)tan(radians / 2.0f);
		float H = W / aspect;

		float A = (near + far) / (near - far);
		float B = (2 * near * far) / (near - far);
		float C = (2 * near) / W;
		float D = (2 * near) / H;
		glm::mat4 persp = {
			C, 0, 0, 0,
			0, D, 0, 0,
			0, 0, A, -1,
			0, 0, B, 0
		};

		return persp;
	}

	
	glm::mat4 orthographic(float width, float height, float aspect, float near, float far)
	{
		float epsilon = 0.0000001f;
		if (fabs(near) <= epsilon || fabs(aspect) <= epsilon || fabs(near - far) <= epsilon) 
		{
			//TODO see what to return in these cases
			std::cout << "Error in perspective method." << std::endl;
			return glm::mat4();
		}

		float W = width;
		float H = height;

		float A = (near + far) / (near - far);
		float B = (2) / (near - far);
		float C = (2) / W;
		float D = (2) / H;
		glm::mat4 orthographic = {
			C, 0, 0, 0,
			0, D, 0, 0,
			0, 0, A, 0,
			0, 0, B, 1
		};

		return orthographic;
	}
}