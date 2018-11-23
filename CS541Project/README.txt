//////////////////////////////////
// HEADER			//
//				//
// Name: Jose Rosenbluth Chiu	//
// Course Number: CS541		//
// Assignment number: #5	//
// Due date: 12/06/2018		//
//				//
//////////////////////////////////

///////////////////////////////////////////////
////   COMPILING INSTRUCTIONS              ////
///////////////////////////////////////////////

To compile from Visual Studio command prompt:				

cl /EHsc Program.cpp Camera.cpp Affine.cpp Client.cpp Actor.cpp UnlitMaterial.cpp MaterialPBR.cpp MaterialSkybox.cpp MaterialTextured.cpp PolarPlane.cpp Torus.cpp Sphere.cpp Plane.cpp opengl32.lib glew32.lib sdl2.lib sdl2main.lib /link /subsystem:console

If the computer does not have the GL, glm, SDL2 and libs in the include directories, then you can compile by putting them all in this directory 
and running the command below on the Visual Studio command prompt:

cl /I. /EHsc Program.cpp Camera.cpp Affine.cpp Client.cpp Actor.cpp UnlitMaterial.cpp MaterialPBR.cpp MaterialSkybox.cpp MaterialTextured.cpp PolarPlane.cpp Torus.cpp Sphere.cpp Plane.cpp opengl32.lib glew32.lib sdl2.lib sdl2main.lib /link /subsystem:console

To compile from Linux command line:

g++ Program.cpp Camera.cpp Affine.cpp Client.cpp Actor.cpp Torus.cpp Sphere.cpp Plane.cpp -lGL -lGLEW -lSDL2


///////////////////////////////////////////////
////   INSTRUCTIONS (keyboard and mouse)   ////
///////////////////////////////////////////////

- In this example, the light source will move along the camera, and point in the direction you are looking at (as a lantern).
  This is to facilitate seeing the light and shadow interaction.
  
- Also, I made the shader only lit up what is inside the light fustrum. This means the rest of the scene will look as if it 
  was in   darkness (except for the skybox, which is unaffected).

- Move camera using:
	W for forward,
	S for back,
	D for right,
	A for left.
	Q for turning.

- Rotate camera by click and drag.

- Zoom in and out using P and O keys.
