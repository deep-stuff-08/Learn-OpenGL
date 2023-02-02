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
	vaoTriangle,
	vaoLen
};

enum bolist {
	boTriangle,
	boLen
};

struct GLObjects {
	GLuint vao[vaoLen];
	GLuint bo[boLen];
} glo;

glprogram_dl program;

void createProgram() {
	glshader_dl vertexShader;
	glshader_dl fragmentShader;

	cout<<glshaderCreate(&vertexShader, GL_VERTEX_SHADER, "shaders/render.vert");
	cout<<glshaderCreate(&fragmentShader, GL_FRAGMENT_SHADER, "shaders/render.frag");

	cout<<glprogramCreate(&program, "Render", vector<glshader_dl>({vertexShader, fragmentShader}));
}

void init(void) {
	createProgram();
	
	vec2 triangleVertexArray[] = {
		// Triangle 1
		vec2(-1.0f, 1.0f),
		vec2(-2.0f, -1.0f),
		vec2(0.0f, -1.0f),

		// //Triangle 2
		vec2(1.0f, 1.0f),
		vec2(0.0f, -1.0f),
		vec2(2.0f, -1.0f)
	};

	glGenVertexArrays(vaoLen, glo.vao);
	glGenBuffers(boLen, glo.bo);

	glBindVertexArray(glo.vao[vaoTriangle]);
	glBindBuffer(GL_ARRAY_BUFFER, glo.bo[boTriangle]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * len(triangleVertexArray), triangleVertexArray, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

void render(void) {
	glClearBufferfv(GL_COLOR, 0, vec4(0.2f, 0.3f, 0.3f, 1.0f));
	glViewport(0, 0, (GLsizei)winSize.w, (GLsizei)winSize.h);
	glUseProgram(program.programObject);
	glUniformMatrix4fv(0, 1, GL_FALSE, perspective(45.0f, winSize.w / winSize.h, 0.1f, 100.0f) * translate(0.0f, 0.0f, -5.0f));
	glDrawArrays(GL_TRIANGLES, 0, 6);
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
