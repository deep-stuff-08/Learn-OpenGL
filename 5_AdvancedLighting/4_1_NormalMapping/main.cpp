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
	boLen
};

enum texlist {
	texBrickwallDiffuse,
	texBrickwallNormal,
	texLen
};

struct GLTimer {
	double delta;
	double last;	
} gltime = {0.0f, 0.0f};

struct GLCamera {
	vec3 position;
	vec3 front;
	vec3 up;
} glcamera = {
	vec3(0.0f, 0.0f, 3.0f),
	vec3(0.0f, 0.0f, -1.0f),
	vec3(0.0f, 1.0f, 0.0f)
};

vec3 lightPos = vec3(0.5f, 1.0f, 0.3f);

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

	vec3 pos1(-1.0f,  1.0f, 0.0f);
	vec3 pos2(-1.0f, -1.0f, 0.0f);
	vec3 pos3( 1.0f, -1.0f, 0.0f);
	vec3 pos4( 1.0f,  1.0f, 0.0f);
	vec2 uv1(0.0f, 1.0f);
	vec2 uv2(0.0f, 0.0f);
	vec2 uv3(1.0f, 0.0f);
	vec2 uv4(1.0f, 1.0f);
	vec3 nm(0.0f, 0.0f, 1.0f);
	vec3 tangent1, bitangent1;
	vec3 tangent2, bitangent2;
	vec3 edge1 = pos2 - pos1;
	vec3 edge2 = pos3 - pos1;
	vec2 deltaUV1 = uv2 - uv1;
	vec2 deltaUV2 = uv3 - uv1;

	float f = 1.0f / (deltaUV1[0] * deltaUV2[1] - deltaUV2[0] * deltaUV1[1]);

	tangent1[0] = f * (deltaUV2[1] * edge1[0] - deltaUV1[1] * edge2[0]);
	tangent1[1] = f * (deltaUV2[1] * edge1[1] - deltaUV1[1] * edge2[1]);
	tangent1[2] = f * (deltaUV2[1] * edge1[2] - deltaUV1[1] * edge2[2]);
	tangent1 = normalize(tangent1);

	bitangent1[0] = f * (-deltaUV2[0] * edge1[0] + deltaUV1[0] * edge2[0]);
	bitangent1[1] = f * (-deltaUV2[0] * edge1[1] + deltaUV1[0] * edge2[1]);
	bitangent1[2] = f * (-deltaUV2[0] * edge1[2] + deltaUV1[0] * edge2[2]);
	bitangent1 = normalize(bitangent1);

	edge1 = pos3 - pos1;
	edge2 = pos4 - pos1;
	deltaUV1 = uv3 - uv1;
	deltaUV2 = uv4 - uv1;

	f = 1.0f / (deltaUV1[0] * deltaUV2[1] - deltaUV2[0] * deltaUV1[1]);

	tangent2[0] = f * (deltaUV2[1] * edge1[0] - deltaUV1[1] * edge2[0]);
	tangent2[1] = f * (deltaUV2[1] * edge1[1] - deltaUV1[1] * edge2[1]);
	tangent2[2] = f * (deltaUV2[1] * edge1[2] - deltaUV1[1] * edge2[2]);
	tangent2 = normalize(tangent2);

	bitangent2[0] = f * (-deltaUV2[0] * edge1[0] + deltaUV1[0] * edge2[0]);
	bitangent2[1] = f * (-deltaUV2[0] * edge1[1] + deltaUV1[0] * edge2[1]);
	bitangent2[2] = f * (-deltaUV2[0] * edge1[2] + deltaUV1[0] * edge2[2]);
	bitangent2 = normalize(bitangent2);

	float quadVertices[] = {
		pos1[0], pos1[1], pos1[2], nm[0], nm[1], nm[2], uv1[0], uv1[1], tangent1[0], tangent1[1], tangent1[2], bitangent1[0], bitangent1[1], bitangent1[2],
		pos2[0], pos2[1], pos2[2], nm[0], nm[1], nm[2], uv2[0], uv2[1], tangent1[0], tangent1[1], tangent1[2], bitangent1[0], bitangent1[1], bitangent1[2],
		pos3[0], pos3[1], pos3[2], nm[0], nm[1], nm[2], uv3[0], uv3[1], tangent1[0], tangent1[1], tangent1[2], bitangent1[0], bitangent1[1], bitangent1[2],

		pos1[0], pos1[1], pos1[2], nm[0], nm[1], nm[2], uv1[0], uv1[1], tangent2[0], tangent2[1], tangent2[2], bitangent2[0], bitangent2[1], bitangent2[2],
		pos3[0], pos3[1], pos3[2], nm[0], nm[1], nm[2], uv3[0], uv3[1], tangent2[0], tangent2[1], tangent2[2], bitangent2[0], bitangent2[1], bitangent2[2],
		pos4[0], pos4[1], pos4[2], nm[0], nm[1], nm[2], uv4[0], uv4[1], tangent2[0], tangent2[1], tangent2[2], bitangent2[0], bitangent2[1], bitangent2[2]
	};
	
	glGenVertexArrays(vaoLen, glo.vao);
	glGenBuffers(boLen, glo.bo);

	glBindVertexArray(glo.vao[vaoTriangle]);
	glBindBuffer(GL_ARRAY_BUFFER, glo.bo[vboTriangle]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * len(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (void*)(sizeof(float) * 8));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (void*)(sizeof(float) * 11));
	
	createTexture2D(&glo.tex[texBrickwallDiffuse], "../../resources/brickwall.jpg");
	createTexture2D(&glo.tex[texBrickwallNormal], "../../resources/brickwall_normal.jpg");
}

void render(void) {
	float currentTime = getTime();
	gltime.delta = currentTime - gltime.last;
	gltime.last = currentTime;

	glClearBufferfv(GL_COLOR, 0, vec4(0.1f, 0.1f, 0.1f, 1.0f));
	glViewport(0, 0, (GLsizei)winSize.w, (GLsizei)winSize.h);
	glUseProgram(program.programObject);
	glUniformMatrix4fv(0, 1, GL_FALSE, perspective(45.0f, winSize.w / winSize.h, 0.1f, 100.0f));
	glUniformMatrix4fv(1, 1, GL_FALSE, lookat(glcamera.position, glcamera.position + glcamera.front, glcamera.up));
	glUniformMatrix4fv(2, 1, GL_FALSE, rotate((float)getTime() * 10.0f, 1.0f, 0.0f, 1.0f));
	glUniform3fv(3, 1, glcamera.position);
	glUniform3fv(4, 1, lightPos);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texBrickwallDiffuse]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texBrickwallNormal]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUniformMatrix4fv(2, 1, GL_FALSE, translate(lightPos) * scale(0.1f));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
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
