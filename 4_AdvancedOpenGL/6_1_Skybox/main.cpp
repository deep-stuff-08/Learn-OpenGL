#include<iostream>
#include<string>
#include<vector>

#include<GL/glew.h>

#include<glwindowing.h>
#include<vmath.h>
#include<glshaderloader.h>
#include<gltextureloader.h>

using namespace std;
using namespace vmath;

#define len(x) sizeof(x) / sizeof(x[0])

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

enum vaolist {
	vaoCube,
	vaoSkybox,
	vaoLen
};

enum bolist {
	vboCube,
	vboSkybox,
	boLen
};

enum texlist {
	texContainer,
	texSkybox,
	texLen
};

struct GLObjects {
	GLuint vao[vaoLen];
	GLuint bo[boLen];
	GLuint tex[texLen];
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
	vec3(0.0f, 0.0f, 4.0f),
	vec3(0.0f, 0.0f, -1.0f),
	vec3(0.0f, 1.0f, 0.0f)
};

glprogram_dl program;
glprogram_dl programSky;

void createProgram() {
	glshader_dl vertexShader;
	glshader_dl fragmentShader;
	cout<<glshaderCreate(&vertexShader, GL_VERTEX_SHADER, "shaders/render.vert");
	cout<<glshaderCreate(&fragmentShader, GL_FRAGMENT_SHADER, "shaders/render.frag");
	
	cout<<glprogramCreate(&program, "Render", vector<glshader_dl>({vertexShader, fragmentShader}));

	glshaderDestroy(&vertexShader);
	glshaderDestroy(&fragmentShader);

	cout<<glshaderCreate(&vertexShader, GL_VERTEX_SHADER, "shaders/skybox.vert");
	cout<<glshaderCreate(&fragmentShader, GL_FRAGMENT_SHADER, "shaders/skybox.frag");

	cout<<glprogramCreate(&programSky, "Skybox", vector<glshader_dl>({vertexShader, fragmentShader}));

	glshaderDestroy(&vertexShader);
	glshaderDestroy(&fragmentShader);
}

void init(void) {
	createProgram();

	float triangleVertexArray[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	float skyboxVertices[] = {
	// positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(vaoLen, glo.vao);
	glGenBuffers(boLen, glo.bo);
	glGenTextures(texLen, glo.tex);

	glBindVertexArray(glo.vao[vaoCube]);
	glBindBuffer(GL_ARRAY_BUFFER, glo.bo[vboCube]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * len(triangleVertexArray), triangleVertexArray, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) + sizeof(vec2), (void*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vec3) + sizeof(vec2), (void*)(sizeof(vec3)));

	glBindVertexArray(glo.vao[vaoSkybox]);
	glBindBuffer(GL_ARRAY_BUFFER, glo.bo[vboSkybox]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * len(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)(0));
	
	cout<<createTexture2D(&glo.tex[texContainer], "../../resources/container.jpg");
	unsigned char* data;
	int w, h;
	glBindTexture(GL_TEXTURE_CUBE_MAP, glo.tex[texSkybox]);
	cout<<loadTextureData("../../resources/skybox/right.jpg", &data, &w, &h);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	cout<<loadTextureData("../../resources/skybox/left.jpg", &data, &w, &h);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	cout<<loadTextureData("../../resources/skybox/bottom.jpg", &data, &w, &h);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	cout<<loadTextureData("../../resources/skybox/top.jpg", &data, &w, &h);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	cout<<loadTextureData("../../resources/skybox/front.jpg", &data, &w, &h);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	cout<<loadTextureData("../../resources/skybox/back.jpg", &data, &w, &h);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

void render(void) {
	float currentTime = getTime();
	gltime.delta = currentTime - gltime.last;
	gltime.last = currentTime;

	glClearBufferfv(GL_COLOR, 0, vec4(0.2f, 0.3f, 0.3f, 1.0f));
	glClearBufferfv(GL_DEPTH, 0, vec1(1.0f));
	glViewport(0, 0, (GLsizei)winSize.w, (GLsizei)winSize.h);
	glUseProgram(program.programObject);
	glBindVertexArray(glo.vao[vaoCube]);
	glUniformMatrix4fv(0, 1, GL_FALSE, perspective(30.0f, winSize.w / winSize.h, 0.1f, 100.0f)); 
	glUniformMatrix4fv(1, 1, GL_FALSE, lookat(glcamera.position, glcamera.position + glcamera.front, glcamera.up));
	glBindVertexArray(glo.vao[vaoCube]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texContainer]); 	
	glUniformMatrix4fv(2, 1, GL_FALSE, mat4::identity());
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	glUseProgram(programSky.programObject);
	glUniformMatrix4fv(0, 1, GL_FALSE, perspective(30.0f, winSize.w / winSize.h, 0.1f, 100.0f)); 
	glUniformMatrix4fv(1, 1, GL_FALSE, lookat(vec3(0.0f, 0.0f, 0.0f), glcamera.front, glcamera.up));
	glBindVertexArray(glo.vao[vaoSkybox]);
	glBindTexture(GL_TEXTURE_CUBE_MAP, glo.tex[texSkybox]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void uninit(void) {

}
void keyboard(unsigned int key, int state) {
	float speed = 2.5f * gltime.delta;
	static bool isDepth = true;
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
	setKeyboardFunc(keyboard);
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
