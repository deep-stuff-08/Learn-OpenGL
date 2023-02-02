#include<glwindowing.h>
#include<glshaderloader.h>
#include<gltextureloader.h>
#include<glmodelloader.h>
#include<iostream>
#include<vmath.h>
#include<assimp/postprocess.h>

using namespace std;
using namespace vmath;

glmodel_dl model;
glprogram_dl program;
string directory;

struct GLTimer {
	double delta;
	double last;	
} gltime = {0.0f, 0.0f};

struct GLCamera {
	vec3 position;
	vec3 front;
	vec3 up;
} glcamera = {
	vec3(0.0f, 0.0f, 5.0f),
	vec3(0.0f, 0.0f, -1.0f),
	vec3(0.0f, 1.0f, 0.0f)
};

void setupProgram() {
	glshader_dl vertexShader, geometryShader, fragmentShader;

	cout<<glshaderCreate(&vertexShader, GL_VERTEX_SHADER, "shaders/draw.vert", DL_SHADER_CORE, 460);
	cout<<glshaderCreate(&geometryShader, GL_GEOMETRY_SHADER, "shaders/draw.geom", DL_SHADER_CORE, 460);
	cout<<glshaderCreate(&fragmentShader, GL_FRAGMENT_SHADER, "shaders/draw.frag", DL_SHADER_CORE, 460);

	cout<<glprogramCreate(&program, "Draw", { vertexShader, geometryShader, fragmentShader });

	glshaderDestroy(&vertexShader);
	glshaderDestroy(&fragmentShader);
}

void init(void) {
	setupProgram();

	// createModel(&model, "../../models/cyborg/cyborg.obj", aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_CalcTangentSpace, 0, 1, 2);
	createModel(&model, "../../models/backpack/backpack.obj", aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_CalcTangentSpace, 0, 1, 2);

	glEnable(GL_DEPTH_TEST);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void render(void) {
	float currentTime = getTime();
	gltime.delta = currentTime - gltime.last;
	gltime.last = currentTime;

	static float time = 0.0f;
	time += gltime.delta * 4.0f;

	glClearBufferfv(GL_COLOR, 0, vec4(0.1f, 0.3f, 0.3f, 1.0f));
	glClearBufferfv(GL_DEPTH, 0, vec1(1.0f));

	glViewport(0, 0, winSize.w, winSize.h);

	glUseProgram(program.programObject);
	glUniformMatrix4fv(0, 1, GL_FALSE, perspective(45.0f, winSize.w / winSize.h, 0.1f, 100.0f));
	glUniformMatrix4fv(1, 1, GL_FALSE, lookat(glcamera.position, glcamera.front + glcamera.position, glcamera.up));
	glUniformMatrix4fv(2, 1, GL_FALSE, rotate((float)getTime() * 40.0f, 1.0f, 1.0f, 0.0f));// * translate(0.0f, -1.5f, 0.0f));
	glUniform1f(3, time);
	glUniform1f(4, 2.0f);
	
	drawModel(&model);
}

void uninit(void) {

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
