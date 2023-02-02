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
	vaoTriangle1,
	vaoLen
};

enum bolist {
	boTriangle1,
	boUniformPoint,
	boLen
};

enum texlist {
	texContainerDiffuse,
	texContainerSpecular,
	texLen
};

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
	vec3(0.0f, 0.0f, 3.0f),
	vec3(0.0f, 0.0f, -1.0f),
	vec3(0.0f, 1.0f, 0.0f)
};

struct lightubo{
	struct directionallight {
		vec3 direction;
		char:4;
		vec3 ambient;
		char:4;
		vec3 diffuse;
		char:4;
		vec3 specular;
		char:4;
	} dir;
	struct spotlight {
		vec3 position;
		float innerCutoff;
		vec3 direction;
		float outerCutoff;
		vec3 ambient;
		float constant;
		vec3 diffuse;
		float linear;
		vec3 specular;
		float quadratic;
	} spot;
	struct pointlight {
		vec3 position;
		char:4;
		vec3 ambient;
		float constant;
		vec3 diffuse;
		float linear;
		vec3 specular;
		float quadratic;
	} point[4];
};

glprogram_dl renderProgram;
glprogram_dl lightProgram;

vec3 pointLightPositions[] = {
	vec3( 0.7f,  0.2f,  2.0f),
	vec3( 2.3f, -3.3f, -4.0f),
	vec3(-4.0f,  2.0f, -12.0f),
	vec3( 0.0f,  0.0f, -3.0f)
};

vec3 pointColor[4];
vec4 clearColor;

enum scene {
	Desert,
	Factory,
	Horror,
	Biochemical
} currentScene;

void setupForDesert(lightubo* lights) {
	clearColor = vec4(0.75f, 0.52f, 0.3f, 1.0f);
	pointColor[0] = vec3(1.0f, 0.6f, 0.0f);
	pointColor[1] = vec3(1.0f, 0.0f, 0.0f);
	pointColor[2] = vec3(1.0f, 1.0f, 0.0f);
	pointColor[3] = vec3(0.2f, 0.2f, 1.0f);
	// directional light
	lights->dir.direction = vec3(-0.2f, -1.0f, -0.3f);
	lights->dir.ambient = vec3(0.3f, 0.24f, 0.14f);
	lights->dir.diffuse = vec3(0.7f, 0.42f, 0.26f);
	lights->dir.specular = vec3(0.5f, 0.5f, 0.5f);
	// point light 1
	lights->point[0].position = pointLightPositions[0];
	lights->point[0].ambient = pointColor[0] * 0.1f;
	lights->point[0].diffuse = pointColor[0];
	lights->point[0].specular = pointColor[0];
	lights->point[0].constant = 1.0f;
	lights->point[0].linear = 0.09f;
	lights->point[0].quadratic = 0.032f;
	// point light 2
	lights->point[1].position = pointLightPositions[1];
	lights->point[1].ambient = pointColor[1] * 0.1f;
	lights->point[1].diffuse = pointColor[1];
	lights->point[1].specular = pointColor[1];
	lights->point[1].constant = 1.0f;
	lights->point[1].linear = 0.09f;
	lights->point[1].quadratic = 0.032f;
	// point light 3
	lights->point[2].position = pointLightPositions[2];
	lights->point[2].ambient = pointColor[2] * 0.1f;
	lights->point[2].diffuse = pointColor[2];
	lights->point[2].specular = pointColor[2];
	lights->point[2].constant = 1.0f;
	lights->point[2].linear = 0.09f;
	lights->point[2].quadratic = 0.032f;
	// point light 4
	lights->point[3].position = pointLightPositions[3];
	lights->point[3].ambient = pointColor[3] * 0.1f;
	lights->point[3].diffuse = pointColor[3];
	lights->point[3].specular = pointColor[3];
	lights->point[3].constant = 1.0f;
	lights->point[3].linear = 0.09f;
	lights->point[3].quadratic = 0.032f;
	// spotLight
	lights->spot.position = glcamera.position;
	lights->spot.direction = glcamera.front;
	lights->spot.ambient = vec3(0.0f, 0.0f, 0.0f);
	lights->spot.diffuse = vec3(0.8f, 0.8f, 0.0f);
	lights->spot.specular = vec3(0.8f, 0.8f, 0.0f);
	lights->spot.constant = 1.0f;
	lights->spot.linear = 0.09f;
	lights->spot.quadratic = 0.032f;
	lights->spot.innerCutoff = 12.5f;
	lights->spot.outerCutoff = 13.0f;
}

void setupForFactory(lightubo* lights) {
	clearColor = vec4(0.1f, 0.1f, 0.1f, 1.0f);
	pointColor[0] = vec3(0.2f, 0.2f, 0.6f);
	pointColor[1] = vec3(0.3f, 0.3f, 0.7f);
	pointColor[2] = vec3(0.0f, 0.0f, 0.3f);
	pointColor[3] = vec3(0.4f, 0.4f, 0.4f);
	// directional light
	lights->dir.direction = vec3(-0.2f, -1.0f, -0.3f);		
	lights->dir.ambient = vec3(0.05f, 0.05f, 0.1f);	
	lights->dir.diffuse = vec3(0.2f, 0.2f, 0.7); 
	lights->dir.specular = vec3(0.7f, 0.7f, 0.7f);
	// point light 1
	lights->point[0].position = pointLightPositions[0];
	lights->point[0].ambient = pointColor[0] * 0.1f;
	lights->point[0].diffuse = pointColor[0];
	lights->point[0].specular = pointColor[0];
	lights->point[0].constant = 1.0f;
	lights->point[0].linear = 0.09f;
	lights->point[0].quadratic = 0.032f;
	// point light 2
	lights->point[1].position = pointLightPositions[1];
	lights->point[1].ambient = pointColor[1] * 0.1f;
	lights->point[1].diffuse = pointColor[1];
	lights->point[1].specular = pointColor[1];
	lights->point[1].constant = 1.0f;
	lights->point[1].linear = 0.09f;
	lights->point[1].quadratic = 0.032f;
	// point light 3
	lights->point[2].position = pointLightPositions[2];
	lights->point[2].ambient = pointColor[2] * 0.1f;
	lights->point[2].diffuse = pointColor[2];
	lights->point[2].specular = pointColor[2];
	lights->point[2].constant = 1.0f;
	lights->point[2].linear = 0.09f;
	lights->point[2].quadratic = 0.032f;
	// point light 4
	lights->point[3].position = pointLightPositions[3];
	lights->point[3].ambient = pointColor[3] * 0.1f;
	lights->point[3].diffuse = pointColor[3];
	lights->point[3].specular = pointColor[3];
	lights->point[3].constant = 1.0f;
	lights->point[3].linear = 0.09f;
	lights->point[3].quadratic = 0.032f;
	// spotLight
	lights->spot.position = glcamera.position;
	lights->spot.direction = glcamera.front;
	lights->spot.ambient = vec3(0.0f, 0.0f, 0.0f);
	lights->spot.diffuse = vec3(1.0f, 1.0f, 1.0f);
	lights->spot.specular = vec3(1.0f, 1.0f, 1.0f);
	lights->spot.constant = 1.0f;
	lights->spot.linear = 0.09f;
	lights->spot.quadratic = 0.032f;
	lights->spot.innerCutoff = 10.0f;
	lights->spot.outerCutoff = 12.5f;
}

void setupForHorror(lightubo* lights) {
	clearColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	pointColor[0] = vec3(0.1f, 0.1f, 0.1f);
	pointColor[1] = vec3(0.1f, 0.1f, 0.1f);
	pointColor[2] = vec3(0.1f, 0.1f, 0.1f);
	pointColor[3] = vec3(0.3f, 0.1f, 0.1f);
	// directional light
	lights->dir.direction = vec3(-0.2f, -1.0f, -0.3f);		
	lights->dir.ambient = vec3(0.0f, 0.0f, 0.0f);	
	lights->dir.diffuse = vec3(0.05f, 0.05f, 0.05f); 
	lights->dir.specular = vec3(0.2f, 0.2f, 0.2f);
	// point light 1
	lights->point[0].position = pointLightPositions[0];
	lights->point[0].ambient = pointColor[0] * 0.1f;
	lights->point[0].diffuse = pointColor[0];
	lights->point[0].specular = pointColor[0];
	lights->point[0].constant = 1.0f;
	lights->point[0].linear = 0.09f;
	lights->point[0].quadratic = 0.032f;
	// point light 2
	lights->point[1].position = pointLightPositions[1];
	lights->point[1].ambient = pointColor[1] * 0.1f;
	lights->point[1].diffuse = pointColor[1];
	lights->point[1].specular = pointColor[1];
	lights->point[1].constant = 1.0f;
	lights->point[1].linear = 0.09f;
	lights->point[1].quadratic = 0.032f;
	// point light 3
	lights->point[2].position = pointLightPositions[2];
	lights->point[2].ambient = pointColor[2] * 0.1f;
	lights->point[2].diffuse = pointColor[2];
	lights->point[2].specular = pointColor[2];
	lights->point[2].constant = 1.0f;
	lights->point[2].linear = 0.09f;
	lights->point[2].quadratic = 0.032f;
	// point light 4
	lights->point[3].position = pointLightPositions[3];
	lights->point[3].ambient = pointColor[3] * 0.1f;
	lights->point[3].diffuse = pointColor[3];
	lights->point[3].specular = pointColor[3];
	lights->point[3].constant = 1.0f;
	lights->point[3].linear = 0.09f;
	lights->point[3].quadratic = 0.032f;
	// spotLight
	lights->spot.position = glcamera.position;
	lights->spot.direction = glcamera.front;
	lights->spot.ambient = vec3(0.0f, 0.0f, 0.0f);
	lights->spot.diffuse = vec3(1.0f, 1.0f, 1.0f);
	lights->spot.specular = vec3(1.0f, 1.0f, 1.0f);
	lights->spot.constant = 1.0f;
	lights->spot.linear = 0.09f;
	lights->spot.quadratic = 0.032f;
	lights->spot.innerCutoff = 10.0f;
	lights->spot.outerCutoff = 15.0f;
}

void setupForBiochemical(lightubo* lights) {
	clearColor = vec4(0.9f, 0.9f, 0.9f, 1.0f);
	pointColor[0] = vec3(0.4f, 0.7f, 0.1f);
	pointColor[1] = vec3(0.4f, 0.7f, 0.1f);
	pointColor[2] = vec3(0.4f, 0.7f, 0.1f);
	pointColor[3] = vec3(0.4f, 0.7f, 0.1f);
	// directional light
	lights->dir.direction = vec3(-0.2f, -1.0f, -0.3f);		
	lights->dir.ambient = vec3(0.5f, 0.5f, 0.5f);	
	lights->dir.diffuse = vec3(1.0f, 1.0f, 1.0f); 
	lights->dir.specular = vec3(1.0f, 1.0f, 1.0f);
	// point light 1
	lights->point[0].position = pointLightPositions[0];
	lights->point[0].ambient = pointColor[0] * 0.1f;
	lights->point[0].diffuse = pointColor[0];
	lights->point[0].specular = pointColor[0];
	lights->point[0].constant = 1.0f;
	lights->point[0].linear = 0.07f;
	lights->point[0].quadratic = 0.017f;
	// point light 2
	lights->point[1].position = pointLightPositions[1];
	lights->point[1].ambient = pointColor[1] * 0.1f;
	lights->point[1].diffuse = pointColor[1];
	lights->point[1].specular = pointColor[1];
	lights->point[1].constant = 1.0f;
	lights->point[1].linear = 0.07f;
	lights->point[1].quadratic = 0.017f;
	// point light 3
	lights->point[2].position = pointLightPositions[2];
	lights->point[2].ambient = pointColor[2] * 0.1f;
	lights->point[2].diffuse = pointColor[2];
	lights->point[2].specular = pointColor[2];
	lights->point[2].constant = 1.0f;
	lights->point[2].linear = 0.07f;
	lights->point[2].quadratic = 0.017f;
	// point light 4
	lights->point[3].position = pointLightPositions[3];
	lights->point[3].ambient = pointColor[3] * 0.1f;
	lights->point[3].diffuse = pointColor[3];
	lights->point[3].specular = pointColor[3];
	lights->point[3].constant = 1.0f;
	lights->point[3].linear = 0.07f;
	lights->point[3].quadratic = 0.017f;
	// spotLight
	lights->spot.position = glcamera.position;
	lights->spot.direction = glcamera.front;
	lights->spot.ambient = vec3(0.0f, 0.0f, 0.0f);
	lights->spot.diffuse = vec3(0.0f, 1.0f, 0.0f);
	lights->spot.specular = vec3(0.0f, 1.0f, 0.0f);
	lights->spot.constant = 1.0f;
	lights->spot.linear = 0.07f;
	lights->spot.quadratic = 0.017f;
	lights->spot.innerCutoff = 7.0f;
	lights->spot.outerCutoff = 10.0f;
}

void createProgram() {
	glshader_dl vertexShader;
	glshader_dl fragmentShader;

	cout<<glshaderCreate(&vertexShader, GL_VERTEX_SHADER, "shaders/dirlight.vert");
	cout<<glshaderCreate(&fragmentShader, GL_FRAGMENT_SHADER, "shaders/dirlight.frag");

	cout<<glprogramCreate(&renderProgram, "Render", vector<glshader_dl>({vertexShader, fragmentShader}));

	glshaderDestroy(&vertexShader);
	glshaderDestroy(&fragmentShader);

	cout<<glshaderCreate(&vertexShader, GL_VERTEX_SHADER, "shaders/light.vert");
	cout<<glshaderCreate(&fragmentShader, GL_FRAGMENT_SHADER, "shaders/light.frag");

	cout<<glprogramCreate(&lightProgram, "Light", vector<glshader_dl>({vertexShader, fragmentShader}));

	glshaderDestroy(&vertexShader);
	glshaderDestroy(&fragmentShader);
}

void init(void) {
	createProgram();
	
	float triangleVertexArray[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	glGenVertexArrays(vaoLen, glo.vao);
	glGenBuffers(boLen, glo.bo);
	glGenTextures(texLen, glo.tex);

	glBindVertexArray(glo.vao[vaoTriangle1]);
	glBindBuffer(GL_ARRAY_BUFFER, glo.bo[boTriangle1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * len(triangleVertexArray), triangleVertexArray, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) + sizeof(vec3) + sizeof(vec2), (void*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) + sizeof(vec3) + sizeof(vec2), (void*)(sizeof(vec3)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vec3) + sizeof(vec3) + sizeof(vec2), (void*)(sizeof(vec3) * 2));

	cout<<createTexture2D(&glo.tex[texContainerDiffuse], "../../resources/container2.png");
	cout<<createTexture2D(&glo.tex[texContainerSpecular], "../../resources/container2_specular.png");

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, glo.bo[boUniformPoint]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(lightubo), NULL, GL_DYNAMIC_DRAW);
	lightubo* lights = (lightubo*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(lightubo), GL_MAP_WRITE_BIT);
	setupForDesert(lights);
	glUnmapBuffer(GL_UNIFORM_BUFFER);
		
	glEnable(GL_DEPTH_TEST);
}

void render() {
	float currentTime = getTime();
	gltime.delta = currentTime - gltime.last;
	gltime.last = currentTime;

	lightubo* lights;
	
	vec3 cubePositions[] = {
		vec3( 0.0f,  0.0f,  0.0f), 
		vec3( 2.0f,  5.0f, -15.0f), 
		vec3(-1.5f, -2.2f, -2.5f),  
		vec3(-3.8f, -2.0f, -12.3f),  
		vec3( 2.4f, -0.4f, -3.5f),  
		vec3(-1.7f,  3.0f, -7.5f),  
		vec3( 1.3f, -2.0f, -2.5f),  
		vec3( 1.5f,  2.0f, -2.5f), 
		vec3( 1.5f,  0.2f, -1.5f), 
		vec3(-1.3f,  1.0f, -1.5f)
	};

	glClearBufferfv(GL_COLOR, 0, clearColor);
	glClearBufferfv(GL_DEPTH, 0, vec1(1.0f));
	glViewport(0, 0, (GLsizei)winSize.w, (GLsizei)winSize.h);
	glUseProgram(renderProgram.programObject);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texContainerDiffuse]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, glo.tex[texContainerSpecular]);
	glBindVertexArray(glo.vao[vaoTriangle1]);

	lights = (lightubo*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(lightubo), GL_MAP_WRITE_BIT);
	switch(currentScene) {
	case Desert:
		setupForDesert(lights);
		break;
	case Factory:
		setupForFactory(lights);
		break;
	case Horror:
		setupForHorror(lights);
		break;
	case Biochemical:
		setupForBiochemical(lights);
		break;
	}
	glUnmapBuffer(GL_UNIFORM_BUFFER);

	glUniformMatrix4fv(0, 1, GL_FALSE, perspective(45.0f, winSize.w / winSize.h, 0.1f, 100.0f)); 
	glUniformMatrix4fv(1, 1, GL_FALSE, lookat(glcamera.position, glcamera.position + glcamera.front, glcamera.up));
	glUniform3fv(3, 1, glcamera.position);
	glUniform1f(4, 32.0f);
	for(int i = 0; i < len(cubePositions); i++) {
		glUniformMatrix4fv(2, 1, GL_FALSE, translate(cubePositions[i]) * rotate(i * 20.0f, normalize(vec3(1.0f, 0.3f, 0.5f))));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	glUseProgram(lightProgram.programObject);
	glUniformMatrix4fv(0, 1, GL_FALSE, perspective(45.0f, winSize.w / winSize.h, 0.1f, 100.0f)); 
	glUniformMatrix4fv(1, 1, GL_FALSE, lookat(glcamera.position, glcamera.position + glcamera.front, glcamera.up));
	for(int i = 0; i < 4; i++) {
		glUniformMatrix4fv(2, 1, GL_FALSE, translate(pointLightPositions[i]) * scale(0.2f));
		glUniform3fv(3, 1, pointColor[i]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void uninit(void) {

}

void keyboard(unsigned int key, int state) {
	float speed = 2.5f * gltime.delta;
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
	case '1': case '2': case '3': case '4':
		currentScene = (scene)(key - '1');
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
