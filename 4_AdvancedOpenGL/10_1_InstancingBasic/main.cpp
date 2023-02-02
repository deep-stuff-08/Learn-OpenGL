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
	vboOffset,
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

	glshaderDestroy(&vertexShader);
	glshaderDestroy(&fragmentShader);
}

void init(void) {
	createProgram();
	
	float triangleVertexArray[] = {
		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		0.05f,  0.05f,  0.0f, 1.0f, 1.0f
	};

	vec2 translations[100];
	int index = 0;
	float offset = 0.1f;
	for (int y = -10; y < 10; y += 2)
	{
		for (int x = -10; x < 10; x += 2)
		{
			vec2 translation;
			translation[0] = (float)x / 10.0f + offset;
			translation[1] = (float)y / 10.0f + offset;
			translations[index++] = translation;
		}
	}
	
	glGenVertexArrays(vaoLen, glo.vao);
	glGenBuffers(boLen, glo.bo);

	glBindVertexArray(glo.vao[vaoTriangle1]);
	glBindBuffer(GL_ARRAY_BUFFER, glo.bo[boTriangle1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * len(triangleVertexArray), triangleVertexArray, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2) + sizeof(vec3), (void*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec2) + sizeof(vec3), (void*)(sizeof(vec2)));
	glBindBuffer(GL_ARRAY_BUFFER, glo.bo[vboOffset]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * len(translations), translations, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(0));
	glVertexAttribDivisor(2, 1);
}

void render(void) {
	glClearBufferfv(GL_COLOR, 0, vec4(0.2f, 0.3f, 0.3f, 1.0f));
	glViewport(0, 0, (GLsizei)winSize.w, (GLsizei)winSize.h);
	glUseProgram(program.programObject);
	glBindVertexArray(glo.vao[vaoTriangle1]);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
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
