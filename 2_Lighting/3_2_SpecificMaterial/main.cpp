#include<iostream>
#include<string>
#include<vector>

#include<GL/glew.h>

#include<glwindowing.h>
#include<vmath.h>
#include<glshaderloader.h>

using namespace std;
using namespace vmath;

#define len(x) sizeof(x) / sizeof(x[0])

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

enum vaolist {
	vaoCube,
	vaoLen
};

enum bolist {
	vboCube,
	boLen
};

struct GLObjects {
	GLuint vao[vaoLen];
	GLuint bo[boLen];
} glo;

struct GLTimer {
	double delta;
	double last;	
} gltime = {0.0f, 0.0f};

struct GLCamera {
	vec3 position;
	vec3 front;
	vec3 up;
} glcamera = {
	vec3(0.0f, 0.0f, 7.0f),
	vec3(0.0f, 0.0f, -1.0f),
	vec3(0.0f, 1.0f, 0.0f)
};

vec3 lightPos = vec3(0.6f, 0.5f, 2.0f);

glprogram_dl lightProgram;
glprogram_dl diffuseProgram;

void createProgram() {
	glshader_dl vertexLightShader;
	glshader_dl fragmentLightShader;

	glshader_dl vertexDiffuseShader;
	glshader_dl fragmentDiffuseShader;

	cout<<glshaderCreate(&vertexLightShader, GL_VERTEX_SHADER, "shaders/light.vert");	
	cout<<glshaderCreate(&fragmentLightShader, GL_FRAGMENT_SHADER, "shaders/light.frag");

	cout<<glprogramCreate(&lightProgram, "Light", vector<glshader_dl>({vertexLightShader, fragmentLightShader}));

	cout<<glshaderCreate(&vertexDiffuseShader, GL_VERTEX_SHADER, "shaders/material.vert");
	cout<<glshaderCreate(&fragmentDiffuseShader, GL_FRAGMENT_SHADER, "shaders/material.frag");

	cout<<glprogramCreate(&diffuseProgram, "Material", vector<glshader_dl>({vertexDiffuseShader, fragmentDiffuseShader}));
}

void init(void) {
	createProgram();
	
	float triangleVertexArray[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	glGenVertexArrays(vaoLen, glo.vao);
	glGenBuffers(boLen, glo.bo);
	
	glBindVertexArray(glo.vao[vaoCube]);
	glBindBuffer(GL_ARRAY_BUFFER, glo.bo[vboCube]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * len(triangleVertexArray), triangleVertexArray, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) + sizeof(vec3), (void*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) + sizeof(vec3), (void*)(sizeof(vec3)));

	glEnable(GL_DEPTH_TEST);
}

void render(void) {
	float currentTime = getTime();
	gltime.delta = currentTime - gltime.last;
	gltime.last = currentTime;

	glClearBufferfv(GL_COLOR, 0, vec4(0.1f, 0.1f, 0.1f, 1.0f));
	glClearBufferfv(GL_DEPTH, 0, vec1(1.0f));
	glViewport(0, 0, (GLsizei)winSize.w, (GLsizei)winSize.h);
	
	glBindVertexArray(glo.vao[vaoCube]);
	
	mat4 perspectiveMatrix = perspective(45.0f, winSize.w / winSize.h, 0.1f, 100.0f);
	mat4 viewMatrix = lookat(glcamera.position, glcamera.position + glcamera.front, glcamera.up);

	glUseProgram(diffuseProgram.programObject);
	glUniformMatrix4fv(0, 1, GL_FALSE, perspectiveMatrix); 
	glUniformMatrix4fv(1, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(2, 1, GL_FALSE, mat4::identity());
	glUniform3fv(3, 1, glcamera.position);
	glUniform3fv(4, 1, lightPos);
	glUniform3f(5, 1.0f, 1.0f, 1.0f);
	glUniform3f(6, 1.0f, 1.0f, 1.0f);
	glUniform3f(7, 1.0f, 1.0f, 1.0f);
	glUniform3f(8, 0.0f, 0.1f, 0.06f);
	glUniform3f(9, 0.0f, 0.50980392f, 0.50980392f);
	glUniform3f(10, 0.50196078f, 0.50196078f, 0.50196078f);
	glUniform1f(11, 32.0f);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glUseProgram(lightProgram.programObject);
	glUniformMatrix4fv(0, 1, GL_FALSE, perspectiveMatrix); 
	glUniformMatrix4fv(1, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(2, 1, GL_FALSE, translate(lightPos) * scale(0.2f));
	glUniform3f(3, 1.0f, 1.0f, 1.0f);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void uninit(void) {

}

void keyboard(unsigned int key, int state) {
	float speed = 2.5f * gltime.delta;
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
	toggleFullscreen();
	while(!isOpenGLWindowClosed()) {
		processEvents();
		render();
		swapBuffers();
	}
	uninit();
	destroyOpenGLWindow();
}
