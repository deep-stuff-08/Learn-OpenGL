#include<GL/glew.h>

#include<glwindowing.h>
#include<glshaderloader.h>
#include<gllighting.h>
#include<glmodelloader.h>

#include<iostream>
#include<vmath.h>

#include<assimp/postprocess.h>

using namespace std;
using namespace vmath;

struct GLTimer {
	double delta;
	double last;	
} gltime = {0.0f, 0.0f};

struct GLCamera {
	vec3 position;
	vec3 front;
	vec3 up;
} glcamera = {
	vec3(0.0f, 0.0f, 3.0f),
	vec3(0.0f, 0.0f, -1.0f),
	vec3(0.0f, 1.0f, 0.0f)
};

glmodel_dl model;
glprogram_dl program;

void init() {
	glEnable(GL_DEPTH_TEST);
	glshader_dl vertex, fragment;
	glshader_dl lighting;

	cout<<compileLightingShader(&lighting);

	cout<<glshaderCreate(&vertex, GL_VERTEX_SHADER, "shaders/draw.vert");	
	cout<<glshaderCreate(&fragment, GL_FRAGMENT_SHADER, "shaders/draw.frag");

	cout<<glprogramCreate(&program, "Render", {vertex, lighting, fragment});

	glshaderDestroy(&vertex);
	glshaderDestroy(&fragment);

	cout<<createModel(&model, "../../models/vampire/dancing_vampire.dae", aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace, 0, 1, 2, -1, -1, 3, 4);
}

void render() {
	float currentTime = getTime();
	gltime.delta = currentTime - gltime.last;
	gltime.last = currentTime;

	cout<<updateModel(&model, gltime.delta, 0);
	
	glClearColor(0.45f, 0.25f, 0.75f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, winSize.w, winSize.h);

	glUseProgram(program.programObject);

	glUniformMatrix4fv(0, 1, GL_FALSE, perspective(45.0f, (float)winSize.w / (float)winSize.h, 0.1f, 100.0f));
	glUniformMatrix4fv(1, 1, GL_FALSE, lookat(glcamera.position, glcamera.position + glcamera.front, glcamera.up));
	glUniformMatrix4fv(2, 1, GL_FALSE, translate(0.0f, -0.4f, 0.0f) * scale(0.5f));
	setBoneMatrixUniform(&model, 0, 3);
	glUniform3fv(104, 1, glcamera.position);

	drawModel(&model);
}

void keyboard(unsigned int key, int state) {
	float speed = 7.5f * gltime.delta;
	switch(key) {
	case 'W': case 'w':
		glcamera.position += speed * glcamera.front;
		break;
	case 'S': case 's':
		glcamera.position -= speed * glcamera.front;
		break;
	case 'D': case 'd':
		glcamera.position += speed * normalize(cross(glcamera.front, glcamera.up));
		break;
	case 'A': case 'a':
		glcamera.position -= speed * normalize(cross(glcamera.front, glcamera.up));
		break;
	case 'F': case 'f':
		toggleFullscreen();
		break;
	case DL_ESCAPE:
		closeOpenGLWindow();
		break;
	}
}

void mouse(int x, int y) {
	static bool isFirst = true;
	static float lastX;
	static float lastY;

	static float pitch;
	static float yaw = -90.0f;

	if(isFirst) {
		lastX = x;
		lastY = y;
		isFirst = false;
	}

	float xoffset = x - lastX;
	float yoffset = lastY - y; 
	lastX = x;
	lastY = y;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw   += xoffset;
	pitch += yoffset;

	if(pitch > 89.0f) {
		pitch = 89.0f;
	} else if(pitch < -89.0f) {
		pitch = -89.0f;
	}
	vec3 direction = vec3(cos(radians(yaw)) * cos(radians(pitch)), sin(radians(pitch)), sin(radians(yaw)) * cos(radians(pitch)));
	glcamera.front = normalize(direction);
}

void uninit() {
}

int main(void) {
	setKeyboardFunc(keyboard);
	setMouseFunc(mouse);
	createOpenGLWindow();
	init();
	while(!isOpenGLWindowClosed()) {
		processEvents();
		render();
		swapBuffers();
	}
	uninit();
	destroyOpenGLWindow();
}
