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

#define SHADOW_WIDTH_HEIGHT 1024

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
	fboDepth,
	fboLen
};

enum texList {
	texWood,
	texDepth,
	texLen
};

struct GLTimer {
	double delta;
	double last;	
} gltime = {0.0f, 0.0f};

struct GLObjects {
	GLuint vao[vaoLen];
	GLuint bo[boLen];
	GLuint fbo[fboLen];
	GLuint tex[texLen];
} glo;

struct GLCamera {
	vec3 position;
	vec3 front;
	vec3 up;
} glcamera = {
	vec3(0.0f, 0.0f, 3.0f),
	vec3(0.0f, 0.0f, -1.0f),
	vec3(0.0f, 1.0f, 0.0f)
};

vec3 lightPos = vec3(0.0f, 0.0f, 3.0f);

glprogram_dl simpleDepthProgram;
glprogram_dl shadowMapProgram;
fsquad_dl fsquad;

void createProgram() {
	glshader_dl renderVertex, renderGeometry, renderFragment;

	cout<<glshaderCreate(&renderVertex, GL_VERTEX_SHADER, "shaders/pointShadowsDepth.vert");
	cout<<glshaderCreate(&renderGeometry, GL_GEOMETRY_SHADER, "shaders/pointShadowsDepth.geom");
	cout<<glshaderCreate(&renderFragment, GL_FRAGMENT_SHADER, "shaders/pointShadowsDepth.frag");

	cout<<glprogramCreate(&simpleDepthProgram, "Simple Depth", { renderVertex, renderGeometry, renderFragment });

	glshaderDestroy(&renderVertex);
	glshaderDestroy(&renderGeometry);
	glshaderDestroy(&renderFragment);

	cout<<glshaderCreate(&renderVertex, GL_VERTEX_SHADER, "shaders/pointShadows.vert");
	cout<<glshaderCreate(&renderFragment, GL_FRAGMENT_SHADER, "shaders/pointShadows.frag");

	cout<<glprogramCreate(&shadowMapProgram, "Shadow Map", { renderVertex, renderFragment });

	glshaderDestroy(&renderVertex);
	glshaderDestroy(&renderFragment);
}

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

void init(void) {
	glEnable              ( GL_DEBUG_OUTPUT );
	glDebugMessageCallback( MessageCallback, 0 );
	
	createProgram();

	glGenVertexArrays(vaoLen, glo.vao);
	glGenBuffers(boLen, glo.bo);
	glGenTextures(texLen, glo.tex);
	glGenFramebuffers(fboLen, glo.fbo);

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
	// link vertex attributes
	glBindVertexArray(glo.vao[vaoCube]);
	glBindBuffer(GL_ARRAY_BUFFER, glo.bo[vboCube]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	
	createTexture2D(&glo.tex[texWood], "../../resources/wood.png");

	glBindTexture(GL_TEXTURE_CUBE_MAP, glo.tex[texDepth]);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH_HEIGHT, SHADOW_WIDTH_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH_HEIGHT, SHADOW_WIDTH_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH_HEIGHT, SHADOW_WIDTH_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH_HEIGHT, SHADOW_WIDTH_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH_HEIGHT, SHADOW_WIDTH_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH_HEIGHT, SHADOW_WIDTH_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	glBindFramebuffer(GL_FRAMEBUFFER, glo.fbo[fboDepth]);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, glo.tex[texDepth], 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glClearDepth(1.0f);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void renderScene(bool forDepth) {
	int modelLoc = forDepth ? 0 : 2;
	// glBindVertexArray(glo.vao[vaoCube]);
	glDisable(GL_CULL_FACE);
	if(!forDepth) {
		glUniform1i(3, -1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, scale(5.0f));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glUniform1i(3, 1);
	} else {
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, scale(5.0f));
		glDrawArrays(GL_TRIANGLES, 0, 36);	
	}
	glEnable(GL_CULL_FACE);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, translate(2.0f, 3.0f, 1.0f) * scale(0.75f));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, translate(-3.0f, -1.0f, 0.0f) * scale(0.5f));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, translate(-1.5f, 1.0f, 1.5f) * scale(0.5f));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, translate(-1.5f, 2.0f, -3.0f) * rotate(60.0f, normalize(vec3(1.0f, 0.0f, 1.0f))) * scale(0.75f));
	glDrawArrays(GL_TRIANGLES, 0, 36);

}

void render(void) {
	float currentTime = getTime();
	gltime.delta = currentTime - gltime.last;
	gltime.last = currentTime;

	lightPos[2] = sin(getTime() * 0.5f) * 3.0f;

	glUseProgram(simpleDepthProgram.programObject);
	glBindFramebuffer(GL_FRAMEBUFFER, glo.fbo[fboDepth]);
	// glClearBufferfv(GL_DEPTH, 0, vec1(1.0f));
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOW_WIDTH_HEIGHT, SHADOW_WIDTH_HEIGHT);
	mat4 proj = perspective(90.0f, 1.0f, 1.0f, 25.0f);
	mat4 shadowView[6] = {
		proj * lookat(lightPos, lightPos + vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),
		proj * lookat(lightPos, lightPos + vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),
		proj * lookat(lightPos, lightPos + vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)),
		proj * lookat(lightPos, lightPos + vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)),
		proj * lookat(lightPos, lightPos + vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)),
		proj * lookat(lightPos, lightPos + vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f))
	};
	for(int i = 0; i < 6; i++) {
		glUniformMatrix4fv(1 + i, 1, GL_FALSE, shadowView[i]);
	}
	glUniform3fv(7, 1, lightPos);
	glUniform1f(8, 25.0f);
	renderScene(true);

	glUseProgram(shadowMapProgram.programObject);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// glClearBufferfv(GL_COLOR, 0, vec4(0.1f, 0.1f, 0.1f, 1.0f));
	// glClearBufferfv(GL_DEPTH, 0, vec1(1.0f));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, winSize.w, winSize.h);
	glUniformMatrix4fv(0, 1, GL_FALSE, perspective(45.0f, winSize.w / winSize.h, 0.1f, 100.0f));
	glUniformMatrix4fv(1, 1, GL_FALSE, lookat(glcamera.position, glcamera.front + glcamera.position, glcamera.up));
	glUniform3fv(4, 1, lightPos);
	glUniform3fv(5, 1, glcamera.position);
	glUniform1f(6, 25.0f);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texWood]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, glo.tex[texDepth]);
	renderScene(false);

	// fsquadRender(&fsquad, NULL, glo.tex[texDepth]);
}

void uninit(void) {
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
	setGLContextVersion(4, 6);
	setGLContextProfile(DL_CORE_CONTEXT);
	setGLContextFlags(DL_FORWARD_CONTEX_BIT);
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
