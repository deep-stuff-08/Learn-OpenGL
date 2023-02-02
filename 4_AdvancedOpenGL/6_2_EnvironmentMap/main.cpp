#include<iostream>
#include<string>
#include<vector>

#include<GL/glew.h>

#include<glwindowing.h>
#include<vmath.h>
#include<glshaderloader.h>
#include<gltextureloader.h>
#include<glmodelloader.h>
#include<shapes.h>
#include<assimp/postprocess.h>

using namespace std;
using namespace vmath;

#define len(x) sizeof(x) / sizeof(x[0])

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

enum vaolist {
	vaoCube,
	vaoSkybox,
	vaoLen
};

enum bolist {
	vboCube,
	vboSkybox,
	boLen
};

enum texlist {
	texContainer,
	texSkybox,
	texLen
};

enum Model {
	Cube,
	Spring,
	Backpack,
	Vampire
} currentModel;

bool isReflected = true;

struct GLObjects {
	GLuint vao[vaoLen];
	GLuint bo[boLen];
	GLuint tex[texLen];
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
glprogram_dl programSky;
glmodel_dl backpack;
glmodel_dl vampire;
shapes_dl spring;

void createProgram() {
	glshader_dl vertexShader;
	glshader_dl fragmentShader;
	cout<<glshaderCreate(&vertexShader, GL_VERTEX_SHADER, "shaders/render.vert");
	cout<<glshaderCreate(&fragmentShader, GL_FRAGMENT_SHADER, "shaders/render.frag");
	
	cout<<glprogramCreate(&program, "Render", vector<glshader_dl>({vertexShader, fragmentShader}));

	glshaderDestroy(&vertexShader);
	glshaderDestroy(&fragmentShader);

	cout<<glshaderCreate(&vertexShader, GL_VERTEX_SHADER, "shaders/skybox.vert");
	cout<<glshaderCreate(&fragmentShader, GL_FRAGMENT_SHADER, "shaders/skybox.frag");

	cout<<glprogramCreate(&programSky, "Skybox", vector<glshader_dl>({vertexShader, fragmentShader}));

	glshaderDestroy(&vertexShader);
	glshaderDestroy(&fragmentShader);
}

void init(void) {
	createProgram();

	float triangleVertexArray[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	float skyboxVertices[] = {
	// positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(vaoLen, glo.vao);
	glGenBuffers(boLen, glo.bo);
	glGenTextures(texLen, glo.tex);

	glBindVertexArray(glo.vao[vaoCube]);
	glBindBuffer(GL_ARRAY_BUFFER, glo.bo[vboCube]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * len(triangleVertexArray), triangleVertexArray, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) + sizeof(vec3), (void*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) + sizeof(vec3), (void*)(sizeof(vec3)));

	glBindVertexArray(glo.vao[vaoSkybox]);
	glBindBuffer(GL_ARRAY_BUFFER, glo.bo[vboSkybox]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * len(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)(0));
	
	unsigned char* data;
	int w, h;
	glBindTexture(GL_TEXTURE_CUBE_MAP, glo.tex[texSkybox]);
	cout<<loadTextureData("../../resources/skybox/right.jpg", &data, &w, &h);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	cout<<loadTextureData("../../resources/skybox/left.jpg", &data, &w, &h);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	cout<<loadTextureData("../../resources/skybox/bottom.jpg", &data, &w, &h);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	cout<<loadTextureData("../../resources/skybox/top.jpg", &data, &w, &h);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	cout<<loadTextureData("../../resources/skybox/front.jpg", &data, &w, &h);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	cout<<loadTextureData("../../resources/skybox/back.jpg", &data, &w, &h);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	cout<<createModel(&backpack, "../../models/backpack/backpack.obj", aiProcess_GenNormals, 0, 1);
	cout<<createModel(&vampire, "../../models/vampire/dancing_vampire.dae", aiProcess_GenNormals, 0, 1, -1, -1, -1, 2, 3);

	initUnitSpring(&spring, 100, 100, 3, 0.2f, 0, 1);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

void render(void) {
	float currentTime = getTime();
	gltime.delta = currentTime - gltime.last;
	gltime.last = currentTime;

	glClearBufferfv(GL_COLOR, 0, vec4(0.2f, 0.3f, 0.3f, 1.0f));
	glClearBufferfv(GL_DEPTH, 0, vec1(1.0f));
	glViewport(0, 0, (GLsizei)winSize.w, (GLsizei)winSize.h);
	glUseProgram(program.programObject);
	glBindVertexArray(glo.vao[vaoCube]);
	glUniformMatrix4fv(0, 1, GL_FALSE, perspective(30.0f, winSize.w / winSize.h, 0.1f, 100.0f)); 
	glUniformMatrix4fv(1, 1, GL_FALSE, lookat(glcamera.position, glcamera.position + glcamera.front, glcamera.up));
	glUniform3fv(3, 1, glcamera.position);
	glUniform1i(5, isReflected ? GL_TRUE : GL_FALSE);
	static float angle = 0.0f;
	angle += gltime.delta;
		
	if(currentModel == Cube) {
		glUniformMatrix4fv(2, 1, GL_FALSE, rotate(angle * 40.0f, 1.0f, 1.0f, 1.0f));
		glUniform1i(4, GL_FALSE);
		glBindVertexArray(glo.vao[vaoCube]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	} else if(currentModel == Spring) {
		glUniformMatrix4fv(2, 1, GL_FALSE, rotate(angle * 40.0f, 1.0f, 1.0f, 1.0f) * scale(0.5f));
		glUniform1i(4, GL_FALSE);
		renderShape(&spring);
	} else if(currentModel == Backpack) {
		glUniformMatrix4fv(2, 1, GL_FALSE, rotate(angle * 40.0f, 1.0f, 1.0f, 1.0f) * scale(0.5f));
		glUniform1i(4, GL_FALSE);
		drawModel(&backpack);
	} else if(currentModel == Vampire) {
		glUniformMatrix4fv(2, 1, GL_FALSE, translate(0.0f, -0.4f, 0.0f) * scale(0.65f));
		glUniform1i(4, GL_TRUE);
		updateModel(&vampire, gltime.delta, 0);
		setBoneMatrixUniform(&vampire, 0, 6);
		drawModel(&vampire);
	}
	
	glBindVertexArray(glo.vao[vaoCube]);
	glUseProgram(programSky.programObject);
	glUniformMatrix4fv(0, 1, GL_FALSE, perspective(30.0f, winSize.w / winSize.h, 0.1f, 100.0f)); 
	glUniformMatrix4fv(1, 1, GL_FALSE, lookat(vec3(0.0f, 0.0f, 0.0f), glcamera.front, glcamera.up));
	glBindVertexArray(glo.vao[vaoSkybox]);
	glBindTexture(GL_TEXTURE_CUBE_MAP, glo.tex[texSkybox]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
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
	case '1': case '2': case '3': case '4':
		currentModel = (Model)(key - '1');
		break;
	case 'F': case 'f':
		toggleFullscreen();
		break;
	case 'R': case 'r':
		isReflected = !isReflected;
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
