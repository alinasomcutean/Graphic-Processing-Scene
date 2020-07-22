//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC

#include <iostream>
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include "Shader.hpp"
#include "Camera.hpp"
#define TINYOBJLOADER_IMPLEMENTATION

#include "Model3D.hpp"
#include "Mesh.hpp"
#include "SkyBox.hpp"
#include "OpenGL_4_Application_VS2015.h"

int glWindowWidth = 640;
int glWindowHeight = 480;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat3 lightDirMatrix;
GLuint lightDirMatrixLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

glm::mat3 lightDirMatrix2;
GLuint lightDirMatrixLoc2;

glm::vec3 lightDir2;
GLuint lightDirLoc2;
glm::vec3 lightColor2;
GLuint lightColorLoc2;

gps::Camera myCamera(glm::vec3(0.0f, 1.0f, 2.5f), glm::vec3(70.0f, 0.0f, -10.0f));
float cameraSpeed = 0.4f;

bool pressedKeys[1024];
GLfloat angle = 0.0f;
GLfloat lightAngle;

gps::Model3D myModel;
gps::Model3D ground;

gps::Model3D cathedral;
gps::Model3D house;
gps::Model3D house2;
gps::Model3D windmill;
gps::Model3D watchTower;

gps::Model3D tree1;
//gps::Model3D tree2;
gps::Model3D tree3;

gps::Model3D plant1;

gps::Model3D frog;
gps::Model3D dog;
gps::Model3D rabbit;
gps::Model3D eagle;

gps::Model3D boy;
gps::Model3D lamp;
gps::Model3D scouter;

gps::Model3D test;

gps::Shader myCustomShader;
gps::Shader skyboxShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

gps::SkyBox mySkyBox;

double lastX, lastY;
double yaw = 0.0f;
double pitch = 0.0f;
bool firstMouse = true;

float delta = 0.0f;
bool dogDirection = true;

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	//set projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	myCamera.rotate(pitch, yaw);
}

void processMovement()
{
	if (pressedKeys[GLFW_KEY_Q]) {
		//angle += 0.1f;
		angle += 10;
		if (angle > 360.0f)
			angle -= 360.0f;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		//angle -= 0.1f;
		angle += 10;
		if (angle < 0.0f)
			angle += 360.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_J]) {

		lightAngle += 0.3f;
		if (lightAngle > 360.0f)
			lightAngle -= 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle -= 0.3f;
		if (lightAngle < 0.0f)
			lightAngle += 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}

	if (pressedKeys[GLFW_KEY_RIGHT])
	{
		pitch += 1.0f;
		if (pitch > 360.0f)
			pitch = 0.0f;

		myCamera.rotate(yaw, pitch);
	}

	if (pressedKeys[GLFW_KEY_LEFT])
	{
		pitch -= 1.0f;
		if (pitch < -360.0f)
			pitch = 0.0f;

		myCamera.rotate(yaw, pitch);
	}

	if (pressedKeys[GLFW_KEY_UP])
	{
		yaw += 1.0f;
		if (yaw > 360.0f)
			yaw = 0.0f;

		myCamera.rotate(yaw, pitch);
	}

	if (pressedKeys[GLFW_KEY_DOWN])
	{
		yaw -= 1.0f;
		if (yaw < -360.0f)
			yaw = 0.0f;

		myCamera.rotate(yaw, pitch);
	}

	if (pressedKeys[GLFW_KEY_M]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (pressedKeys[GLFW_KEY_N]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (pressedKeys[GLFW_KEY_B]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	//for Mac OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);

	glfwWindowHint(GLFW_SAMPLES, 4);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	//glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initFBO() {
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTransformMatrix() {
	const GLfloat near_plane = 1.0f, far_plane = 30.0f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

	glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	glm::mat4 lightView = glm::lookAt(myCamera.getCameraTarget() + 1.0f * lightDirTr, myCamera.getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));

	//glm::mat4 lightView = glm::lookAt(myCamera.getCameraTarget() + 2.0f * lightDir, myCamera.getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));

	return lightProjection * lightView;
}

void initModels()
{
	//myModel = gps::Model3D("objects/nanosuit/nanosuit.obj", "objects/nanosuit/");
	ground = gps::Model3D("objects/floor/ground.obj", "objects/floor/");

	cathedral = gps::Model3D("objects/arch/catedral/catedral.obj", "objects/arch/catedral/");
	house = gps::Model3D("objects/arch/house/WoodenCabinObj.obj", "objects/arch/house/");
	windmill = gps::Model3D("objects/arch/windmill/windmill.obj", "objects/arch/windmill/");
	house2 = gps::Model3D("objects/arch/house2/gs.obj", "objects/arch/house2/");
	watchTower = gps::Model3D("objects/arch/watchTower/wooden_watch_tower2.obj", "objects/arch/watchTower/");

	tree1 = gps::Model3D("objects/tree/tree.obj", "objects/tree/");
	tree3 = gps::Model3D("objects/trees/tree3/02 (1).obj", "objects/trees/tree3/");

	frog = gps::Model3D("objects/animals/frog/pantest2.obj", "objects/animals/frog/");
	dog = gps::Model3D("objects/animals/dog/Wilczek.obj", "objects/animals/dog/");
	rabbit = gps::Model3D("objects/animals/rabbit/untitled.obj", "objects/animals/rabbit/");
	eagle = gps::Model3D("objects/animals/eagle/White_Eagle.obj", "objects/animals/eagle/");

	boy = gps::Model3D("objects/characters/boy/cartoonboy.obj", "objects/characters/boy/");
	lamp = gps::Model3D("objects/lamp/lamp.obj", "objects/lamp/");

	plant1 = gps::Model3D("objects/plants/snapdragon.obj", "objects/plants/");

	scouter = gps::Model3D("objects/others/scouter/Vespa.obj", "objects/others/scouter/");
	//test = gps::Model3D("objects/test/tree_pack_005.obj", "objects/test/");
}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	depthMapShader.loadShader("shaders/simpleDepthMap.vert", "shaders/simpleDepthMap.frag");
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	myCustomShader.useShaderProgram();
}

void initUniforms()
{

	myCustomShader.useShaderProgram();

	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");

	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");

	lightDirMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDirMatrix");
	lightDirMatrixLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "lightDirMatrix2");

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 5.0f, -8.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	lightDir2 = glm::vec3(0.0f, 1.0f, 1.0f);
	lightDirLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir2");
	glUniform3fv(lightDirLoc2, 1, glm::value_ptr(lightDir2));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	lightColor2 = glm::vec3(1.0f, 0.0f, 0.0f); //white light
	lightColorLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor2");
	glUniform3fv(lightColorLoc2, 1, glm::value_ptr(lightColor2));

	skyboxShader.useShaderProgram();
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

}

void drawBuildings() {
	//draw cathedral
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(0.0f, -2.5f, -8.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	cathedral.Draw(myCustomShader);

	//draw the house
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-200.0f, -25.0f, -200.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	house.Draw(myCustomShader);

	//draw the second house
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(220.0f, -20.0f, -145.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	house2.Draw(myCustomShader);

	//draw the windmill
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-2600.0f, -500.0f, -4000.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	windmill.Draw(myCustomShader);

	//draw the watch tower
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(10.0f, -3.0f, -15.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	watchTower.Draw(myCustomShader);
}

void drawBuildingsShadow() {
	//draw cathedral
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(0.0f, -2.5f, -8.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	cathedral.Draw(depthMapShader);

	//draw the house
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-200.0f, -25.0f, -200.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	house.Draw(depthMapShader);

	//draw the second house
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(220.0f, -20.0f, -145.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	house2.Draw(depthMapShader);

	//draw the windmill
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-2600.0f, -500.0f, -4000.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	windmill.Draw(depthMapShader);

	//draw the watch tower
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(10.0f, -3.0f, -15.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	watchTower.Draw(depthMapShader);
}

void drawTrees() {
	//draw first tree
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-2000.0f, -200.0f, -1200.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	tree1.Draw(myCustomShader);

	//draw second tree
	/*model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	tree2.Draw(myCustomShader);*/

	//draw third tree
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-2000.0f, -500.0f, -4500.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	tree3.Draw(myCustomShader);

	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-1600.0f, -500.0f, -4500.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	tree3.Draw(myCustomShader);

	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-1200.0f, -500.0f, -4500.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	tree3.Draw(myCustomShader);
}

void drawTreesShadow() {
	//draw first tree
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-2000.0f, -200.0f, -1200.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	tree1.Draw(depthMapShader);

	//draw second tree
	/*model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	tree2.Draw(myCustomShader);*/

	//draw third tree
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-2000.0f, -500.0f, -4500.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	tree3.Draw(depthMapShader);

	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-1600.0f, -500.0f, -4500.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	tree3.Draw(depthMapShader);

	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-1200.0f, -500.0f, -4500.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	tree3.Draw(depthMapShader);
}

void drawAnimals() {
	//draw the frog
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(380.0f, -100.0f, -600.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	frog.Draw(myCustomShader);

	//Draw the dog
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-3.7f, -0.4f, -2.9f));
	
	if (dogDirection) {
		delta += 0.05;
		model = glm::translate(model, glm::vec3(0, 0, delta));
		if (delta >= 5) {
			dogDirection = false;
		}
	}
	else {
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
		delta -= 0.05;
		model = glm::translate(model, glm::vec3(0, 0, -delta));
		if (delta <= 0) {
			dogDirection = true;
		}
	}

	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	dog.Draw(myCustomShader);

	//draw the rabbit
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-38.0f, -17.0f, -70.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	rabbit.Draw(myCustomShader);

	//draw the eagle
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
	model = glm::translate(model, glm::vec3(250.0f, 89.0f, 170.0f));
	model = glm::rotate(model, glm::radians(-60.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	eagle.Draw(myCustomShader);
}

void drawAnimalsShadow() {
	//draw the frog
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(380.0f, -100.0f, -600.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	frog.Draw(depthMapShader);

	//Draw the dog
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-3.7f, -0.4f, -2.9f));

	if (dogDirection) {
		delta += 0.05;
		model = glm::translate(model, glm::vec3(0, 0, delta));
		if (delta >= 5) {
			dogDirection = false;
		}
	}
	else {
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
		delta -= 0.05;
		model = glm::translate(model, glm::vec3(0, 0, -delta));
		if (delta <= 0) {
			dogDirection = true;
		}
	}

	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	dog.Draw(depthMapShader);

	//draw the rabbit
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-38.0f, -17.0f, -70.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	rabbit.Draw(depthMapShader);
	
	//draw the eagle
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
	model = glm::translate(model, glm::vec3(250.0f, 89.0f, 170.0f));
	model = glm::rotate(model, glm::radians(-60.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	eagle.Draw(myCustomShader);
}

void drawingsShadow() {
	// draw the boy
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	model = glm::rotate(model, glm::radians(-135.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-100.0f, -100.0f, 700.0f));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	boy.Draw(depthMapShader);

	//draw the lamp
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));
	model = glm::translate(model, glm::vec3(5850.0f, -900.0f, 7950.0f));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	lamp.Draw(depthMapShader);

	//draw the scouter
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
	model = glm::translate(model, glm::vec3(900.0f, -203.0f, -1700.0f));
	model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	scouter.Draw(myCustomShader);
}

void drawings() {
	// draw the boy
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	model = glm::rotate(model, glm::radians(-135.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-100.0f, -100.0f, 700.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	boy.Draw(myCustomShader);

	// draw the lamp
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));
	model = glm::translate(model, glm::vec3(5850.0f, -900.0f, 7950.0f));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	lamp.Draw(myCustomShader);

	// draw the scouter
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
	model = glm::translate(model, glm::vec3(900.0f, -203.0f, -1800.0f));
	model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	scouter.Draw(myCustomShader);

	/*model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	test.Draw(myCustomShader);*/
}

void plants() {
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
	model = glm::translate(model, glm::vec3(1200, -145, 1050));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	plant1.Draw(myCustomShader);

	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
	model = glm::translate(model, glm::vec3(1170, -145, 1020));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	plant1.Draw(myCustomShader);


	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
	model = glm::translate(model, glm::vec3(1210, -145, 1000));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	plant1.Draw(myCustomShader);


	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
	model = glm::translate(model, glm::vec3(1185, -145, 1040));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	plant1.Draw(myCustomShader);


	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
	model = glm::translate(model, glm::vec3(1160, -145, 1045));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	plant1.Draw(myCustomShader);

	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
	model = glm::translate(model, glm::vec3(1200, -145, 1010));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	plant1.Draw(myCustomShader);
}

void plantsShadow() {
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
	model = glm::translate(model, glm::vec3(1200, -145, 1050));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	plant1.Draw(depthMapShader);

	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
	model = glm::translate(model, glm::vec3(1170, -145, 1020));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	plant1.Draw(depthMapShader);


	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
	model = glm::translate(model, glm::vec3(1210, -145, 1000));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	plant1.Draw(depthMapShader);


	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
	model = glm::translate(model, glm::vec3(1185, -145, 1040));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	plant1.Draw(depthMapShader);


	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
	model = glm::translate(model, glm::vec3(1160, -145, 1045));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	plant1.Draw(depthMapShader);

	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
	model = glm::translate(model, glm::vec3(1200, -145, 1010));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	plant1.Draw(depthMapShader);
}

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	processMovement();

	//render the scene to the depth buffer (first pass)
	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTransformMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	drawBuildingsShadow();
	drawTreesShadow();
	drawAnimalsShadow();
	drawingsShadow();
	plantsShadow();

	//create model matrix for gound
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	ground.Draw(depthMapShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//**************************
	//render the scene (second pass)
	myCustomShader.useShaderProgram();

	glViewport(0, 0, retina_width, retina_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE3);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);

	//send lightSpace matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTransformMatrix()));

	//initialize the view matrix
	view = myCamera.getViewMatrix();
	//send view matrix data to shader
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	//compute light direction transformation matrix
	lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
	//send lightDir matrix data to shader
	glUniformMatrix3fv(lightDirMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightDirMatrix));

	glViewport(0, 0, retina_width, retina_height);
	myCustomShader.useShaderProgram();

	//bind the depth map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

	drawBuildings();
	drawTrees();
	drawAnimals();
	drawings();
	plants();

	//create model matrix for gound
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	
	//create normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	ground.Draw(myCustomShader);

	mySkyBox.Draw(skyboxShader, view, projection);
}

int main(int argc, const char * argv[]) {

	initOpenGLWindow();
	initOpenGLState();
	initModels();
	initShaders();
	initUniforms();
	initFBO();
	glCheckError();

	std::vector<const GLchar*> faces;
	faces.push_back("texture/skybox/organic_rt.tga");
	faces.push_back("texture/skybox/organic_lf.tga");
	faces.push_back("texture/skybox/organic_up.tga");
	faces.push_back("texture/skybox/organic_dn.tga");
	faces.push_back("texture/skybox/organic_bk.tga");
	faces.push_back("texture/skybox/organic_ft.tga");

	mySkyBox.Load(faces);

	while (!glfwWindowShouldClose(glWindow)) {
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}
