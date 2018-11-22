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

#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Affine.h"
#include "Client.h"
#include <cstdlib>
#include "Camera.h"
#include "MaterialTextured.h"
#include "MaterialPBR.h"
#include "MaterialSkybox.h"
#include "UnlitMaterial.h"

using namespace std;

namespace cs541
{
	//GLOBAL CAMERA VARS
	float fovDivider = 4.0f;
	float FOV = 180.0f / fovDivider;
	float AR = 1.0f;

	//GLOBAL vars
	float rgh = 0.5f;
	float ks = 0.23f;

	//CONSTANTS
	const int MESH_SIZE = 64;
	const float THICKNESS = 0.5f;
	const float PI = 4.0f*atan(1.0f);
	const glm::vec4 Or = { 0, 0, 0, 1 };
	const glm::vec4 Ex = { 1, 0, 0, 0 };
	const glm::vec4 Ey = { 0, 1, 0, 0 };
	const glm::vec4 Ez = { 0, 0, 1, 0 };

	//ShadowMap globals
	float lightFov = 90.0f;
	float lightAR = 1.0f;

	Client::Client(int width, int height)
	{
		//Init pointer to null
		cubemapPtr = NULL;
		unlitSphere = NULL;

		//Set windows size params
		this->width = width;
		this->height = height;

		// Set number of elements to draw
		ActorCount = 0;
		MeshCount = 0;
		MaterialCount = 0;
		TextureCount = 0;

		// Create shader program
		PBRProgram = glCreateProgram();
		TexturedProgram = glCreateProgram();
		SkyboxProgram = glCreateProgram();
		PBRTexturedProgram = glCreateProgram();
		ShadowMapProgram = glCreateProgram();
		UnlitProgram = glCreateProgram();

		// Initialize the shaders and link them into program
		initShaders("PBR.vert", "PBR.frag", &PBRProgram);
		initShaders("PBR.vert", "PBRTextured.frag", &PBRTexturedProgram);
		initShaders("Texture.vert", "Texture.frag", &TexturedProgram);
		initShaders("Skybox.vert", "Skybox.frag", &SkyboxProgram);
		initShaders("ShadowMap.vert", "ShadowMap.frag", &ShadowMapProgram);
		initShaders("Unlit.vert", "Unlit.frag", &UnlitProgram);

		// Uniforms locations (shadowmap)
		uLightTransformation = glGetUniformLocation(ShadowMapProgram, "lightT");

		// Generate the VAO, VBO, and EBO
		glGenVertexArrays(NUM_MESHES, VertexArrayObjects);
		glGenBuffers(NUM_MESHES, vertexBufferName);
		glGenBuffers(NUM_MESHES, normalBufferName);
		glGenBuffers(NUM_MESHES, faceBufferName);
		glGenBuffers(NUM_MESHES, uvBufferName);
		glGenTextures(NUM_TEXTURES, textureBufferName);

		//Upload textures
		loadTextureAndPassToGPU("cube5.bmp", false);
		loadTextureAndPassToGPU("cube1.bmp", true);
		loadTextureAndPassToGPU("cube2.bmp", false);
		loadTextureAndPassToGPU("spherical.bmp", false);
		loadTextureAndPassToGPU("brick.bmp", true);
		//CUBEMAP TEXTURES AND MESH (VAO)
		std::vector<char const*> faces = { "cube0.bmp", "cube1.bmp", "cube2.bmp", "cube3.bmp", "cube4.bmp", "cube5.bmp" };
		loadCubemapTextures(faces);
		CubeVAOSetup();
		//CUBEMAP TEXTURES AND MESH (VAO)
		loadTextureAndPassToGPU("brick.bmp", false);
		loadTextureAndPassToGPU("grass.bmp", true);

		// Upload torus mesh to GPU
		cout << "Initializing torus" << endl;
		torus = new Torus(THICKNESS, MESH_SIZE);
		initMeshGlVAO(torus);

		// Upload sphere mesh to GPU
		cout << "Initializing sphere" << endl;
		sphere = new Sphere(MESH_SIZE);
		initMeshGlVAO(sphere);

		// Upload plane mesh to GPU
		cout << "Initializing plane" << endl;
		plane = new Plane(MESH_SIZE);
		initMeshGlVAO(plane);

		// Upload plane mesh to GPU
		cout << "Initializing polar plane" << endl;
		polarPlane = new PolarPlane(MESH_SIZE);
		initMeshGlVAO(polarPlane);

		// Enable use of z-buffer
		glEnable(GL_DEPTH_TEST);
		//TODO disable backface
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		//Setup shadow mapping
		setupShadowFrameBuffer();

		aspect = 1.0f;
		time = 0.0f;

		//Create the materials and actors to populate scene
		createMaterials();
		createActors();

		cout << "Initialization Complete!" << endl;
	}


	Client::~Client(void)
	{
		// delete the shader program
		glUseProgram(0);
		glDeleteProgram(PBRProgram);
		glDeleteProgram(PBRTexturedProgram);
		glDeleteProgram(TexturedProgram);
		glDeleteProgram(SkyboxProgram);
		glDeleteProgram(ShadowMapProgram);
		glDeleteProgram(UnlitProgram);

		// delete the vertex, normal, and face buffers
		glDeleteVertexArrays(NUM_MESHES, VertexArrayObjects);
		glDeleteBuffers(NUM_MESHES, vertexBufferName);
		glDeleteBuffers(NUM_MESHES, normalBufferName);
		glDeleteBuffers(NUM_MESHES, faceBufferName);
		glDeleteBuffers(NUM_MESHES, uvBufferName);
		glDeleteBuffers(NUM_TEXTURES, textureBufferName);

		//Shadow map stuff
		glDeleteTextures(1, &shadowFBTexture);
		glDeleteFramebuffers(1, &shadowFrameBuffer);

		//Delete the Cubemap buffers
		glDeleteVertexArrays(1, &cubeVAO);
		glDeleteVertexArrays(1, &cubeVBO);
		glDeleteVertexArrays(1, &cubeEBO);

		//Delete the new pointers
		if (torus)
			delete torus;
		if (sphere)
			delete sphere;
		if (plane)
			delete plane;
		if (polarPlane)
			delete polarPlane;

		for (int i = 0; i < ActorCount; i++)
		{
			delete actors[i];
			cout << "Deleted actor" << endl;
		}
		
		for (int i = 0; i < MaterialCount; i++)
		{
			delete materials[i];
			cout << "Deleted Material" << endl;
		}

		cout << "Destructor finished!" << endl;
	}


	//Ran once per frame. Input delta time
	void Client::draw(double dt)
	{
		///DEBUG ONLY
		light1[0] = eye + 1.0f * look;
		lightLookAt[0] = { look.x, look.y, look.z, 1.0f };
		unlitSphere->T = cs541::translate(light1[0]);
		////DEBUG ONLY//*/

		//View and Perspective transforms
		V = cs541::view(eye, look, Ey);
		P = cs541::perspective(FOV, AR, 0.5f);

		//Update and render every actor
		for (int i = 0; i < ActorCount; i++)
		{
			Actor *actor = actors[i];
			actor->update(static_cast<float>(dt));
		}

		/////////////////////////////////////////
		////  FIRST PASS				/////////
		/////////////////////////////////////////
		glViewport(0, 0, 1024, 1024);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);

		///Use the shadow map shader
		glUseProgram(ShadowMapProgram);

		///Prepare shadow map and draw
		glClear(GL_DEPTH_BUFFER_BIT);

		setupShadowMapMatrices(light1[0], lightLookAt[0]);
		renderShadowMap();

		///Back to original settings
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);

		/////////////////////////////////////////
		////  SECOND PASS				/////////
		/////////////////////////////////////////

		// clear frame buffer and z-buffer
		glClearColor(0.8f, 0.8f, 0.8f, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearDepth(1);
		glClear(GL_DEPTH_BUFFER_BIT);

		//Bind the shadowmap as texture1
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, shadowFBTexture);
		GLint uShadowMap_1 = glGetUniformLocation(TexturedProgram, "shadowTexture");
		GLint uShadowMap_2 = glGetUniformLocation(PBRProgram, "shadowTexture");
		glUseProgram(TexturedProgram);
		glUniform1i(uShadowMap_1, 1); 
		glUseProgram(PBRProgram);
		glUniform1i(uShadowMap_2, 1);
		glUseProgram(0);

		//For every material, give them the view and proj 
		//(and all the info that is common to every instance)
		for (int i = 0; i < MaterialCount; ++i) 
		{
			Material *m = materials[i];
			if (m != 0) 
			{
				m->passViewAndProj(V, P, eye);
				m->passLightData(light1, light1Color, 1);
			}
		}

		//Update skybox so it moves with camera
		if (cubemapPtr) 
		{
			cubemapPtr->T = cs541::translate(eye);
		}

		//Render InlitSphere's light direction
		RenderLightDebugDir();

		//Update and render every actor
		for (int i = 0; i < ActorCount; i++)
		{
			Actor *actor = actors[i];
			actor->updateUniformValues(lightTransform);
			if (i == ActorCount - 1) 
			{
				glDepthFunc(GL_EQUAL);
				actor->render();
				glDepthFunc(GL_LESS);
			}
			else 
			{
				actor->render();
			}
		}
		
		time += (float)dt;
	}


	void Client::setupShadowFrameBuffer() 
	{
		//Generate and bind
		glGenFramebuffers(1, &shadowFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);

		//gen texture that we'll use for frame buffer
		glGenTextures(1, &shadowFBTexture);
		glBindTexture(GL_TEXTURE_2D, shadowFBTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		//float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		//Just to be sure
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);

		///Attach this texture to the new framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowFBTexture, 0);
		glReadBuffer(GL_NONE); 
		glDrawBuffer(GL_NONE);

		//Check everything went ok
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
		{
			std::cout << "\nERROR WHEN SETTING UP DEPTH FRAME BUFFER\n" << std::endl;
		}

		//Bind back to main framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	
	void Client::setupShadowMapMatrices(glm::vec4 const& lightPos, glm::vec4 const& lightLook)
	{
		///Get the light's view matrix
		glm::mat4 lView = cs541::view(lightPos, lightLook, Ey);
		
		float near_plane = 0.1f, far_plane = 1000.0f;
		//glm::mat4 lightProjection = glm::perspective(PI/2.0f, 1.0f, near_plane, far_plane);
		glm::mat4 lightProjection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, near_plane, far_plane);

		///Get the light's projection matrix (perspective for now)
		///TODO: FIX ORTHO IN CS541 *******************************
		//glm::mat4 lProj = cs541::orthographic(300, 300, lightAR, 0.1f, 1000.0f);
		//glm::mat4 lProj = cs541::perspective(lightFov, lightAR, 0.1f, 1000.0f);
		
		///Shadow transformation matrix
		lightTransform = lightProjection * lView;
		//lightTransform = lProj * lView;

		///Pass them to gpu		
		glUseProgram(ShadowMapProgram);
		glUniformMatrix4fv(uLightTransformation, 1, GL_FALSE, &lightTransform[0][0]);
	}


	void Client::renderShadowMap()
	{
		//Update and render every actor except for skybox
		for (int i = 0; i < ActorCount - 1; i++)
		{
			Actor *actor = actors[i];

			if (actor == cubemapPtr || actor == unlitSphere)
				continue;

			actor->updateShadowPassUniformValues(ShadowMapProgram);
			actor->depthRender(ShadowMapProgram);
		}
	}


	void Client::createActors() 
	{
		cout << "Starting to create Actors!" << endl;

		/////////////////////////////////////////////////////////
		////                ACTORS SETUP 					/////
		////   TORUS 0, SPHERE 1, PLANE 2, POLAR SPHERE 3	/////
		/////////////////////////////////////////////////////////

		//Create an actor with sphere mesh --- UNLIT LIGHT ---
		actors[ActorCount] = new Actor(0);
		unlitSphere = actors[ActorCount];
		actors[ActorCount]->init(VertexArrayObjects[3], sphere->faceCount(), materials[8]);
		glm::vec4 unlitpos = light1[0];
		glm::vec4 unlitrotAxis = { 1.0f, 0.0f, 0.0f, 0.0f };
		actors[ActorCount]->setupMatrices(unlitpos, 0.0f, unlitrotAxis, 2.0f);
		++ActorCount;//*/

		//Create an actor with plane mesh --- POLAR SPHERE PLANE ---
		actors[ActorCount] = new Actor(0);
		actors[ActorCount]->init(VertexArrayObjects[3], plane->faceCount(), materials[0]);
		glm::vec4 pos = { 0.0f, 10.0f, -30.0f, 1.0f };
		glm::vec4 rotAxis = { 1.0f, 0.0f, 0.0f, 0.0f };
		actors[ActorCount]->setupMatrices(pos, 0.0f, rotAxis, 10.0f);
		++ActorCount;//*/

		//Create an actor with plane mesh --- FLOOR PLANE ---
		actors[ActorCount] = new Actor(0);
		actors[ActorCount]->init(VertexArrayObjects[2], plane->faceCount(), materials[7]);
		glm::vec4 pos2 = { 0.0f, -10.0f, 0.0f, 1.0f };
		glm::vec4 rotAxis2 = { 1.0f, 0.0f, 0.0f, 0.0f };
		actors[ActorCount]->setupMatrices(pos2, -90.0f, rotAxis2, 400.0f);
		++ActorCount;

		//Create an actor with torus mesh for light, passes a 1 for animation
		actors[ActorCount] = new Actor(1);
		//actors[ActorCount]->init(VertexArrayObjects[0], torus->faceCount(), materials[6]);
		actors[ActorCount]->init(VertexArrayObjects[0], torus->faceCount(), materials[2]);
		glm::vec4 pos3 = { 20.0f, 10.0f, -20.0f, 1.0f };
		glm::vec4 rotAxis3 = { 0.0f, 0.0f, 1.0f, 0.0f };
		actors[ActorCount]->setupMatrices(pos3, 60.0f, rotAxis3, 5.0f);
		++ActorCount;

		//Create an actor with torus mesh for light, passes a 1 for animation
		actors[ActorCount] = new Actor(1);
		actors[ActorCount]->init(VertexArrayObjects[0], torus->faceCount(), materials[3]);
		//actors[ActorCount]->init(VertexArrayObjects[0], torus->faceCount(), materials[6]);
		glm::vec4 pos5 = { 10.0f, -8.0f, -40.0f, 1.0f };
		glm::vec4 rotAxis5 = { 0.0f, 0.0f, 0.0f, 0.0f };
		actors[ActorCount]->setupMatrices(pos5, 0.0f, rotAxis5, 15.0f);
		++ActorCount;//*/

		//Create an actor with torus mesh for light, passes a 1 for animation
		actors[ActorCount] = new Actor(1);
		//actors[ActorCount]->init(VertexArrayObjects[0], torus->faceCount(), materials[9]);
		actors[ActorCount]->init(VertexArrayObjects[0], torus->faceCount(), materials[4]);
		glm::vec4 pos4 = { -20.0f, -10.0f, -20.0f, 1.0f };
		glm::vec4 rotAxis4 = { 0.0f, 0.0f, 1.0f, 0.0f };
		actors[ActorCount]->setupMatrices(pos4, 0.0f, rotAxis4, 10.0f);
		++ActorCount;

		//Create an actor with plane mesh WALLS (leave material index 9)
		actors[ActorCount] = new Actor(0);
		actors[ActorCount]->init(VertexArrayObjects[2], plane->faceCount(), materials[9]);
		glm::vec4 pos9 = { 0.0f, 20.0f, -50.0f, 1.0f };
		glm::vec4 rotAxis9 = { 0.0f, 0.0f, 0.0f, 0.0f };
		actors[ActorCount]->setupMatrices(pos9, 0.0f, rotAxis9, 100.0f);
		++ActorCount;
		//Create an actor with plane mesh
		actors[ActorCount] = new Actor(0);
		actors[ActorCount]->init(VertexArrayObjects[2], plane->faceCount(), materials[9]);
		glm::vec4 pos_9 = { -100.0f, 20.0f, 50.0f, 1.0f };
		glm::vec4 rotAxis_9 = { 0.0f, 1.0f, 0.0f, 0.0f };
		actors[ActorCount]->setupMatrices(pos_9, 90.0f, rotAxis_9, 100.0f);
		++ActorCount;
		//Create an actor with plane mesh
		actors[ActorCount] = new Actor(0);
		actors[ActorCount]->init(VertexArrayObjects[2], plane->faceCount(), materials[9]);
		glm::vec4 pos__9 = { 100.0f, 20.0f, 50.0f, 1.0f };
		glm::vec4 rotAxis__9 = { 0.0f, 1.0f, 0.0f, 0.0f };
		actors[ActorCount]->setupMatrices(pos__9, -90.0f, rotAxis__9, 100.0f);
		++ActorCount;

		/*Create an actor with plane mesh (TILING)
		actors[ActorCount] = new Actor(0);
		actors[ActorCount]->init(VertexArrayObjects[2], plane->faceCount(), materials[9]);
		glm::vec4 pos10 = { 15.0f, 10.0f, -50.0f, 1.0f };
		glm::vec4 rotAxis10 = { 0.0f, 0.0f, 0.0f, 0.0f };
		actors[ActorCount]->setupMatrices(pos10, 0.0f, rotAxis10, 10.0f);
		++ActorCount;//*/
		
		//Create the SKYBOX with cube mesh (renders last) --- SKY-BOX ---
		actors[ActorCount] = new Actor(0);
		actors[ActorCount]->init(cubeVAO, 12, materials[5]);
		glm::vec4 pos6 = { 0.0f, 0.0f, 0.0f, 1.0f };
		glm::vec4 rotAxis6 = { 1.0f, 0.0f, 0.0f, 0.0f };
		actors[ActorCount]->setupMatrices(eye, -90.0f, rotAxis6, 1.0f);
		cubemapPtr = actors[ActorCount];
		++ActorCount;//*/

		cout << "Now setting camera and lighting values" << endl;

		/////////////////////////////////////
		////      CAMERA STUFF 			/////
		/////////////////////////////////////
		eye = {0.0f, 0.0f, 30.0f, 1.0f};
		look = -Ez;

		/////////////////////////////////////
		////		LIGHTS STUFF		///// 
		////         (1 for now) 	    /////
		/////////////////////////////////////
		light1Color[0] = { 1.0f, 1.0f, 0.5f, 1.0f };
		light1[0] = eye + 1.0f * look;
		lightLookAt[0] = { look.x, look.y, look.z, 1.0f };
		unlitSphere->T = cs541::translate(light1[0]);
		//light1[0] = { 0.0f, 20.0f, 10.0f, 1.0f };
		//light1Color[0] = { 1.0f, 1.0f, 0.5f, 1.0f };
		//lightLookAt[0] = {look.x, look.y, look.z, 1.0f};


		cout << "Actors and camera setup succesful!" << endl;
	}


	void Client::createMaterials()
	{
		cout << "Starting to create Materials!" << endl;

		//POLAR SPHERE MATERIAL (index 0, texture index 3)
		materials[MaterialCount++] = new MaterialTextured(TexturedProgram, textureBufferName[3], 0.8f, 20.0f);

		//FLOOR PLANE MATERIAL (index 1, texture index 4, brick.bmp)
		materials[MaterialCount++] = new MaterialTextured(TexturedProgram, textureBufferName[4], 0.8f, 10.0f);

		//TORUS PBR TEXTURED MATERIAL (index 2, texture index 0)
		materials[MaterialCount++] = new MaterialPBR(PBRTexturedProgram, textureBufferName[0], 0.05f, 1.0f, 0.6f, { 0, 1, 0 });

		//TORUS PBR MATERIAL (index 3, Solid color green)
		materials[MaterialCount++] = new MaterialPBR(PBRProgram, NULL, 0.15f, 1.0f, 0.1f, { 0, 1, 0 });

		//TORUS PBR MATERIAL (index 4, solid color purple)
		materials[MaterialCount++] = new MaterialPBR(PBRProgram, NULL, 0.5f, 1.0f, 0.99f, { 1, 0, 1 });

		//SKYBOX MATERIAL (index 5, texture index 5, cubemap texture)
		materials[MaterialCount++] = new MaterialSkybox(SkyboxProgram, textureBufferName[5]);

		//NOT TILING TEXT MATERIAL (index 6, texture index 6, brick.bmp)
		materials[MaterialCount++] = new MaterialTextured(TexturedProgram, textureBufferName[6], 0.8f, 10.0f);

		//FLOOR PLANE TEXTURED PBR MATERIAL (index 7, grass.bmp)
		materials[MaterialCount++] = new MaterialTextured(TexturedProgram, textureBufferName[7], 0.8f, 10.0f);

		//LIGHT MATERIAL UNLIT (index 8, no texture)
		materials[MaterialCount++] = new UnlitMaterial(UnlitProgram, NULL);

		//Lamber grey # INDEX 9
		materials[MaterialCount++] = new MaterialTextured(TexturedProgram, textureBufferName[4], 0.05f, 1.0f);
		//materials[MaterialCount++] = new MaterialTextured(TexturedProgram, 0, 0.8f, 10.0f);
	}


	void Client::keypress(SDL_Keycode kc)
	{
		// respond to keyboard input
		//   kc: SDL keycode (e.g., SDLK_SPACE, SDLK_a, SDLK_s)

		//temporarily using EY as rup
		float mgt = (float)sqrt(look.x*look.x + look.y*look.y + look.z*look.z);
		glm::vec4 unitL = { look.x / mgt, look.y / mgt, look.z / mgt, 0 };
		glm::vec4 right = cs541::cross(unitL, Ey);

		switch (kc)
		{
		case SDLK_UP:
			//*
			light1[0] = light1[0] + 2.0f * unitL;
			unlitSphere->T = cs541::translate(light1[0]);
			//*/
			break;
		case SDLK_w:
			eye = eye + 5.f * unitL;
			/*EXPERIMENT////////////////////////////////////////////////////
			light1[0] = eye + 1.0f * look;
			lightLookAt[0] = { look.x, look.y, look.z, 1.0f };
			unlitSphere->T = cs541::translate(light1[0]);
			//EXPERIMENT//////////////////////////////////////////////////*/
			break;
		case SDLK_DOWN:
			//*
			light1[0] = light1[0] - 2.0f * unitL;
			unlitSphere->T = cs541::translate(light1[0]);
			//*/
			break;
		case SDLK_s:
			eye = eye - 5.f * unitL;
			/*EXPERIMENT////////////////////////////////////////////////////
			light1[0] = eye + 1.0f * look;
			lightLookAt[0] = { look.x, look.y, look.z, 1.0f };
			unlitSphere->T = cs541::translate(light1[0]);
			//EXPERIMENT//////////////////////////////////////////////////*/
			break;
		case SDLK_LEFT:
			//*
			light1[0] = light1[0] - 2.0f * right;
			unlitSphere->T = cs541::translate(light1[0]);
			//*/
			break;
		case SDLK_a:
			eye = eye - 5.f * right;
			/*EXPERIMENT////////////////////////////////////////////////////
			light1[0] = eye + 1.0f * look;
			lightLookAt[0] = { look.x, look.y, look.z, 1.0f };
			unlitSphere->T = cs541::translate(light1[0]);
			//EXPERIMENT//////////////////////////////////////////////////*/
			break;
		case SDLK_q:
			look = -look;
			break;
		case SDLK_RIGHT:
			//*
			light1[0] = light1[0] + 2.0f * right;
			unlitSphere->T = cs541::translate(light1[0]);
			//*/
			break;
		case SDLK_d:
			eye = eye + 5.f * right;
			/*EXPERIMENT////////////////////////////////////////////////////
			light1[0] = eye + 1.0f * look;
			lightLookAt[0] = { look.x, look.y, look.z, 1.0f };
			unlitSphere->T = cs541::translate(light1[0]);
			//EXPERIMENT//////////////////////////////////////////////////*/
			break;
		case SDLK_p:
			fovDivider += 0.1f;
			if (fovDivider >= 180.0f)
			{
				fovDivider = 180.0f;
			}
			FOV = 180.0f / fovDivider;
			break;
		case SDLK_o:
			fovDivider -= 0.1f;
			if (fovDivider <= 1.1f)
			{
				fovDivider = 1.1f;
			}
			FOV = 180.0f / fovDivider;
			break;
		case SDLK_TAB:
			light1[0] = eye + 1.0f * look;
			lightLookAt[0] = { look.x, look.y, look.z, 1.0f };
			unlitSphere->T = cs541::translate(light1[0]);
			break;
		};
	}


	void Client::resize(int W, int H)
	{
		// The aspect ratio of the camera should match the
		//   aspect ratio of the window

		AR = (float)W / (float)H;

		glViewport(0, 0, W, H);
	}


	void Client::mousedrag(int x, int y, bool left_button)
	{
		if (left_button)
		{
			glm::vec4 u, v;
			glm::mat4 R;

			u = cs541::cross(look, Ey);
			v = cs541::cross(u, look);

			float rotateSpeed = 0.25f / fovDivider; /*Correct this by distance to counter the arc problem*/
			R = cs541::rotate(rotateSpeed * x, v) * cs541::rotate(y * rotateSpeed, u);

			look = R * look;
		}

	}


	std::string Client::loadFile(const char *path) 
	{
		string out, line;

		ifstream in(path);
		if (in.is_open()) 
		{
			getline(in, line);
			while (in)
			{
				out += line + "\n";
				getline(in, line);
			}
			in.close();
			return out;
		}
		else 
		{
			cout << "Failed to open file!" << endl;
			return "";
		}
	}

	
	void Client::initShaders(const char *vertexPath, const char *fragmentPath, GLint *program) 
	{
		int value;
		char infolog[1024];
		string shaderString;

		// Compile vertex shader
		shaderString = loadFile(vertexPath);
		const char *vertex_shader_text = shaderString.c_str();
		GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vshader, 1, &vertex_shader_text, 0);
		glCompileShader(vshader);
		glGetShaderiv(vshader, GL_COMPILE_STATUS, &value);
		if (!value)
		{
			cerr << "vertex shader failed to compile.\nError described next: " << endl;
			glGetShaderInfoLog(vshader, 1024, 0, infolog);
			cerr << infolog << endl;
		}

		// Compile fragment shader
		shaderString = loadFile(fragmentPath);
		const char *fragment_shader_text = shaderString.c_str();
		GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fshader, 1, &fragment_shader_text, 0);
		glCompileShader(fshader);
		glGetShaderiv(fshader, GL_COMPILE_STATUS, &value);
		if (!value)
		{
			cerr << "fragment shader failed to compile.\nError described next: " << endl;
			glGetShaderInfoLog(fshader, 1024, 0, infolog);
			cerr << infolog << endl;
		}

		// (I.C) link shaders
		glAttachShader(*program, fshader);
		glAttachShader(*program, vshader);
		glLinkProgram(*program);
		glGetProgramiv(*program, GL_LINK_STATUS, &value);
		if (!value)
		{
			cerr << "PROGRAM failed to link.\nError described next: " << endl;
			glGetProgramInfoLog(*program, 1024, 0, infolog);
			cerr << infolog << endl;
		}

		glDeleteShader(fshader);
		glDeleteShader(vshader);
	}


	void Client::initMeshGlVAO(Mesh3D *mesh)
	{
		cout << "Setting up VAO number " << MeshCount << endl;
		glBindVertexArray(VertexArrayObjects[MeshCount]);

		//INEFICIENT (dynamic casts, fix later)
		float *texcoord = new float[2 * mesh->vertexCount()];
		if (dynamic_cast<PolarPlane*>(mesh))
		{
			PolarPlane *polar = static_cast<PolarPlane*>(mesh);
			for (int i = 0; i < mesh->vertexCount(); ++i)
			{
				texcoord[2 * i + 0] = polar->uvArray()[i].x;
				texcoord[2 * i + 1] = polar->uvArray()[i].y;
			}
		}
		else if (dynamic_cast<Plane*>(mesh))
		{
			glm::mat4 Std2Unit = cs541::scale(0.5f, 0.5f, 1) * cs541::translate(glm::vec4(1, 1, 0, 0));
			for (int i = 0; i < mesh->vertexCount(); ++i)
			{
				glm::vec4 uv = Std2Unit * mesh->vertexArray()[i];
				texcoord[2 * i + 0] = 10 * uv[0];
				texcoord[2 * i + 1] = 10 * uv[1];
			}
		}
		else
		{
			glm::mat4 Std2Unit = cs541::scale(0.5f, 0.5f, 1) * cs541::translate(glm::vec4(1, 1, 0, 0));
			for (int i = 0; i < mesh->vertexCount(); ++i)
			{
				glm::vec4 uv = Std2Unit * mesh->vertexArray()[i];
				texcoord[2 * i + 0] = uv[0];
				texcoord[2 * i + 1] = uv[1];
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferName[MeshCount]);
		glBufferData(GL_ARRAY_BUFFER, mesh->vertexCount() * sizeof(glm::vec4), mesh->vertexArray(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferName[MeshCount]);
		glBufferData(GL_ARRAY_BUFFER, mesh->vertexCount() * sizeof(glm::vec4), mesh->normalArray(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferName[MeshCount]);
		glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float) * mesh->vertexCount(), texcoord, GL_STATIC_DRAW);
		delete[] texcoord;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceBufferName[MeshCount]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->faceCount() * sizeof(Mesh3D::Face), mesh->faceArray(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferName[MeshCount]);
		glVertexAttribPointer(0, 4, GL_FLOAT, false, 4 * sizeof(GL_FLOAT), 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, normalBufferName[MeshCount]);
		glVertexAttribPointer(1, 4, GL_FLOAT, false, 4 * sizeof(GL_FLOAT), 0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, uvBufferName[MeshCount]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), 0);
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);

		++MeshCount;
	}


	void Client::loadTextureAndPassToGPU(const char *path, bool tiled)
	{
		int width, height;
		unsigned char *data = loadBitmapFile(path, &width, &height);
		if (data) 
		{
			glBindTexture(GL_TEXTURE_2D, textureBufferName[TextureCount++]);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			if (tiled)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			delete [] data;
		}
		else 
		{
			cout << "FAILED TO LOAD TEXTURE!!" << endl;
		}
	}


	unsigned char* Client::loadBitmapFile(const char *fname, int *width, int *height) 
	{
		fstream in(fname, ios_base::in | ios_base::binary);
		char header[38];
		in.read(header, 38);

		unsigned offset = *reinterpret_cast<unsigned*>(header + 10);
		int W = *reinterpret_cast<int*>(header + 18);
		int H = *reinterpret_cast<int*>(header + 22);
		int S = 3 * W;
		S += (4 - S % 4) % 4;
		int size = S * H;
		in.seekg(offset, ios_base::beg);
		unsigned char *data = new unsigned char[size];

		in.read(reinterpret_cast<char*>(data), size);
		if (!in) 
		{
			delete[] data;
			in.close();
			return 0;
		}

		for (int j = 0; j < H; ++j) 
		{
			for (int i = 0; i < W; ++i) 
			{
				int index = S * j + 3 * i;
				swap(data[index + 0], data[index + 2]);
			}
		}

		*width = W;
		*height = H;
		in.close();
		return data;
	}


	void Client::loadCubemapTextures(std::vector<char const *> faces)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureBufferName[TextureCount++]);
		int width, height;

		for (int i = 0; i < 6; ++i) 
		{
			unsigned char *data = loadBitmapFile(faces[i], &width, &height);
			if (data)
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

				delete[] data;
			}
			else
			{
				cout << "FAILED TO LOAD TEXTURE INTO CUBEMAP!!" << endl;
			}
		}
	}


	void Client::CubeVAOSetup() 
	{
		float vertices[] =
		{
			-1, -1, -1,
			 1, -1, -1,
			 1,  1, -1,
			-1,  1, -1,
			-1, -1,  1,
			 1, -1,  1,
			 1,  1,  1,
			-1,  1,  1
		};

		int indices[] =
		{
			0, 1, 3, 3, 1, 2,
			1, 5, 2, 2, 5, 6,
			5, 4, 6, 6, 4, 7,
			4, 0, 7, 7, 0, 3,
			3, 2, 7, 7, 2, 6,
			4, 5, 0, 0, 5, 1
		};

		glGenVertexArrays(1, &cubeVAO);
		glBindVertexArray(cubeVAO);

		glGenBuffers(1, &cubeEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glGenBuffers(1, &cubeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}

	
	void Client::RenderLightDebugDir()
	{
		float length = 0.01f;
		float vertices[] = 
		{
			0, 0, 0,
			length*lightLookAt[0].x, length*lightLookAt[0].y, length*lightLookAt[0].z
		};

		glLineWidth(8.0f);
		glUseProgram(UnlitProgram);
		unsigned int vboTemp;
		glGenBuffers(1, &vboTemp);
		glBindBuffer(GL_ARRAY_BUFFER, vboTemp);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_LINES, 0, 2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &vboTemp);
		glUseProgram(0);
		glLineWidth(8.0f);
	}
}
