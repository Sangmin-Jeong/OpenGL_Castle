//***************************************************************************
// Game2012_Final_Heron_Jeong.cpp by Heron(101375650) and Jeong(101369732) - ID

// Description:
//	Click run to see the results.
//*****************************************************************************

////http://glew.sourceforge.net/
//The OpenGL Extension Wrangler Library (GLEW) is a cross-platform open-source C/C++ extension loading library. 
//GLEW provides efficient run-time mechanisms for determining which OpenGL extensions are supported on the target
//platform. OpenGL core and extension functionality is exposed in a single header file. GLEW has been tested on a 
//variety of operating systems, including Windows, Linux, Mac OS X, FreeBSD, Irix, and Solaris.
//
//http://freeglut.sourceforge.net/
//The OpenGL Utility Toolkit(GLUT) is a library of utilities for OpenGL programs, which primarily perform system - level I / O with the host operating system.
//Functions performed include window definition, window control, and monitoring of keyboardand mouse input.
//Routines for drawing a number of geometric primitives(both in solid and wireframe mode) are also provided, including cubes, spheresand the Utah teapot.
//GLUT also has some limited support for creating pop - up menus..

//OpenGL functions are in a single library named GL (or OpenGL in Windows). Function names begin with the letters glSomeFunction*();
//Shaders are written in the OpenGL Shading Language(GLSL)
//To interface with the window system and to get input from external devices into our programs, we need another library. For the XWindow System, this library is called GLX, for Windows, it is wgl,
//and for the Macintosh, it is agl. Rather than using a different library for each system,
//we use two readily available libraries, the OpenGL Extension Wrangler(GLEW) and the OpenGLUtilityToolkit(GLUT).
//GLEW removes operating system dependencies. GLUT provides the minimum functionality that should be expected in any modern window system.
//OpenGL makes heavy use of defined constants to increase code readability and avoid the use of magic numbers.Thus, strings such as GL_FILL and GL_POINTS are defined in header(#include <GL/glut.h>)

//https://glm.g-truc.net/0.9.9/index.html
////OpenGL Mathematics (GLM) is a header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications.
///////////////////////////////////////////////////////////////////////

using namespace std;

#include "stdlib.h"
#include "time.h"
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include "prepShader.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include <string>
#include <iostream>
#include "Shape.h"

#define BUFFER_OFFSET(x)  ((const void*) (x))
#define FPS 60
#define MOVESPEED 0.5f
#define TURNSPEED 0.05f
#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)
#define XY_AXIS glm::vec3(1,0.9,0)
#define YZ_AXIS glm::vec3(0,1,1)
#define XZ_AXIS glm::vec3(1,0,1)
#define XYZ_AXIS glm::vec3(1,1,1)
#define SPEED 0.25f

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

enum keyMasks {
	KEY_FORWARD = 0b00000001,		// 0x01 or   1	or   01
	KEY_BACKWARD = 0b00000010,		// 0x02 or   2	or   02
	KEY_LEFT = 0b00000100,
	KEY_RIGHT = 0b00001000,
	KEY_UP = 0b00010000,
	KEY_DOWN = 0b00100000,
	KEY_MOUSECLICKED = 0b01000000

	// Any other keys you want to add.
};

static unsigned int
program,
vertexShaderId,
fragmentShaderId;

GLuint modelID, viewID, projID;
glm::mat4 View, Projection;

// Our bitflag variable. 1 byte for up to 8 key states.
unsigned char keys = 0; // Initialized to 0 or 0b00000000.

// Texture variables.
GLuint blankID, brickID, doorID, grassID, woodID;
GLint width, height, bitDepth;

// Light objects. Now OOP.
AmbientLight aLight(
	glm::vec3(1.0f, 1.0f, 0.9f),	// Diffuse colour.
	0.5f);

PointLight pLights[2] = {
	{ glm::vec3(50.0f, 10.0f, 0.0f),	// Position.
	100.0f,							// Range.
	1.0f, 4.5f, 75.0f,				// Constant, Linear, Quadratic.   
	glm::vec3(1.0f, 1.0f, 1.0f),	// Diffuse colour.
	1.0f },							// Diffuse strength.

	{ glm::vec3(50.0f, 44.0f, -50.f),	// Position.
	1000.0f,							// Range.
	1.0f, 4.5f, 75.0f,				// Constant, Linear, Quadratic.   
	glm::vec3(1.0f, 1.0f, 1.0f),	// Diffuse colour.
	1.0f } };						// Diffuse strength.

PointLight pLight =
{ glm::vec3(50.0f, 10.0f, -100.0f),	// Position.
100.0f,							// Range.
1.0f, 4.5f, 75.0f,				// Constant, Linear, Quadratic.   
glm::vec3(1.0f, 1.0f, 1.0f),	// Diffuse colour.
1.0f };

// Camera and transform variables.
float scale = 1.0f, angle = 0.0f;
glm::vec3 position, frontVec, worldUp, upVec, rightVec; // Set by function
GLfloat pitch, yaw;
int lastX, lastY;

// Geometry data.
Grid g_grid(100);

// Ground
Plane ground;

// Outer walls
Cube front_wall(80.0f,8.0f,3.0f);
Cube left_wall(80.0f, 8.0f, 3.0f);
Cube right_wall(80.0f, 8.0f, 3.0f);
Cube back_wall(80.0f, 8.0f, 3.0f);
Cube cube(1.0f, 1.0f, 1.0f);

// Towers and Cones
Prism frontLeft_tower(12);
Cone frontLeft_cone(12);

Prism frontRight_tower(12);
Cone frontRight_cone(12);

Prism backLeft_tower(12);
Cone backLeft_cone(12);

Prism backRight_tower(12);
Cone backRight_cone(12);

// Gate
Plane gate;

// Keep
Cube inner_tower(5.0f, 30.0f, 5.0f);
Cube keep(31.0f, 28.0f, 31.0f);
Cube entrance(10.0f, 15.0f, 10.0f);

void timer(int); // Prototype.

void resetView()
{
	position = glm::vec3(53.0f, 3.0f, 25.0f); // Super pulled back because of grid size.
	frontVec = glm::vec3(0.0f, 0.0f, -1.0f);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	pitch = 0.0f;
	yaw = -90.0f;
	// View will now get set only in transformObject
}

void SetupLights()
{
	// Setting ambient light.
	glUniform3f(glGetUniformLocation(program, "aLight.base.diffuseColour"), aLight.diffuseColour.x, aLight.diffuseColour.y, aLight.diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "aLight.base.diffuseStrength"), aLight.diffuseStrength);

	// Setting point lights.
	glUniform3f(glGetUniformLocation(program, "pLights[0].base.diffuseColour"), pLights[0].diffuseColour.x, pLights[0].diffuseColour.y, pLights[0].diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLights[0].base.diffuseStrength"), pLights[0].diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLights[0].position"), pLights[0].position.x, pLights[0].position.y, pLights[0].position.z);
	glUniform1f(glGetUniformLocation(program, "pLights[0].constant"), pLights[0].constant);
	glUniform1f(glGetUniformLocation(program, "pLights[0].linear"), pLights[0].linear);
	glUniform1f(glGetUniformLocation(program, "pLights[0].quadratic"), pLights[0].quadratic);

	glUniform3f(glGetUniformLocation(program, "pLights[1].base.diffuseColour"), pLights[1].diffuseColour.x, pLights[1].diffuseColour.y, pLights[1].diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLights[1].base.diffuseStrength"), pLights[1].diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLights[1].position"), pLights[1].position.x, pLights[1].position.y, pLights[1].position.z);
	glUniform1f(glGetUniformLocation(program, "pLights[1].constant"), pLights[1].constant);
	glUniform1f(glGetUniformLocation(program, "pLights[1].linear"), pLights[1].linear);
	glUniform1f(glGetUniformLocation(program, "pLights[1].quadratic"), pLights[1].quadratic);

	glUniform3f(glGetUniformLocation(program, "pLight.base.diffuseColour"), pLight.diffuseColour.x, pLight.diffuseColour.y, pLight.diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLight.base.diffuseStrength"), pLight.diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLight.position"), pLight.position.x, pLight.position.y, pLight.position.z);
	glUniform1f(glGetUniformLocation(program, "pLight.constant"), pLight.constant);
	glUniform1f(glGetUniformLocation(program, "pLight.linear"), pLight.linear);
	glUniform1f(glGetUniformLocation(program, "pLight.quadratic"), pLight.quadratic);
}

void init(void)
{
	srand((unsigned)time(NULL));
	// Create shader program executable.
	vertexShaderId = setShader((char*)"vertex", (char*)"cube3.vert");
	fragmentShaderId = setShader((char*)"fragment", (char*)"cube3.frag");
	program = glCreateProgram();
	glAttachShader(program, vertexShaderId);
	glAttachShader(program, fragmentShaderId);
	glLinkProgram(program);
	glUseProgram(program);

	modelID = glGetUniformLocation(program, "model");
	viewID = glGetUniformLocation(program, "view");
	projID = glGetUniformLocation(program, "projection");
	
	// Projection matrix : 45∞ Field of View, 1:1 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), 1.0f / 1.0f, 0.1f, 200.0f);
	// Or, for an ortho camera :
	// Projection = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	resetView();

	// Image loading.
	stbi_set_flip_vertically_on_load(true);


	unsigned char* image;
	image = stbi_load("brick.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &brickID);
	glBindTexture(GL_TEXTURE_2D, brickID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);

	image = stbi_load("door.png", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &doorID);
	glBindTexture(GL_TEXTURE_2D, doorID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);

	image = stbi_load("grass2.png", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &grassID);
	glBindTexture(GL_TEXTURE_2D, grassID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);

	image = stbi_load("wood.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &woodID);
	glBindTexture(GL_TEXTURE_2D, woodID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	
	glUniform1i(glGetUniformLocation(program, "texture0"), 0);

	SetupLights();

	// All VAO/VBO data now in Shape.h! But we still need to do this AFTER OpenGL is initialized.
	g_grid.BufferShape();

	// Ground
	ground.BufferShape();

	// Walls
	front_wall.BufferShape();
	right_wall.BufferShape();
	left_wall.BufferShape();
	back_wall.BufferShape();
	cube.BufferShape();

	// Towers and Cones
	frontLeft_tower.BufferShape();
	frontLeft_cone.BufferShape();
	frontRight_tower.BufferShape();
	frontRight_cone.BufferShape();
	backLeft_tower.BufferShape();
	backLeft_cone.BufferShape();
	backRight_tower.BufferShape();
	backRight_cone.BufferShape();

	// Gate
	gate.BufferShape();

	// Keep
	inner_tower.BufferShape();
	keep.BufferShape();
	entrance.BufferShape();

	// Enable depth testing and face culling. 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	//glCullFace(GL_BACK);

	timer(0); // Setup my recursive 'fixed' timestep/framerate.
}

void calculateView()
{
	frontVec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontVec.y = sin(glm::radians(pitch));
	frontVec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontVec = glm::normalize(frontVec);
	rightVec = glm::normalize(glm::cross(frontVec, worldUp));
	upVec = glm::normalize(glm::cross(rightVec, frontVec));

	View = glm::lookAt(
		position, // Camera position
		position + frontVec, // Look target
		upVec); // Up vector

	glUniform3f(glGetUniformLocation(program, "eyePosition"), position.x, position.y, position.z);

}

void transformObject(glm::vec3 scale, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 translation) {
	glm::mat4 Model;
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, translation);
	Model = glm::rotate(Model, glm::radians(rotationAngle), rotationAxis);
	Model = glm::scale(Model, scale);
	
	// We must now update the View.
	calculateView();

	glUniformMatrix4fv(modelID, 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(projID, 1, GL_FALSE, &Projection[0][0]);
}

void BuildBattlementsX(int size, float x, float y, float z)
{
	float x1 = x, y1 = y, z1 = z;
	for (int i = 0; i < size; i += 3)
	{
		glBindTexture(GL_TEXTURE_2D, brickID);
		transformObject(glm::vec3(1.0f, 1.0f, 1.0f), Y_AXIS, 0.0f, glm::vec3(x1, y1, z1));
		cube.DrawShape(GL_TRIANGLES, program);
		x1++;
		transformObject(glm::vec3(1.0f, 1.0f, 1.0f), Y_AXIS, 0.0f, glm::vec3(x1, y1, z1));
		cube.DrawShape(GL_TRIANGLES, program);
		x1++;
		x1++;
	}
}

void BuildBattlementsZ(int size, float x, float y, float z)
{
	float x1 = x, y1 = y, z1 = z;
	for (int i = 0; i < size; i += 3)
	{
		glBindTexture(GL_TEXTURE_2D, brickID);
		transformObject(glm::vec3(1.0f, 1.0f, 1.0f), Y_AXIS, 0.0f, glm::vec3(x1, y1, z1));
		cube.DrawShape(GL_TRIANGLES, program);
		z1--;
		transformObject(glm::vec3(1.0f, 1.0f, 1.0f), Y_AXIS, 0.0f, glm::vec3(x1, y1, z1));
		cube.DrawShape(GL_TRIANGLES, program);
		z1--;
		z1--;
	}
}

void DrawPlane(int size)
{
	for (int i = 0; i < size; i++)
	{
		glBindTexture(GL_TEXTURE_2D, grassID);
		transformObject(glm::vec3(1.0f, 1.0f, 1.0f), X_AXIS, -90.0f, glm::vec3(i, 0.0f, 0.0f));
		ground.DrawShape(GL_TRIANGLES, program);

		for (int j = 1; j < size; j++)
		{
			transformObject(glm::vec3(1.0f, 1.0f, 1.0f), X_AXIS, -90.0f, glm::vec3(i, 0.0f, -j));
			ground.DrawShape(GL_TRIANGLES, program);
		}
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Grid.
	//glBindTexture(GL_TEXTURE_2D, blankID);
	//transformObject(glm::vec3(1.0f, 1.0f, 1.0f), X_AXIS, -90.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	//g_grid.DrawShape(GL_LINE_STRIP, program);

	// Ground
	DrawPlane(100);

	// Outer walls.
	glBindTexture(GL_TEXTURE_2D, brickID);
	transformObject(glm::vec3(80.0f, 8.0f, 3.0f), XY_AXIS, 0.0f, glm::vec3(10.0f, 0.0f, -10.0f));
	front_wall.DrawShape(GL_TRIANGLES, program);
	BuildBattlementsX(77, 14.0f, 8.0f, -8.0f);

	transformObject(glm::vec3(80.0f, 8.0f, 3.0f), Y_AXIS, 90.0f, glm::vec3(10.0f, 0.0f, -10.0f));
	left_wall.DrawShape(GL_TRIANGLES, program);
	BuildBattlementsZ(77, 10.0f, 8.0f, -8.0f);

	transformObject(glm::vec3(80.0f, 8.0f, 3.0f), Y_AXIS, 90.0f, glm::vec3(87.0f, 0.0f, -10.0f));
	right_wall.DrawShape(GL_TRIANGLES, program);
	BuildBattlementsZ(77, 89.0f, 8.0f, -8.0f);

	transformObject(glm::vec3(80.0f, 8.0f, 3.0f), Y_AXIS, 0.0f, glm::vec3(10.0f, 0.0f, -90.0f));
	back_wall.DrawShape(GL_TRIANGLES, program);
	BuildBattlementsX(77, 14.0f, 8.0f, -90.0f);


	// Towers with Cones
	glBindTexture(GL_TEXTURE_2D, brickID);
	transformObject(glm::vec3(8.0f, 13.0f, 8.0f), X_AXIS, 0.0f, glm::vec3(7.0f, 0.0f, -13.0f));
	frontLeft_tower.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(9.0f, 3.0f, 9.0f), X_AXIS, 0.0f, glm::vec3(6.5f, 13.0f, -13.5f));
	frontLeft_cone.DrawShape(GL_TRIANGLES, program);

	transformObject(glm::vec3(8.0f, 13.0f, 8.0f), X_AXIS, 0.0f, glm::vec3(85.0f, 0.0f, -13.0f));
	frontRight_tower.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(9.0f, 3.0f, 9.0f), X_AXIS, 0.0f, glm::vec3(84.5f, 13.0f, -13.5f));
	frontRight_cone.DrawShape(GL_TRIANGLES, program);

	transformObject(glm::vec3(8.0f, 13.0f, 8.0f), X_AXIS, 0.0f, glm::vec3(7.0f, 0.0f, -93.0f));
	backLeft_tower.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(9.0f, 3.0f, 9.0f), X_AXIS, 0.0f, glm::vec3(6.5f, 13.0f, -93.5f));
	backLeft_cone.DrawShape(GL_TRIANGLES, program);

	transformObject(glm::vec3(8.0f, 13.0f, 8.0f), X_AXIS, 0.0f, glm::vec3(85.0f, 0.0f, -93.0f));
	backRight_tower.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(9.0f, 3.0f, 9.0f), X_AXIS, 0.0f, glm::vec3(84.5f, 13.0f, -93.5f));
	backRight_cone.DrawShape(GL_TRIANGLES, program);

	// Towers for the main gate
	glBindTexture(GL_TEXTURE_2D, brickID);
	transformObject(glm::vec3(6.0f, 10.0f, 6.0f), X_AXIS, 0.0f, glm::vec3(40.0f, 0.0f, -12.0f));
	frontLeft_tower.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(7.0f, 2.0f, 7.0f), X_AXIS, 0.0f, glm::vec3(39.5f, 10.0f, -12.5f));
	frontLeft_cone.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(6.0f, 10.0f, 6.0f), X_AXIS, 0.0f, glm::vec3(60.0f, 0.0f, -12.0f));
	frontRight_tower.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(7.0f, 2.0f, 7.0f), X_AXIS, 0.0f, glm::vec3(59.5f, 10.0f, -12.5f));
	frontRight_cone.DrawShape(GL_TRIANGLES, program);

	// Stair for the main gate
	transformObject(glm::vec3(4.0f, 0.5f, 1.0f), X_AXIS, 0.0f, glm::vec3(51.0f, 0.0f, -7.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(4.0f, 0.5f, 0.5f), X_AXIS, 0.0f, glm::vec3(51.0f, 0.5f, -7.0f));
	cube.DrawShape(GL_TRIANGLES, program);

	// Main gate
	glBindTexture(GL_TEXTURE_2D, doorID);
	transformObject(glm::vec3(10.0f, 6.5f, 1.0f), X_AXIS, 0.0f, glm::vec3(48.0f, 1.0f, -6.9f));
	gate.RecolorShape(1.0f, 1.0f, 1.0f, 1.0f);
	gate.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(10.0f, 7.5f, 1.0f), X_AXIS, 0.0f, glm::vec3(48.0f, 0.0f, -10.1f));
	gate.RecolorShape(0, 0, 0, 1);
	gate.DrawShape(GL_TRIANGLES, program);

	// Keep
	glBindTexture(GL_TEXTURE_2D, brickID);
	transformObject(glm::vec3(31.0f, 28.0f, 31.0f), X_AXIS, 0.0f, glm::vec3(37.0f, 0.0f, -74.0f));
	keep.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(8.0f, 10.0f, 8.0f), X_AXIS, 0.0f, glm::vec3(48.0f, 0.0f, -44.0f));
	entrance.DrawShape(GL_TRIANGLES, program);

	// Stair for keep's entrance
	transformObject(glm::vec3(4.0f, 1.0f, 4.0f), X_AXIS, 0.0f, glm::vec3(56.0f, 0.0f, -42.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(3.0f, 1.0f, 4.0f), X_AXIS, 0.0f, glm::vec3(56.0f, 1.0f, -42.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(2.0f, 1.0f, 4.0f), X_AXIS, 0.0f, glm::vec3(56.0f, 2.0f, -42.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(1.0f, 1.0f, 4.0f), X_AXIS, 0.0f, glm::vec3(56.0f, 3.0f, -42.0f));
	cube.DrawShape(GL_TRIANGLES, program);

	// Keep's gate
	glBindTexture(GL_TEXTURE_2D, doorID);
	transformObject(glm::vec3(4.0f, 3.0f, 1.0f), Y_AXIS, 90.0f, glm::vec3(56.1f, 4.0f, -38.0f));
	gate.RecolorShape(1.0f, 1.0f, 1.0f, 1.0f);
	gate.DrawShape(GL_TRIANGLES, program);
	glBindTexture(GL_TEXTURE_2D, brickID);

	// Roof
	glBindTexture(GL_TEXTURE_2D, woodID);
	transformObject(glm::vec3(20.0f, 20.0f, 20.0f), X_AXIS, 45.0f,  glm::vec3(42.5f, 26.0f, -73.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	glBindTexture(GL_TEXTURE_2D, brickID);


	// Keep's tower
	transformObject(glm::vec3(5.0f, 30.0f, 5.0f), X_AXIS, 0.0f, glm::vec3(35.0f, 0.0f, -45.0f));
	inner_tower.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(2.0f, 1.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(35.0f, 30.0f, -45.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(2.0f, 1.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(38.0f, 30.0f, -45.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(2.0f, 1.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(35.0f, 30.0f, -41.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(2.0f, 1.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(38.0f, 30.0f, -41.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(1.0f, 1.0f, 2.0f), X_AXIS, 0.0f, glm::vec3(35.0f, 30.0f, -45.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(1.0f, 1.0f, 2.0f), X_AXIS, 0.0f, glm::vec3(35.0f, 30.0f, -42.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(1.0f, 1.0f, 2.0f), X_AXIS, 0.0f, glm::vec3(39.0f, 30.0f, -42.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(1.0f, 1.0f, 2.0f), X_AXIS, 0.0f, glm::vec3(39.0f, 30.0f, -45.0f));
	cube.DrawShape(GL_TRIANGLES, program);

	transformObject(glm::vec3(5.0f, 30.0f, 5.0f), X_AXIS, 0.0f, glm::vec3(65.0f, 0.0f, -45.0f));
	inner_tower.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(2.0f, 1.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(65.0f, 30.0f, -45.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(2.0f, 1.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(68.0f, 30.0f, -45.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(2.0f, 1.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(65.0f, 30.0f, -41.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(2.0f, 1.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(68.0f, 30.0f, -41.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(1.0f, 1.0f, 2.0f), X_AXIS, 0.0f, glm::vec3(65.0f, 30.0f, -45.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(1.0f, 1.0f, 2.0f), X_AXIS, 0.0f, glm::vec3(65.0f, 30.0f, -42.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(1.0f, 1.0f, 2.0f), X_AXIS, 0.0f, glm::vec3(69.0f, 30.0f, -42.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(1.0f, 1.0f, 2.0f), X_AXIS, 0.0f, glm::vec3(69.0f, 30.0f, -45.0f));
	cube.DrawShape(GL_TRIANGLES, program);

	transformObject(glm::vec3(5.0f, 30.0f, 5.0f), X_AXIS, 0.0f, glm::vec3(35.0f, 0.0f, -75.0f));
	inner_tower.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(2.0f, 1.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(35.0f, 30.0f, -75.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(2.0f, 1.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(38.0f, 30.0f, -75.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(2.0f, 1.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(35.0f, 30.0f, -71.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(2.0f, 1.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(38.0f, 30.0f, -71.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(1.0f, 1.0f, 2.0f), X_AXIS, 0.0f, glm::vec3(35.0f, 30.0f, -75.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(1.0f, 1.0f, 2.0f), X_AXIS, 0.0f, glm::vec3(35.0f, 30.0f, -72.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(1.0f, 1.0f, 2.0f), X_AXIS, 0.0f, glm::vec3(39.0f, 30.0f, -72.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(1.0f, 1.0f, 2.0f), X_AXIS, 0.0f, glm::vec3(39.0f, 30.0f, -75.0f));
	cube.DrawShape(GL_TRIANGLES, program);

	transformObject(glm::vec3(5.0f, 30.0f, 5.0f), X_AXIS, 0.0f, glm::vec3(65.0f, 0.0f, -75.0f));
	inner_tower.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(2.0f, 1.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(65.0f, 30.0f, -75.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(2.0f, 1.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(68.0f, 30.0f, -75.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(2.0f, 1.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(65.0f, 30.0f, -71.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(2.0f, 1.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(68.0f, 30.0f, -71.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(1.0f, 1.0f, 2.0f), X_AXIS, 0.0f, glm::vec3(65.0f, 30.0f, -75.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(1.0f, 1.0f, 2.0f), X_AXIS, 0.0f, glm::vec3(65.0f, 30.0f, -72.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(1.0f, 1.0f, 2.0f), X_AXIS, 0.0f, glm::vec3(69.0f, 30.0f, -72.0f));
	cube.DrawShape(GL_TRIANGLES, program);
	transformObject(glm::vec3(1.0f, 1.0f, 2.0f), X_AXIS, 0.0f, glm::vec3(69.0f, 30.0f, -75.0f));
	cube.DrawShape(GL_TRIANGLES, program);


	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glutSwapBuffers(); // Now for a potentially smoother render.
}

void idle() // Not even called.
{
	glutPostRedisplay();
}

void parseKeys()
{
	if (keys & KEY_FORWARD)
		position += frontVec * MOVESPEED;
	if (keys & KEY_BACKWARD)
		position -= frontVec * MOVESPEED;
	if (keys & KEY_LEFT)
		position -= rightVec * MOVESPEED;
	if (keys & KEY_RIGHT)
		position += rightVec * MOVESPEED;
	if (keys & KEY_UP)
		position += upVec * MOVESPEED;
	if (keys & KEY_DOWN)
		position -= upVec * MOVESPEED;
}

void timer(int) { // Tick of the frame.
	// Get first timestamp
	int start = glutGet(GLUT_ELAPSED_TIME);
	// Update call
	parseKeys();
	// Display call
	glutPostRedisplay();
	// Calling next tick
	int end = glutGet(GLUT_ELAPSED_TIME);
	glutTimerFunc((1000 / FPS) - (end-start), timer, 0);
}

// Keyboard input processing routine.
// Keyboard input processing routine.
void keyDown(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 'w':
		if (!(keys & KEY_FORWARD))
			keys |= KEY_FORWARD; // keys = keys | KEY_FORWARD
		break;
	case 's':
		if (!(keys & KEY_BACKWARD))
			keys |= KEY_BACKWARD;
		break;
	case 'a':
		if (!(keys & KEY_LEFT))
			keys |= KEY_LEFT;
		break;
	case 'd':
		if (!(keys & KEY_RIGHT))
			keys |= KEY_RIGHT;
		break;
	case 'r':
		if (!(keys & KEY_UP))
			keys |= KEY_UP;
		break;
	case 'f':
		if (!(keys & KEY_DOWN))
			keys |= KEY_DOWN;
		break;
	default:
		break;
	}
}

void keyDownSpec(int key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case GLUT_KEY_UP: // Up arrow.
		if (!(keys & KEY_FORWARD))
			keys |= KEY_FORWARD; // keys = keys | KEY_FORWARD
		break;
	case GLUT_KEY_DOWN: // Down arrow.
		if (!(keys & KEY_BACKWARD))
			keys |= KEY_BACKWARD;
		break;
	default:
		break;
	}
}

void keyUp(unsigned char key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case 'w':
		keys &= ~KEY_FORWARD; // keys = keys & ~KEY_FORWARD. ~ is bitwise NOT.
		break;
	case 's':
		keys &= ~KEY_BACKWARD;
		break;
	case 'a':
		keys &= ~KEY_LEFT;
		break;
	case 'd':
		keys &= ~KEY_RIGHT;
		break;
	case 'r':
		keys &= ~KEY_UP;
		break;
	case 'f':
		keys &= ~KEY_DOWN;
		break;
	case ' ':
		resetView();
		break;
	default:
		break;
	}
}

void keyUpSpec(int key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case GLUT_KEY_UP:
		keys &= ~KEY_FORWARD; // keys = keys & ~KEY_FORWARD
		break;
	case GLUT_KEY_DOWN:
		keys &= ~KEY_BACKWARD;
		break;
	default:
		break;
	}
}

void mouseMove(int x, int y)
{
	if (keys & KEY_MOUSECLICKED)
	{
		pitch += (GLfloat)((y - lastY) * TURNSPEED);
		yaw -= (GLfloat)((x - lastX) * TURNSPEED);
		lastY = y;
		lastX = x;
	}
}

void mouseClick(int btn, int state, int x, int y)
{
	if (state == 0)
	{
		lastX = x;
		lastY = y;
		keys |= KEY_MOUSECLICKED; // Flip flag to true
		glutSetCursor(GLUT_CURSOR_NONE);
		//cout << "Mouse clicked." << endl;
	}
	else
	{
		keys &= ~KEY_MOUSECLICKED; // Reset flag to false
		glutSetCursor(GLUT_CURSOR_INHERIT);
		//cout << "Mouse released." << endl;
	}
}

//---------------------------------------------------------------------
//
// clean
//
void clean()
{
	cout << "Cleaning up!" << endl;
	glDeleteTextures(1, &blankID);
}

//---------------------------------------------------------------------
//
// main
//
int main(int argc, char** argv)
{
	//Before we can open a window, theremust be interaction between the windowing systemand OpenGL.In GLUT, this interaction is initiated by the following function call :
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
	glutSetOption(GLUT_MULTISAMPLE, 8);

	//if you comment out this line, a window is created with a default size
	glutInitWindowSize(1024, 1024);

	//the top-left corner of the display
	glutInitWindowPosition(0, 0);

	glutCreateWindow("Game2012_Final_Heron_Jeong");

	glewInit();	//Initializes the glew and prepares the drawing pipeline.

	init(); // Our own custom function.

	glutDisplayFunc(display);
	glutKeyboardFunc(keyDown);
	glutSpecialFunc(keyDownSpec);
	glutKeyboardUpFunc(keyUp);
	glutSpecialUpFunc(keyUpSpec);

	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove); // Requires click to register.

	atexit(clean); // This useful GLUT function calls specified function before exiting program. 
	glutMainLoop();

	return 0;
}