#include<iostream>
#include<string>
#include<vector>

#include<GL/glew.h>

#include<glwindowing.h>
#include<vmath.h>
#include<glshaderloader.h>
#include<fullscreenquad.h>
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
	vaoLen
};

enum bolist {
	vboCube,
	boLen
};

enum fboList {
	fboHdr,
	fboPing,
	fboPong,
	fboLen
};

enum texList {
	texWood,
	texContainer,
	texSceneColorBuffer,
	texBrightColorBuffer,
	texPing,
	texPong,
	texLen
};

enum rboList {
	rboSceneDepthBuffer,
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
	vec3(0.0f, 0.0f, 5.0f),
	vec3(0.0f, 0.0f, -1.0f),
	vec3(0.0f, 1.0f, 0.0f)
};

glprogram_dl renderProgram;
glprogram_dl lightSourceProgram;
glprogram_dl gaussianBlurProgram;

fsquad_dl resolve;
fsquad_dl blur;

GLboolean bloom = GL_FALSE;
GLfloat exposure = 1.0f;

void createProgram() {
	glshader_dl objectVertexShader, objectFragmentShader;
	glshader_dl lightSourceFragmentShader;
	glshader_dl blurComputeShader;

	cout<<glshaderCreate(&objectVertexShader, GL_VERTEX_SHADER, "shaders/render.vert");
	cout<<glshaderCreate(&objectFragmentShader, GL_FRAGMENT_SHADER, "shaders/render.frag");

	cout<<glprogramCreate(&renderProgram, "Render", vector<glshader_dl>{ objectVertexShader, objectFragmentShader });

	glshaderDestroy(&objectVertexShader);
	glshaderDestroy(&objectFragmentShader);
	glshaderDestroy(&lightSourceFragmentShader);
	glshaderDestroy(&blurComputeShader);
}

void init(void) {
	createProgram();
	
	glGenFramebuffers(fboLen, glo.fbo);
	glGenRenderbuffers(rboLen, glo.rbo);
	glGenVertexArrays(vaoLen, glo.vao);
	glGenBuffers(boLen, glo.bo);
	glGenTextures(texLen, glo.tex);

	glBindFramebuffer(GL_FRAMEBUFFER, glo.fbo[fboHdr]);

	glBindTexture(GL_TEXTURE_2D, glo.tex[texSceneColorBuffer]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1024, 1024, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glo.tex[texSceneColorBuffer], 0);

	glBindTexture(GL_TEXTURE_2D, glo.tex[texBrightColorBuffer]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1024, 1024, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, glo.tex[texBrightColorBuffer], 0);
	
	glBindRenderbuffer(GL_RENDERBUFFER, glo.rbo[rboSceneDepthBuffer]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, 1024, 1024);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glo.rbo[rboSceneDepthBuffer]);
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout<<"Framebuffer not complete!"<<endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, glo.fbo[fboPing]);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texPing]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1024, 1024, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glo.tex[texPing], 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout<<"Framebuffer not complete!"<<endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, glo.fbo[fboPong]);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texPong]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1024, 1024, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glo.tex[texPong], 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout<<"Framebuffer not complete!"<<endl;
	}

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
	glBindVertexArray(glo.vao[vaoCube]);
	glBindBuffer(GL_ARRAY_BUFFER, glo.bo[vboCube]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	cout<<createTexture2D(&glo.tex[texWood], "../../resources/wood.png");
	cout<<createTexture2D(&glo.tex[texContainer], "../../resources/container2.png");

	cout<<fsquadInit(&resolve, "", "shaders/resolve.frag");
	cout<<fsquadInit(&blur, "", "shaders/blur.frag");
	
	glEnable(GL_DEPTH_TEST);
}

void render(void) {
	float lightsPos[4 * 4] = {
		0.0f, 0.5f, 1.5f, 1.0f,
		-4.0f, 0.5f, -3.0f, 1.0f,
		3.0f, 0.5f, 1.0f, 1.0f,
		-0.8f, 2.4f, -1.0f, 1.0f
	};
	float lightsColor[4 * 4] = {
		5.0f, 5.0f, 5.0f, 1.0f,
		10.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 15.0f, 1.0f,
		0.0f, 5.0f, 0.0f, 1.0f
	};

	float currentTime = getTime();
	gltime.delta = currentTime - gltime.last;
	gltime.last = currentTime;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, glo.fbo[fboHdr]);
	glViewport(0, 0, 1024, 1024);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat4 projection = perspective(45.0f, winSize.w / winSize.h, 0.1f, 1000.0f);
	mat4 view = lookat(glcamera.position, glcamera.position + glcamera.front, glcamera.up);
	mat4 model = mat4::identity();
	glUseProgram(renderProgram.programObject);
	glUniformMatrix4fv(0, 1, GL_FALSE, projection);
	glUniformMatrix4fv(1, 1, GL_FALSE, view);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texWood]);
	glUniform3fv(3, 1, glcamera.position);
	glUniform4fv(4, 4, lightsPos);
	glUniform4fv(9, 4, lightsColor);
	glUniform1i(13, -1);

	glBindVertexArray(glo.vao[vaoCube]);
	
	glUniformMatrix4fv(2, 1, GL_FALSE, translate(0.0f, -1.0f, 0.0f) * scale(12.5f, 0.5f, 12.5f));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindTexture(GL_TEXTURE_2D, glo.tex[texContainer]);
	
	glUniformMatrix4fv(2, 1, GL_FALSE, translate(0.0f, 1.5f, 0.0f) * scale(0.5f));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glUniformMatrix4fv(2, 1, GL_FALSE, translate(2.0f, 0.0f, 1.0f) * scale(0.5f));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	glUniformMatrix4fv(2, 1, GL_FALSE, translate(-1.0f, -1.0f, 2.0f) * rotate(60.0f, 1.0f, 0.0f, 1.0f));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glUniformMatrix4fv(2, 1, GL_FALSE, translate(0.0f, 2.7f, 4.0f) * rotate(23.0f, 1.0f, 0.0f, 1.0f) * scale(1.25f));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	glUniformMatrix4fv(2, 1, GL_FALSE, translate(-2.0f, 1.0f, -3.0f) * rotate(124.0f, 1.0f, 0.0f, 1.0f));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	glUniformMatrix4fv(2, 1, GL_FALSE, translate(-3.0f, 0.0f, 0.0f) * scale(0.5f));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	for (unsigned int i = 0; i < 4; i++) {
		glUniformMatrix4fv(2, 1, GL_FALSE, translate(lightsPos[i * 4 + 0], lightsPos[i * 4 + 1], lightsPos[i * 4 + 2]) * scale(0.25f));
		glUniform1i(13, i);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, glo.fbo[fboPing]);
	fsquadRender(&blur, [](){
		glUniform1i(0, GL_TRUE);
	}, glo.tex[texBrightColorBuffer]);
	glBindFramebuffer(GL_FRAMEBUFFER, glo.fbo[fboPong]);
	fsquadRender(&blur, [](){
		glUniform1i(0, GL_FALSE);
	}, glo.tex[texPing]);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, winSize.w, winSize.h);
	fsquadRender(&resolve, [](){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, glo.tex[texSceneColorBuffer]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, glo.tex[texPong]);
		glUniform1i(0, bloom ? GL_TRUE : GL_FALSE);
		glUniform1f(1, exposure);
	}, 0);
}

void uninit(void) {
	glprogramDestory(&lightSourceProgram);
	glprogramDestory(&renderProgram);
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
	case 'B': case 'b':
		bloom = (bloom == GL_TRUE) ? GL_FALSE : GL_TRUE;
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
