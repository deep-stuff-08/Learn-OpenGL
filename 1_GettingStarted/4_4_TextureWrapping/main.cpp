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
	vaoTriangle,
	vaoLen
};

enum bolist {
	vboTriangle,
	eaboTriangle,
	boLen
};

enum texlist {
	texCardboard,
	texSmiley,
	texLen
};

struct GLObjects {
	GLuint vao[vaoLen];
	GLuint bo[boLen];
	GLuint tex[texLen];
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
		vec2(1.0f,  1.0f),		vec2(1.0f,  1.0f),
		vec2(1.0f, -1.0f),		vec2(1.0f, -1.0f),
		vec2(-1.0f, -1.0f),		vec2(-1.0f, -1.0f),
		vec2(-1.0f,  1.0f),		vec2(-1.0f,  1.0f)
	};

	uvec3 triangleIndexArray[] = {
		uvec3(0, 1, 3),
		uvec3(1, 2, 3)
	};

	glGenVertexArrays(vaoLen, glo.vao);
	glGenBuffers(boLen, glo.bo);
	glGenTextures(texLen, glo.tex);

	glBindVertexArray(glo.vao[vaoTriangle]);
	glBindBuffer(GL_ARRAY_BUFFER, glo.bo[vboTriangle]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * len(triangleVertexArray), triangleVertexArray, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2) * 2, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vec2) * 2, (void*)(sizeof(vec2)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glo.bo[eaboTriangle]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uvec3) * len(triangleIndexArray), triangleIndexArray, GL_STATIC_DRAW);

	createTexture2D(&glo.tex[texCardboard], "../../resources/container.jpg");
	createTexture2D(&glo.tex[texSmiley], "../../resources/awesomeface.png");
}

void render(void) {
	glClearBufferfv(GL_COLOR, 0, vec4(0.2f, 0.3f, 0.3f, 1.0f));
	glViewport(0, 0, (GLsizei)winSize.w, (GLsizei)winSize.h);
	glUseProgram(program.programObject);
	glUniformMatrix4fv(0, 1, GL_FALSE, perspective(45.0f, winSize.w / winSize.h, 0.1f, 100.0f) * translate(0.0f, 0.0f, -5.0f));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texSmiley]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texCardboard]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
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
