#include<iostream>
#include<string>
#include<vector>

#include<GL/glew.h>

#include<glwindowing.h>
#include<vmath.h>
#include<glshaderloader.h>
#include<gltextureloader.h>
#include<fullscreenquad.h>

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

enum fboList {
	fboHdr,
	fboLen
};

enum texList {
	texWood,
	texHDRColorBuffer,
	texLen
};

enum rboList {
	rboHDRDepthBuffer,
	rboLen
};

struct GLObjects {
	GLuint vao[vaoLen];
	GLuint bo[boLen];
	GLuint fbo[fboLen];
	GLuint tex[texLen];
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
	vec3(0.0f, 0.0f, -2.0f),
	vec3(0.0f, 0.0f, 1.0f),
	vec3(0.0f, 1.0f, 0.0f)
};

glprogram_dl lightingProgram;
glprogram_dl hdrProgram;

fsquad_dl fullscreenQuad;

GLboolean hdr = GL_FALSE;
GLfloat exposure = 1.0f;

void createProgram() {
	glshader_dl lightingVertexShader, lightingFragmentShader;

	cout<<glshaderCreate(&lightingVertexShader, GL_VERTEX_SHADER, "shaders/lighting.vert");
	cout<<glshaderCreate(&lightingFragmentShader, GL_FRAGMENT_SHADER, "shaders/lighting.frag");

	cout<<glprogramCreate(&lightingProgram, "lighting", vector<glshader_dl>{ lightingVertexShader, lightingFragmentShader });

	glshaderDestroy(&lightingVertexShader);
	glshaderDestroy(&lightingFragmentShader);
}

void init(void) {
	createProgram();

	glGenFramebuffers(fboLen, glo.fbo);
	glGenRenderbuffers(rboLen, glo.rbo);
	glGenVertexArrays(vaoLen, glo.vao);
	glGenBuffers(boLen, glo.bo);
	glGenTextures(texLen, glo.tex);

	createTexture2D(&glo.tex[texWood], "../../resources/wood.png");

	glBindTexture(GL_TEXTURE_2D, glo.tex[texHDRColorBuffer]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1024, 1024, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindRenderbuffer(GL_RENDERBUFFER, glo.rbo[rboHDRDepthBuffer]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 1024);

	glBindFramebuffer(GL_FRAMEBUFFER, glo.fbo[fboHdr]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glo.tex[texHDRColorBuffer], 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glo.rbo[rboHDRDepthBuffer]);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout<<"FBO: Couldn't create FBO"<<endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float vertices[] = {
		// back face
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		// front face
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		// left face
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		// right face
		1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
		// bottom face
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		// top face
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
		1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
	};
	
	glBindBuffer(GL_ARRAY_BUFFER, glo.bo[vboCube]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(glo.vao[vaoCube]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	cout<<fsquadInit(&fullscreenQuad, "", "shaders/fullscreenquad.frag");

	glEnable(GL_DEPTH_TEST);
}

void render(void) {
	float lights[4 * 4 * 2] = {
		0.0f,  0.0f, 49.5f, 1.0f, 200.0f, 200.0f, 200.0f, 1.0f,
		-1.0f, -2.0f, 13.0f, 1.0f, 0.1f, 0.0f, 0.0f, 1.0f,
		0.0f, -2.0f, 10.0f, 1.0f, 0.0f, 0.0f, 0.1f, 1.0f,
		1.0f, -2.0f, 7.0f, 1.0f, 0.0f, 0.1f, 0.0f, 1.0f
	};

	float currentTime = getTime();
	gltime.delta = currentTime - gltime.last;
	gltime.last = currentTime;

	glBindFramebuffer(GL_FRAMEBUFFER, glo.fbo[fboHdr]);
	glClearBufferfv(GL_COLOR, 0, vec4(0.2f, 0.3f, 0.3f, 1.0f));
	glClearBufferfv(GL_DEPTH, 0, vec1(1.0f));
	glViewport(0, 0, (GLsizei)1024, (GLsizei)1024);
	glUseProgram(lightingProgram.programObject);
	glUniformMatrix4fv(0, 1, GL_FALSE, perspective(45.0f, winSize.w / winSize.h, 0.1f, 1000.0f));
	glUniformMatrix4fv(1, 1, GL_FALSE, lookat(glcamera.position, glcamera.position + glcamera.front, glcamera.up));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texWood]);
	for(int i = 0; i < 4 * 2; i++) {
		glUniform4fv(5 + i, 1, &lights[i * 4]);
	}
	glUniform4fv(4, 1, glcamera.position);
	glUniformMatrix4fv(2, 1, GL_FALSE, translate(0.0f, 0.0f, 25.0f) * scale(2.5f, 2.5f, 27.5f));
	glUniform1i(3, GL_TRUE);
	glBindVertexArray(glo.vao[vaoCube]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearBufferfv(GL_COLOR, 0, vec4(0.2f, 0.3f, 0.3f, 1.0f));
	glClearBufferfv(GL_DEPTH, 0, vec1(1.0f));
	glViewport(0, 0, (GLsizei)winSize.w, (GLsizei)winSize.h);
	fsquadRender(&fullscreenQuad, []()->void { 
		glUniform1i(0, hdr); 
		glUniform1f(1, exposure);
	}, glo.tex[texHDRColorBuffer]);
}

void uninit(void) {
	glprogramDestory(&lightingProgram);
	glprogramDestory(&hdrProgram);
}

void keyboard(unsigned int key, int state) {
	float speed = 7.5f * gltime.delta;
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
	case 'H': case 'h':
		hdr = (hdr == GL_TRUE) ? GL_FALSE : GL_TRUE;
		break;
	case '+': case '=':
		exposure += exposure * 0.1f;
		break;
	case '-': case '_':
		exposure -= exposure * 0.1f;
		if(exposure < 0.01f) {
			exposure = 0.01f;
		}
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
	static float yaw = 90.0f;

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
	toggleFullscreen();
	processEvents();

	init();
	while(!isOpenGLWindowClosed()) {
		processEvents();
		render();
		swapBuffers();
	}
	uninit();
	destroyOpenGLWindow();
}
