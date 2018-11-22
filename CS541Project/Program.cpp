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
#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include "Affine.h"
#include "Torus.h"
#include "Client.h"

using namespace cs541;
using namespace std;

//Static headers
static void printMatrix(glm::mat4& A, char const *name);

//Main code execution
int main(int argc, char *argv[]) 
{	
	// SDL: initialize and create a window
	SDL_Init(SDL_INIT_VIDEO);
	const char *title = "CS541: Assignment 01";
	int width = 800, height = 600;
	SDL_Window *window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, width, height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	// GLEW: get function bindings (if possible)
	glewInit();
	if (!GLEW_VERSION_3_0) 
	{
		cout << "needs OpenGL version 2.0 or better" << endl;
		return -1;
	}

	// animation loop
	bool done = false;
	Client *client = new Client(width, height);
	Uint32 ticks_last = SDL_GetTicks();
	while (!done) 
	{
		//Switch for SDL events
		SDL_Event event;
		while (SDL_PollEvent(&event)) 
		{
			switch (event.type) 
			{
			case SDL_QUIT:
				done = true;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					done = true;
				else
					client->keypress(event.key.keysym.sym);
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
					client->resize(event.window.data1, event.window.data2);
				break;
			case SDL_MOUSEMOTION:
				if ((event.motion.state&SDL_BUTTON_LMASK) != 0
					|| (event.motion.state&SDL_BUTTON_RMASK) != 0)
					client->mousedrag(event.motion.xrel, event.motion.yrel,
					(event.motion.state&SDL_BUTTON_LMASK) != 0);
				break;
			}
		}

		//Get the time elapsed, and tell client to draw scene
		Uint32 ticks = SDL_GetTicks();
		double dt = 0.001 * (ticks - ticks_last);
		ticks_last = ticks;
		client->draw(dt);

		//Framerate controller
		while (dt < 0.016f) 
		{
			ticks = SDL_GetTicks();
			dt += 0.001 * (ticks - ticks_last);
		}

		//Double buffering
		SDL_GL_SwapWindow(window);
	}

	cout << "FINISH. PROCEED TO CLEAN UP" << endl;

	// clean up
	delete client;
	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return 0;
}


//Takes a matrix and a char array, and prints the content
static void printMatrix(glm::mat4& A, const char* name)
{
	if (!name) 
		return;

	std::cout << name << "\n";
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			std::cout << "- " << A[j][i];
		}
		std::cout << "\n";
	}
}

