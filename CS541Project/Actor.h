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

#include <GL/gl.h>
#include <glm/glm.hpp>
#include "Affine.h"
#include "Torus.h"
#include "Sphere.h"
#include "Plane.h"
#include "Mesh3D.h"
#include "Material.h"

using namespace cs541;

namespace cs541 
{
	class Actor 
	{

	public:
		Actor(int animated = 0);
		~Actor();

		//void init(GLuint program, GLuint vao, int faceCount, glm::vec3 diffColor, 
		//	float ks, int specPwr, float roughness, float perm, float normalRefCoeff);
		void init(GLuint vao, int faceCount, Material *mat);
		void setupMatrices(glm::vec4& pos, float angle, glm::vec4& rot, float scale);
		void updateUniformValues(glm::mat4 const& lightTransformation);
		void updateShadowPassUniformValues(GLint shadowProgram);
		void update(float);
		void render();
		void depthRender(GLint ShadowMapProgram);

		//To be able to move them from client
		glm::mat4 R;
		glm::mat4 T;
		glm::mat4 H;

	private:
		float time;
		int id;
		int faceCount;
		int animated;

		GLuint program;
		GLuint vao;
		Material *material;

		//Experiment
		glm::mat4 Model;
		glm::mat4 normalTransformation;
	};
}