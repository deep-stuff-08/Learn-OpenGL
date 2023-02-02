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
	vaoTriangle2,
	vaoLen
};

enum bolist {
	boTriangle1,
	boTriangle2,
	boLen
};

struct GLObjects {
	GLuint vao[vaoLen];
	GLuint bo[boLen];
} glo;

glprogram_dl programOrange;
glprogram_dl programYellow;

void createProgram() {
	glshader_dl vertexShader;
	glshader_dl fragmentShader1;
	glshader_dl fragmentShader2;

	cout<<glshaderCreate(&vertexShader, GL_VERTEX_SHADER, "shaders/render.vert");
	cout<<glshaderCreate(&fragmentShader1, GL_FRAGMENT_SHADER, "shaders/render1.frag");
	cout<<glshaderCreate(&fragmentShader2, GL_FRAGMENT_SHADER, "shaders/render2.frag");

	cout<<glprogramCreate(&programOrange, "Render", vector<glshader_dl>({vertexShader, fragmentShader1}));
	cout<<glprogramCreate(&programYellow, "Render", vector<glshader_dl>({vertexShader, fragmentShader2}));
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

	glBindVertexArray(glo.vao[vaoTriangle1]);
	glBindBuffer(GL_ARRAY_BUFFER, glo.bo[boTriangle1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * len(triangleVertexArray) / 2, triangleVertexArray, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindVertexArray(glo.vao[vaoTriangle2]);
	glBindBuffer(GL_ARRAY_BUFFER, glo.bo[boTriangle2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * len(triangleVertexArray) / 2, triangleVertexArray + 3, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

void render(void) {
	glClearBufferfv(GL_COLOR, 0, vec4(0.2f, 0.3f, 0.3f, 1.0f));
	glViewport(0, 0, (GLsizei)winSize.w, (GLsizei)winSize.h);
	glUseProgram(programOrange.programObject);
	glUniformMatrix4fv(0, 1, GL_FALSE, perspective(45.0f, winSize.w / winSize.h, 0.1f, 100.0f) * translate(0.0f, 0.0f, -5.0f));
	glBindVertexArray(glo.vao[vaoTriangle1]);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glUseProgram(programYellow.programObject);
	glUniformMatrix4fv(0, 1, GL_FALSE, perspective(45.0f, winSize.w / winSize.h, 0.1f, 100.0f) * translate(0.0f, 0.0f, -5.0f));
	glBindVertexArray(glo.vao[vaoTriangle2]);
	glDrawArrays(GL_TRIANGLES, 0, 3);
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
