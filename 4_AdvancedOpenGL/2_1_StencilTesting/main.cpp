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
	vaoPlane,
	vaoLen
};

enum bolist {
	vboCube,
	vboPlane,
	boLen
};

enum texlist {
	texMarble,
	texMetal,
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
glprogram_dl programBorder;

void createProgram() {
	glshader_dl vertexShader;
	glshader_dl fragmentShader;
	glshader_dl fragmentBorderShader;

	cout<<glshaderCreate(&vertexShader, GL_VERTEX_SHADER, "shaders/render.vert");
	cout<<glshaderCreate(&fragmentShader, GL_FRAGMENT_SHADER, "shaders/render.frag");
	cout<<glshaderCreate(&fragmentBorderShader, GL_FRAGMENT_SHADER, "shaders/border.frag");

	cout<<glprogramCreate(&program, "Render", vector<glshader_dl>({vertexShader, fragmentShader}));

	cout<<glprogramCreate(&programBorder, "Border", vector<glshader_dl>({vertexShader, fragmentBorderShader}));

	glshaderDestroy(&vertexShader);
	glshaderDestroy(&fragmentShader);
	glshaderDestroy(&fragmentBorderShader);
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
	float planeVertices[] = {
		5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		5.0f, -0.5f, -5.0f,  2.0f, 2.0f								
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

	glBindVertexArray(glo.vao[vaoPlane]);
	glBindBuffer(GL_ARRAY_BUFFER, glo.bo[vboPlane]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * len(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) + sizeof(vec2), (void*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vec3) + sizeof(vec2), (void*)(sizeof(vec3)));

	cout<<createTexture2D(&glo.tex[texMarble], "../../resources/marble.jpg");
	cout<<createTexture2D(&glo.tex[texMetal], "../../resources/metal.png");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}

void render(void) {
	float currentTime = getTime();
	gltime.delta = currentTime - gltime.last;
	gltime.last = currentTime;

	float near = 0.1f;
	float far = 100.0f;

	glClearBufferfv(GL_COLOR, 0, vec4(0.1f, 0.1f, 0.1f, 1.0f));
	glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);
	glViewport(0, 0, (GLsizei)winSize.w, (GLsizei)winSize.h);
	
	glUseProgram(program.programObject);
	glStencilMask(0x00);
	glUniformMatrix4fv(0, 1, GL_FALSE, perspective(45.0f, winSize.w / winSize.h, near, far)); 
	glUniformMatrix4fv(1, 1, GL_FALSE, lookat(glcamera.position, glcamera.position + glcamera.front, glcamera.up));
	glBindVertexArray(glo.vao[vaoPlane]);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texMetal]);
	glUniformMatrix4fv(2, 1, GL_FALSE, mat4::identity());
	glDrawArrays(GL_TRIANGLES, 0, 6);

	
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	glBindVertexArray(glo.vao[vaoCube]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texMarble]); 	
	glUniformMatrix4fv(2, 1, GL_FALSE, translate(-1.0f, 0.0f, -1.0f));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniformMatrix4fv(2, 1, GL_FALSE, translate(2.0f, 0.0f, 0.0f));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	glUseProgram(programBorder.programObject);
	glUniformMatrix4fv(0, 1, GL_FALSE, perspective(45.0f, winSize.w / winSize.h, near, far)); 
	glUniformMatrix4fv(1, 1, GL_FALSE, lookat(glcamera.position, glcamera.position + glcamera.front, glcamera.up));
	glBindVertexArray(glo.vao[vaoCube]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texMarble]); 	
	glUniformMatrix4fv(2, 1, GL_FALSE, translate(-1.0f, 0.0f, -1.0f) * scale(1.1f));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniformMatrix4fv(2, 1, GL_FALSE, translate(2.0f, 0.0f, 0.0f) * scale(1.1f));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glEnable(GL_DEPTH_TEST);
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
