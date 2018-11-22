//////////////////////////////////
// HEADER			//
//				//
// Name: Jose Rosenbluth Chiu	//
// Course Number: CS541		//
// Assignment number: #4	//
// Due date: 11/08/2018		//
//				//
//////////////////////////////////

///////////////////////////////////////////////
////   COMPILING INSTRUCTIONS              ////
///////////////////////////////////////////////

To compile from Visual Studio command prompt:				

cl /EHsc Program.cpp Camera.cpp Affine.cpp Client.cpp Actor.cpp MaterialPBR.cpp MaterialSkybox.cpp MaterialTextured.cpp PolarPlane.cpp Torus.cpp Sphere.cpp Plane.cpp opengl32.lib glew32.lib sdl2.lib sdl2main.lib /link /subsystem:console

If the computer does not have the GL, glm, SDL2 and libs in the include directories, then you can compile by putting them all in this directory 
and running the command below on the Visual Studio command prompt:

cl /I. /EHsc Program.cpp Camera.cpp Affine.cpp Client.cpp Actor.cpp MaterialPBR.cpp MaterialSkybox.cpp MaterialTextured.cpp PolarPlane.cpp Torus.cpp Sphere.cpp Plane.cpp opengl32.lib glew32.lib sdl2.lib sdl2main.lib /link /subsystem:console

To compile from Linux command line:

g++ Program.cpp Camera.cpp Affine.cpp Client.cpp Actor.cpp Torus.cpp Sphere.cpp Plane.cpp -lGL -lGLEW -lSDL2



///////////////////////////////////////////////
////   INSTRUCTIONS (keyboard and mouse)   ////
///////////////////////////////////////////////

- Move camera using:
	W for forward,
	S for back,
	D for right,
	A for left.
	Q for turning.

- Rotate camera by click and drag.

- Zoom in and out using P and O keys.

- Move the light source using the directional arrows
	UP for forward,
	DOWN for back,
	RIGHT for right,
	LEFT for left.