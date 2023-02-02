#include<iostream>
#include<string>
#include<vector>

#include<GL/glew.h>

#include<glwindowing.h>
#include<vmath.h>

using namespace std;
using namespace vmath;

#define len(x) sizeof(x) / sizeof(x[0])

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

void init(void) {
}

void render(void) {
	glViewport(0, 0, winSize.w, winSize.h);
	glClearBufferfv(GL_COLOR, 0, vec4(0.2f, 0.3f, 0.3f, 1.0f));
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
