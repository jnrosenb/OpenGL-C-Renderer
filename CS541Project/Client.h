//////////////////////////////////////////
// HEADER								//
//										//
// Name: Jose Rosenbluth Chiu			//
// Course Number: CS541					//
// Programming Assignment number: #3	//
// Task #2								//
// Due date: 10/25/2018       			//
//                  					//
//////////////////////////////////////////

#include <SDL2/SDL.h>
#include <string>
#include <algorithm>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include "Actor.h"
#include "Torus.h"
#include "Sphere.h"
#include "Plane.h"
#include "Polar.h"
#include <vector>

namespace cs541
{
	#define MAX_ACTORS 512
	#define MAX_MATERIALS 16
	#define NUM_TEXTURES 16
	#define NUM_MESHES 4
	#define LIGHT_COUNT 1

	//GLOBAL ILLUMINATION VARS
	static glm::vec4 light1[LIGHT_COUNT];// = { { 0.0f, 20.0f, 10.0f, 1.0f } };// , { -30.0f, 20.0f, 20.0f, 1.0f }, { 20.0f, 30.0f, -40.0f, 1.0f } };
	static glm::vec4 lightLookAt[LIGHT_COUNT];
	static glm::vec4 light1Color[LIGHT_COUNT];// = { { 1.0f, 1.0f, 1.0f, 1.0f } };//,{ 1.0f, 1.0f, 1.0f, 1.0f } ,{ 1.0f, 1.0f, 1.0f, 1.0f } };
	static glm::vec4 eye;
	static glm::vec4 look;
	static glm::mat4 V;
	static glm::mat4 P;

	class Client
	{

	public:
		Client(int width, int height);
		~Client(void);
		void draw(double dt);
		void createActors();
		void createMaterials();
		void keypress(SDL_Keycode kc);
		void resize(int W, int H);
		void mousedrag(int x, int y, bool lbutton);
		void setupShadowFrameBuffer();
		void setupShadowMapMatrices(glm::vec4 const& lightPos, 
			glm::vec4 const& lightLook);
		void renderShadowMap();

	private:
		std::string loadFile(const char *path);
		unsigned char *loadBitmapFile(const char *fname, int *width, int *height);
		void loadTextureAndPassToGPU(const char *path, bool tiled);
		void loadCubemapTextures(std::vector<char const *> faces);
		void initShaders(const char* vertexPath, const char *fragmentPath, GLint *program);
		void initMeshGlVAO(Mesh3D *mesh);
		void CubeVAOSetup();
		void RenderLightDebugDir();

	private:
		int width, height;

		Torus *torus;
		Sphere *sphere;
		Plane *plane;
		PolarPlane *polarPlane;

		Actor *actors[MAX_ACTORS];
		Actor *cubemapPtr;
		Actor *unlitSphere;
		Material *materials[MAX_MATERIALS];

		//Shadow map variables
		GLuint shadowFrameBuffer;
		GLuint shadowFBTexture;
		GLuint uLightTransformation;
		glm::mat4 lightTransform;

		GLuint VertexArrayObjects[NUM_MESHES];
 		GLuint vertexBufferName[NUM_MESHES];
		GLuint normalBufferName[NUM_MESHES];
		GLuint uvBufferName[NUM_MESHES];
		GLuint faceBufferName[NUM_MESHES];
		GLuint textureBufferName[NUM_TEXTURES];
		GLuint cubeVAO, cubeVBO, cubeEBO;

		GLint PBRProgram;
		GLint PBRTexturedProgram;
		GLint TexturedProgram;
		GLint SkyboxProgram;
		GLint ShadowMapProgram;
		GLint UnlitProgram;

		GLuint uLight_count;

		int torus_face_count;
		int sphere_face_count; 
		int plane_face_count;

		float time;
		float aspect;
		int ActorCount;
		int MaterialCount;
		int TextureCount;
		int MeshCount;
	};
}