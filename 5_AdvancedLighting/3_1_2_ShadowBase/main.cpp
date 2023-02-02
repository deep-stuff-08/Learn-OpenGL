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
	vaoPlane,
	vaoCube,
	vaoLen
};

enum bolist {
	vboPlane,
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

vec3 lightPos = vec3(-2.0f, 4.0f, -1.0f);

glprogram_dl simpleDepthProgram;
glprogram_dl shadowMapProgram;
glprogram_dl shadowMapNewProgram;
fsquad_dl fsquad;

bool useNew = false;

void createProgram() {
	glshader_dl renderVertex, renderFragment;

	cout<<glshaderCreate(&renderVertex, GL_VERTEX_SHADER, "shaders/simpleDepth.vert");
	cout<<glshaderCreate(&renderFragment, GL_FRAGMENT_SHADER, "shaders/simpleDepth.frag");

	cout<<glprogramCreate(&simpleDepthProgram, "Simple Depth", { renderVertex, renderFragment });

	glshaderDestroy(&renderVertex);
	glshaderDestroy(&renderFragment);

	cout<<glshaderCreate(&renderVertex, GL_VERTEX_SHADER, "shaders/shadowMap.vert");
	cout<<glshaderCreate(&renderFragment, GL_FRAGMENT_SHADER, "shaders/shadowMap.frag");

	cout<<glprogramCreate(&shadowMapProgram, "Shadow Map", { renderVertex, renderFragment });

	glshaderDestroy(&renderFragment);

	cout<<glshaderCreate(&renderFragment, GL_FRAGMENT_SHADER, "shaders/shadowMapNew.frag");

	cout<<glprogramCreate(&shadowMapNewProgram, "Shadow Map New", { renderVertex, renderFragment });

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

	float planeVertices[] = {
        // positions            // normals         // texcoords
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };

	glBindVertexArray(glo.vao[vaoPlane]);
	glBindBuffer(GL_ARRAY_BUFFER, glo.bo[vboPlane]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	
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

	glBindTexture(GL_TEXTURE_2D, glo.tex[texDepth]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH_HEIGHT, SHADOW_WIDTH_HEIGHT);
	
	glBindFramebuffer(GL_FRAMEBUFFER, glo.fbo[fboDepth]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, glo.tex[texDepth], 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	fsquadInit(&fsquad, "", "shaders/fullscreenquad.frag");

	glEnable(GL_DEPTH_TEST);
}

void renderScene() {
	glUniformMatrix4fv(2, 1, GL_FALSE, mat4::identity());
	glBindVertexArray(glo.vao[vaoPlane]);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(glo.vao[vaoCube]);
	glUniformMatrix4fv(2, 1, GL_FALSE, translate(0.0f, 1.5f, 0.0f) * scale(0.5f));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniformMatrix4fv(2, 1, GL_FALSE, translate(2.0f, 0.0f, 1.0f) * scale(0.5f));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniformMatrix4fv(2, 1, GL_FALSE, translate(-1.0f, 0.0f, 2.0f) * rotate(60.0f, normalize(vec3(1.0, 0.0, 1.0))) * scale(0.25f));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void render(void) {
	float currentTime = getTime();
	gltime.delta = currentTime - gltime.last;
	gltime.last = currentTime;

	mat4 scaleBiasMatrix = mat4(
		vec4(0.5f, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, 0.5f, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 0.5f, 0.0f),
		vec4(0.5f, 0.5f, 0.5f, 1.0f)
	);

	mat4 lightSpaceMatrix = ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f) * lookat(lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));

	glUseProgram(simpleDepthProgram.programObject);
	glBindFramebuffer(GL_FRAMEBUFFER, glo.fbo[fboDepth]);
	glClearBufferfv(GL_DEPTH, 0, vec1(1.0f));
	glViewport(0, 0, SHADOW_WIDTH_HEIGHT, SHADOW_WIDTH_HEIGHT);
	glUseProgram(simpleDepthProgram.programObject);
	glUniformMatrix4fv(0, 1, GL_FALSE, lightSpaceMatrix);
	renderScene();

	if(useNew) {
		glUseProgram(shadowMapNewProgram.programObject);
	} else {
		glUseProgram(shadowMapProgram.programObject);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearBufferfv(GL_COLOR, 0, vec4(0.1f, 0.1f, 0.1f, 1.0f));
	glClearBufferfv(GL_DEPTH, 0, vec1(1.0f));
	glViewport(0, 0, winSize.w, winSize.h);
	glUniformMatrix4fv(0, 1, GL_FALSE, perspective(45.0f, winSize.w / winSize.h, 0.1f, 100.0f));
	glUniformMatrix4fv(1, 1, GL_FALSE, lookat(glcamera.position, glcamera.front + glcamera.position, glcamera.up));
	if(useNew) {
		glUniformMatrix4fv(3, 1, GL_FALSE, scaleBiasMatrix * lightSpaceMatrix);
	} else {
		glUniformMatrix4fv(3, 1, GL_FALSE, lightSpaceMatrix);
	}
	glUniform3fv(4, 1, lightPos);
	glUniform3fv(5, 1, glcamera.position);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texWood]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texDepth]);
	renderScene();

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
	case 'N': case 'n':
		useNew = !useNew;
		cout<<"Switched to "<<(useNew ? "New" : "Old")<<" version"<<endl;
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
