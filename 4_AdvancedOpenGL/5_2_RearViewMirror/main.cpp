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

#define FBO_SIZE 1024
#define len(x) sizeof(x) / sizeof(x[0])

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

enum vaolist {
	vaoCube,
	vaoPlane,
	vaoMirror,
	vaoLen
};

enum bolist {
	vboCube,
	vboPlane,
	vboMirror,
	boLen
};

enum texlist {
	texContainer,
	texMetal,
	texSceneColor,
	texLen
};

enum fbolist {
	fboScene,
	fboLen
};

enum rbolist {
	rboSceneDepth,
	rboLen
};

struct GLObjects {
	GLuint vao[vaoLen];
	GLuint bo[boLen];
	GLuint tex[texLen];
	GLuint fbo[fboLen];
	GLuint rbo[rboLen];
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
int currentScene = 0;

void createProgram() {
	glshader_dl vertexShader;
	glshader_dl fragmentShader;

	cout<<glshaderCreate(&vertexShader, GL_VERTEX_SHADER, "shaders/render.vert");
	cout<<glshaderCreate(&fragmentShader, GL_FRAGMENT_SHADER, "shaders/render.frag");

	cout<<glprogramCreate(&program, "Render", vector<glshader_dl>({vertexShader, fragmentShader}));
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
	float quadVertices[] = {
		-0.3f,  1.0f, 0.0f, 0.0f, 1.0f,
		-0.3f,  0.7f, 0.0f, 0.0f, 0.0f,
		0.3f,  0.7f, 0.0f, 1.0f, 0.0f,

		-0.3f,  1.0f, 0.0f, 0.0f, 1.0f,
		0.3f,  0.7f, 0.0f, 1.0f, 0.0f,
		0.3f,  1.0f, 0.0f, 1.0f, 1.0f
    };

	glGenVertexArrays(vaoLen, glo.vao);
	glGenBuffers(boLen, glo.bo);
	glGenTextures(texLen, glo.tex);
	glGenFramebuffers(fboLen, glo.fbo);
	glGenRenderbuffers(rboLen, glo.rbo);

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

	glBindVertexArray(glo.vao[vaoMirror]);
	glBindBuffer(GL_ARRAY_BUFFER, glo.bo[vboMirror]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * len(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) + sizeof(vec2), (void*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vec3) + sizeof(vec2), (void*)(sizeof(vec3)));

	cout<<createTexture2D(&glo.tex[texContainer], "../../resources/container.jpg");
	cout<<createTexture2D(&glo.tex[texMetal], "../../resources/metal.png");

	glBindFramebuffer(GL_FRAMEBUFFER, glo.fbo[fboScene]);
	
	glBindRenderbuffer(GL_RENDERBUFFER, glo.rbo[rboSceneDepth]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, FBO_SIZE, FBO_SIZE);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glo.rbo[rboSceneDepth]);

	glBindTexture(GL_TEXTURE_2D, glo.tex[texSceneColor]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, FBO_SIZE, FBO_SIZE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glo.tex[texSceneColor], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout<<"error : Framebuffer is not complete!"<<endl;
	}
}

void render(void) {
	float currentTime = getTime();
	gltime.delta = currentTime - gltime.last;
	gltime.last = currentTime;

	glBindFramebuffer(GL_FRAMEBUFFER, glo.fbo[fboScene]);
	glEnable(GL_DEPTH_TEST);
	glClearBufferfv(GL_COLOR, 0, vec4(0.2f, 0.3f, 0.3f, 1.0f));
	glClearBufferfv(GL_DEPTH, 0, vec1(1.0f));
	glViewport(0, 0, FBO_SIZE, FBO_SIZE);
	glUseProgram(program.programObject);
	glBindVertexArray(glo.vao[vaoCube]);
	glUniformMatrix4fv(0, 1, GL_FALSE, perspective(30.0f, winSize.w / winSize.h, 0.1f, 100.0f)); 
	glUniformMatrix4fv(1, 1, GL_FALSE, lookat(glcamera.position, glcamera.position - glcamera.front, glcamera.up));
	glBindVertexArray(glo.vao[vaoCube]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texContainer]); 	
	glUniformMatrix4fv(2, 1, GL_FALSE, translate(-1.0f, 0.0f, -1.0f));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniformMatrix4fv(2, 1, GL_FALSE, translate(2.0f, 0.0f, 0.0f));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	glBindVertexArray(glo.vao[vaoPlane]);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texMetal]);
	glUniformMatrix4fv(2, 1, GL_FALSE, mat4::identity());
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearBufferfv(GL_COLOR, 0, vec4(0.2f, 0.3f, 0.3f, 1.0f));
	glClearBufferfv(GL_DEPTH, 0, vec1(1.0f));
	glViewport(0, 0, winSize.w, winSize.h);
	glUseProgram(program.programObject);
	glBindVertexArray(glo.vao[vaoCube]);
	glUniformMatrix4fv(0, 1, GL_FALSE, perspective(30.0f, winSize.w / winSize.h, 0.1f, 100.0f)); 
	glUniformMatrix4fv(1, 1, GL_FALSE, lookat(glcamera.position, glcamera.position + glcamera.front, glcamera.up));
	glBindVertexArray(glo.vao[vaoCube]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texContainer]); 	
	glUniformMatrix4fv(2, 1, GL_FALSE, translate(-1.0f, 0.0f, -1.0f));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniformMatrix4fv(2, 1, GL_FALSE, translate(2.0f, 0.0f, 0.0f));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	glBindVertexArray(glo.vao[vaoPlane]);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texMetal]);
	glUniformMatrix4fv(2, 1, GL_FALSE, mat4::identity());
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUniformMatrix4fv(0, 1, GL_FALSE, mat4::identity());
	glUniformMatrix4fv(1, 1, GL_FALSE, mat4::identity());
	glBindVertexArray(glo.vao[vaoMirror]);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texSceneColor]);
	glUniformMatrix4fv(2, 1, GL_FALSE, mat4::identity());
	glDrawArrays(GL_TRIANGLES, 0, 6);
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
	case '1': case '2': case '3': case '4': case '5':
		currentScene = key - '1';
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
	static float yaw = -90.0f;
	static float pitch;

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
