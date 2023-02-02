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
	vaoTriangle1,
	vaoLen
};

enum bolist {
	boTriangle1,
	boLen
};

struct GLObjects {
	GLuint vao[vaoLen];
	GLuint bo[boLen];
} glo;

glprogram_dl program;

void createProgram() {
	glshader_dl vertexShader;
	glshader_dl geometryShader;
	glshader_dl fragmentShader;

	cout<<glshaderCreate(&vertexShader, GL_VERTEX_SHADER, "shaders/render.vert");
	cout<<glshaderCreate(&geometryShader, GL_GEOMETRY_SHADER, "shaders/render.geom");
	cout<<glshaderCreate(&fragmentShader, GL_FRAGMENT_SHADER, "shaders/render.frag");

	cout<<glprogramCreate(&program, "Render", vector<glshader_dl>({vertexShader, geometryShader, fragmentShader}));

	glshaderDestroy(&vertexShader);
	glshaderDestroy(&geometryShader);
	glshaderDestroy(&fragmentShader);
}

void init(void) {
	createProgram();
	
	float triangleVertexArray[] = {
		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
		0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 1.0f, 1.0f, 0.0f 
	};

	glGenVertexArrays(vaoLen, glo.vao);
	glGenBuffers(boLen, glo.bo);

	glBindVertexArray(glo.vao[vaoTriangle1]);
	glBindBuffer(GL_ARRAY_BUFFER, glo.bo[boTriangle1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * len(triangleVertexArray), triangleVertexArray, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2) + sizeof(vec3), (void*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec2) + sizeof(vec3), (void*)(sizeof(vec2)));
}

void render(void) {
	glClearBufferfv(GL_COLOR, 0, vec4(0.2f, 0.3f, 0.3f, 1.0f));
	glViewport(0, 0, (GLsizei)winSize.w, (GLsizei)winSize.h);
	glUseProgram(program.programObject);
	glBindVertexArray(glo.vao[vaoTriangle1]);
	glDrawArrays(GL_POINTS, 0, 4);
}

void uninit(void) {

}

void keyboard(unsigned int key, int state) {
	switch(key) {
	case 'F': case 'f':
		toggleFullscreen();
		break;
	case DL_ESCAPE:
		closeOpenGLWindow();
		break;
	}
}

int main(void) {
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
