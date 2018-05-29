// Library for OpenGL function loading
// Must be included before GLFW
#define GLEW_STATIC
#include <GL/glew.h>

// Library for window creation and event handling
#include <GLFW/glfw3.h>

// Library for vertex and matrix math
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Library for loading .OBJ model
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

// Library for loading an image
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Header for camera structure/functions
#include "camera.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "visual studio/particle_generator.h"

// Configuration
const int WIDTH = 1200;
const int HEIGHT = 900;

// Per-vertex data
struct Vertex {
	glm::vec3 color;
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 UV;
	int ID;
	int bulnum;
	glm::vec2 traj;
	glm::vec2 offset;
	glm::vec4 ori0, ori1, ori2, ori3;
};

//Variables
double xpos, ypos;
glm::mat4 view;
glm::mat4 projection;
glm::mat4 model = glm::mat4(1.0f);
std::vector<Vertex> vertices;
std::vector<Vertex> landscape;
std::vector<Vertex> ship;
std::vector<Vertex> enemyShip;
std::vector<Vertex> bullets;
float nextbul = 0;
bool is8 = false;
float bullettime = 0;
glm::vec4 bulletmult;
glm::vec4 bulletmult2;
double prevtime = 0;
double particleTime = 0;
ParticleGenerator   *Particles;


//shit variables
glm::vec2 shiplocation;
float speed = 0.01f;
int hp = 5;
bool wpress;
bool apress;
bool spress;
bool dpress;

//decides which bullettexture to use
bool bultexture = true;

int NbVertY = 200, NbVertX = 200;
std::vector<float> SurfaceVertices3f;
std::vector<float> SurfaceNormals3f;
std::vector<float> SurfaceColors3f;
std::vector<float> SurfaceTexCoords2f;
std::vector<unsigned int> SurfaceTriangles3ui;

std::vector<float> ShipVertices3f;
std::vector<float> ShipNormals3f;
std::vector<float> ShipColors3f;
std::vector<float> ShipTexCoords2f;
std::vector<unsigned int> ShipTriangles3ui;

//enemy data
std::vector<float> eShipVertices3f;
std::vector<float> eShipNormals3f;
std::vector<float> eShipColors3f;
std::vector<float> eShipTexCoords2f;
std::vector<unsigned int> eShipTriangles3ui;
std::vector<glm::vec4> enemydata;
glm::vec4 enemydata0 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
glm::vec4 enemydata1 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
glm::vec4 enemydata2 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
glm::vec4 enemydata3 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
glm::vec4 enemydata4 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
float spawnrate = 0.004f;
float enemyspeed = 0.01f;
float maxhp = 3;


void addTriangle(std::vector<Vertex> obs, Vertex v1, Vertex v2, Vertex v3) {
	obs.push_back(v1);
	obs.push_back(v2);
	obs.push_back(v3);
}

// Helper function to read a file like a shader
std::string readFile(const std::string& path) {
	std::ifstream file(path, std::ios::binary);

	std::stringstream buffer;
	buffer << file.rdbuf();

	return buffer.str();
}

bool checkShaderErrors(GLuint shader) {
	// Check if the shader compiled successfully
	GLint compileSuccessful;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccessful);

	// If it didn't, then read and print the compile log
	if (!compileSuccessful) {
		GLint logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

		std::vector<GLchar> logBuffer(logLength);
		glGetShaderInfoLog(shader, logLength, nullptr, logBuffer.data());

		std::cerr << logBuffer.data() << std::endl;

		return false;
	}
	else {
		return true;
	}
}

bool checkProgramErrors(GLuint program) {
	// Check if the program linked successfully
	GLint linkSuccessful;
	glGetProgramiv(program, GL_LINK_STATUS, &linkSuccessful);

	// If it didn't, then read and print the link log
	if (!linkSuccessful) {
		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

		std::vector<GLchar> logBuffer(logLength);
		glGetProgramInfoLog(program, logLength, nullptr, logBuffer.data());

		std::cerr << logBuffer.data() << std::endl;

		return false;
	}
	else {
		return true;
	}
}

// OpenGL debug callback
void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
		std::cerr << "OpenGL: " << message << std::endl;
	}
}


// Key handle function
void keyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	cameraKeyboardHandler(key, action);
	if (action == GLFW_RELEASE) {
		switch (key)
		{
		case GLFW_KEY_A:
			apress = false;
			break;
		case GLFW_KEY_D:
			dpress = false;
			break;
		case GLFW_KEY_W:
			wpress = false;
			break;
		case GLFW_KEY_S:
			spress = false;
			break;
		default:
			break;
		}
	}
	if (action == GLFW_REPEAT) {
		switch (key)
		{
		case GLFW_KEY_A:
			apress = true;
			break;
		case GLFW_KEY_D:
			dpress = true;
			break;
		case GLFW_KEY_W:
			wpress = true;
			break;
		case GLFW_KEY_S:
			spress = true;
			break;
		default:
			break;
		}
	}
	if (action == GLFW_PRESS) {
		switch (key)
		{
		case GLFW_KEY_A:
			apress = true;

			break;
		case GLFW_KEY_D:
			dpress = true;

			break;
		case GLFW_KEY_W:
			wpress = true;
			break;
		case GLFW_KEY_S:
			spress = true;
			break;
		default:
			break;
		}
	}
}

// Mouse button handle function
void mouseButtonHandler(GLFWwindow* window, int button, int action, int mods)
{
	camMouseButtonHandler(button, action);
}

void cursorPosHandler(GLFWwindow* window, double xpos, double ypos)
{
	camCursorPosHandler(xpos, ypos);
}

void initWeaponMesh()
{
	glm::vec3 v1 = glm::vec3(-0.5f, -0.5f, -0.5f);
	glm::vec3 v2 = glm::vec3(-0.5f, -0.5f, 0.5f);
	glm::vec3 v3 = glm::vec3(-0.5f, 0.5f, -0.5f);
	glm::vec3 v4 = glm::vec3(-0.5f, 0.5f, 0.5f);
	glm::vec3 v5 = glm::vec3(0.5f, -0.5f, -0.5f);
	glm::vec3 v6 = glm::vec3(0.5f, -0.5f, 0.5f);
	glm::vec3 v7 = glm::vec3(0.5f, 0.5f, -0.5f);
	glm::vec3 v8 = glm::vec3(0.5f, 0.5f, 0.5f);

	
	glm::vec3 c1 = glm::vec3(0.3f, 0.3f, 1.0f);
	glm::vec3 n1 = normalize(glm::vec3(-1, -1, -1));
	glm::vec3 n2 = normalize(glm::vec3(-1, -1, 1));
	glm::vec3 n3 = normalize(glm::vec3(-1, 1, -1));
	glm::vec3 n4 = normalize(glm::vec3(-1, 1, 1));
	glm::vec3 n5 = normalize(glm::vec3(1, -1, -1));
	glm::vec3 n6 = normalize(glm::vec3(1, -1, 1));
	glm::vec3 n7 = normalize(glm::vec3(1, 1, -1));
	glm::vec3 n8 = normalize(glm::vec3(1, 1, 1));

	glm::vec2 t1 = glm::vec2(0.0f, 0.0f);
	glm::vec2 t2 = glm::vec2(0.0f, 0.2f);
	glm::vec2 t3 = glm::vec2(0.2f, 0.0f);
	glm::vec2 t4 = glm::vec2(0.2f, 0.2f);
	
	
	Vertex ver1 = Vertex{ c1, v1, n1, t1, 2};
	Vertex ver2 = Vertex{ c1, v2, n2, t2, 2};
	Vertex ver3 = Vertex{ c1, v3, n3, t3, 2};
	Vertex ver4 = Vertex{ c1, v4, n4, t4, 2};
	Vertex ver5 = Vertex{ c1, v5, n5, t2, 2};
	Vertex ver6 = Vertex{ c1, v6, n6, t1, 2};
	Vertex ver7 = Vertex{ c1, v7, n7, t4, 2};
	Vertex ver8 = Vertex{ c1, v8, n8, t3, 2};

	vertices.push_back(ver1);
	vertices.push_back(ver2);
	vertices.push_back(ver5);

	vertices.push_back(ver4);
	vertices.push_back(ver8);
	vertices.push_back(ver7);

	vertices.push_back(ver5);
	vertices.push_back(ver2);
	vertices.push_back(ver6);

	vertices.push_back(ver3);
	vertices.push_back(ver4);
	vertices.push_back(ver7);

	vertices.push_back(ver1);
	vertices.push_back(ver3);
	vertices.push_back(ver2);

	vertices.push_back(ver7);
	vertices.push_back(ver8);
	vertices.push_back(ver6);

	vertices.push_back(ver2);
	vertices.push_back(ver3);
	vertices.push_back(ver4);

	vertices.push_back(ver5);
	vertices.push_back(ver7);
	vertices.push_back(ver6);

	vertices.push_back(ver1);
	vertices.push_back(ver3);
	vertices.push_back(ver5);

	vertices.push_back(ver4);
	vertices.push_back(ver8);
	vertices.push_back(ver6);

	vertices.push_back(ver3);
	vertices.push_back(ver5);
	vertices.push_back(ver7);

	vertices.push_back(ver2);
	vertices.push_back(ver6);
	vertices.push_back(ver4);
}

void initBulletMesh(int bulnum, glm::vec2 t, glm::mat4 rot)
{
	glm::vec3 v1 = glm::vec3(-0.5f, -0.5f, -0.5f);
	glm::vec3 v2 = glm::vec3(-0.5f, -0.5f, 0.5f);
	glm::vec3 v3 = glm::vec3(-0.5f, 0.5f, -0.5f);
	glm::vec3 v4 = glm::vec3(-0.5f, 0.5f, 0.5f);
	glm::vec3 v5 = glm::vec3(0.5f, -0.5f, -0.5f);
	glm::vec3 v6 = glm::vec3(0.5f, -0.5f, 0.5f);
	glm::vec3 v7 = glm::vec3(0.5f, 0.5f, -0.5f);
	glm::vec3 v8 = glm::vec3(0.5f, 0.5f, 0.5f);


	glm::vec3 c1 = glm::vec3(1.0f, 0.8f, 0.8f);
	glm::vec3 n1 = normalize(glm::vec3(-1, -1, -1));
	glm::vec3 n2 = normalize(glm::vec3(-1, -1, 1));
	glm::vec3 n3 = normalize(glm::vec3(-1, 1, -1));
	glm::vec3 n4 = normalize(glm::vec3(-1, 1, 1));
	glm::vec3 n5 = normalize(glm::vec3(1, -1, -1));
	glm::vec3 n6 = normalize(glm::vec3(1, -1, 1));
	glm::vec3 n7 = normalize(glm::vec3(1, 1, -1));
	glm::vec3 n8 = normalize(glm::vec3(1, 1, 1));

	glm::vec2 t1 = glm::vec2(0.0f, 0.0f);
	glm::vec2 t2 = glm::vec2(0.0f, 1.0f);
	glm::vec2 t3 = glm::vec2(1.0f, 0.0f);
	glm::vec2 t4 = glm::vec2(1.0f, 1.0f);


	Vertex ver1 = Vertex{ c1, v1, n1, t1, 3, bulnum, glm::normalize(t + shiplocation), shiplocation, rot[0], rot[1], rot[2], rot[3] };
	Vertex ver2 = Vertex{ c1, v2, n2, t2, 3, bulnum, glm::normalize(t + shiplocation), shiplocation, rot[0], rot[1], rot[2], rot[3] };
	Vertex ver3 = Vertex{ c1, v3, n3, t3, 3, bulnum, glm::normalize(t + shiplocation), shiplocation, rot[0], rot[1], rot[2], rot[3] };
	Vertex ver4 = Vertex{ c1, v4, n4, t4, 3, bulnum, glm::normalize(t + shiplocation), shiplocation, rot[0], rot[1], rot[2], rot[3] };
	Vertex ver5 = Vertex{ c1, v5, n5, t2, 3, bulnum, glm::normalize(t + shiplocation), shiplocation, rot[0], rot[1], rot[2], rot[3] };
	Vertex ver6 = Vertex{ c1, v6, n6, t1, 3, bulnum, glm::normalize(t + shiplocation), shiplocation, rot[0], rot[1], rot[2], rot[3] };
	Vertex ver7 = Vertex{ c1, v7, n7, t4, 3, bulnum, glm::normalize(t + shiplocation), shiplocation, rot[0], rot[1], rot[2], rot[3] };
	Vertex ver8 = Vertex{ c1, v8, n8, t3, 3, bulnum, glm::normalize(t + shiplocation), shiplocation, rot[0], rot[1], rot[2], rot[3] };

	bullets.push_back(ver1);
	bullets.push_back(ver2);
	bullets.push_back(ver5);

	bullets.push_back(ver4);
	bullets.push_back(ver8);
	bullets.push_back(ver7);

	bullets.push_back(ver5);
	bullets.push_back(ver2);
	bullets.push_back(ver6);

	bullets.push_back(ver3);
	bullets.push_back(ver4);
	bullets.push_back(ver7);

	bullets.push_back(ver1);
	bullets.push_back(ver3);
	bullets.push_back(ver2);

	bullets.push_back(ver7);
	bullets.push_back(ver8);
	bullets.push_back(ver6);

	bullets.push_back(ver2);
	bullets.push_back(ver3);
	bullets.push_back(ver4);

	bullets.push_back(ver5);
	bullets.push_back(ver7);
	bullets.push_back(ver6);

	bullets.push_back(ver1);
	bullets.push_back(ver3);
	bullets.push_back(ver5);

	bullets.push_back(ver4);
	bullets.push_back(ver8);
	bullets.push_back(ver6);

	bullets.push_back(ver3);
	bullets.push_back(ver5);
	bullets.push_back(ver7);

	bullets.push_back(ver2);
	bullets.push_back(ver6);
	bullets.push_back(ver4);
}

glm::vec3 calculateNormal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
	return glm::normalize((glm::cross(glm::normalize(v1), glm::normalize(v2)) + glm::cross(glm::normalize(v1), glm::normalize(v3)) + glm::cross(glm::normalize(v2), glm::normalize(v3))) / 3.0f);
}

void addEnemyData() {
	enemydata.resize(5);
	enemydata.push_back(enemydata0);
	enemydata.push_back(enemydata1);
	enemydata.push_back(enemydata2);
	enemydata.push_back(enemydata3);
	enemydata.push_back(enemydata4);
}

void initShipMesh()
{
	//vertices with 3 coordinates
	ShipVertices3f.resize(12);
	//normals with 3 coordinates
	ShipNormals3f.resize(12);
	//texture coords per vertex
	ShipTexCoords2f.resize(8);
	//triangles (2 per default)
	ShipTriangles3ui.resize(12);
	//per vertex colors 
	ShipColors3f.resize(12);
	//define coords
	ShipVertices3f[0] = -0.1;
	ShipVertices3f[1] = -0.1;
	ShipVertices3f[2] = -0.05;

	ShipVertices3f[3] = 0.1;
	ShipVertices3f[4] = -0.1;
	ShipVertices3f[5] = -0.05;

	ShipVertices3f[6] = 0;
	ShipVertices3f[7] = -0.1;
	ShipVertices3f[8] = 0.05;

	ShipVertices3f[9] = 0;
	ShipVertices3f[10] = 0.1;
	ShipVertices3f[11] = -0.05;
	ShipNormals3f[0] = -1;
	ShipNormals3f[1] = -0.3;
	ShipNormals3f[2] = -0.3;
	ShipNormals3f[3] = 1;
	ShipNormals3f[4] = -0.3;
	ShipNormals3f[5] = -0.3;
	ShipNormals3f[6] = 0;
	ShipNormals3f[7] = 0.3;
	ShipNormals3f[8] = 1;
	ShipNormals3f[9] = 0;
	ShipNormals3f[10] = 1;
	ShipNormals3f[11] = -0.3;

	ShipTexCoords2f[0] = 0;
	ShipTexCoords2f[1] = 0;

	ShipTexCoords2f[2] = 1;
	ShipTexCoords2f[3] = 0;

	ShipTexCoords2f[4] = 0.5;
	ShipTexCoords2f[5] = 0.25;

	ShipTexCoords2f[6] = 0.5;
	ShipTexCoords2f[7] = 1;

	ShipColors3f[0] = 1;
	ShipColors3f[1] = 1;
	ShipColors3f[2] = 1;

	ShipColors3f[3] = 1;
	ShipColors3f[4] = 1;
	ShipColors3f[5] = 1;

	ShipColors3f[6] = 1;
	ShipColors3f[7] = 1;
	ShipColors3f[8] = 1;

	ShipColors3f[9] = 1;
	ShipColors3f[10] = 1;
	ShipColors3f[11] = 1;

	ShipTriangles3ui[0] = 0;
	ShipTriangles3ui[1] = 2;
	ShipTriangles3ui[2] = 1;

	ShipTriangles3ui[3] = 0;
	ShipTriangles3ui[4] = 1;
	ShipTriangles3ui[5] = 3;

	ShipTriangles3ui[6] = 0;
	ShipTriangles3ui[7] = 3;
	ShipTriangles3ui[8] = 2;

	ShipTriangles3ui[9] = 2;
	ShipTriangles3ui[10] = 3;
	ShipTriangles3ui[11] = 1;


}

void initEnemyShipMesh()
{
	//vertices with 3 coordinates
	eShipVertices3f.resize(12);
	//normals with 3 coordinates
	eShipNormals3f.resize(12);
	//texture coords per vertex
	eShipTexCoords2f.resize(8);
	//triangles (2 per default)
	eShipTriangles3ui.resize(12);
	//per vertex colors 
	eShipColors3f.resize(12);
	//define coords
	eShipVertices3f[0] = -0.1;
	eShipVertices3f[1] = 0.1;
	eShipVertices3f[2] = -0.05;

	eShipVertices3f[3] = 0.1;
	eShipVertices3f[4] = 0.1;
	eShipVertices3f[5] = -0.05;

	eShipVertices3f[6] = 0;
	eShipVertices3f[7] = 0.1;
	eShipVertices3f[8] = 0.05;

	eShipVertices3f[9] = 0;
	eShipVertices3f[10] = -0.1;
	eShipVertices3f[11] = -0.05;
	
	eShipNormals3f[0] = -1;
	eShipNormals3f[1] = 0.3;
	eShipNormals3f[2] = -0.3;
	eShipNormals3f[3] = 1;
	eShipNormals3f[4] = 0.3;
	eShipNormals3f[5] = -0.3;
	eShipNormals3f[6] = 0;
	eShipNormals3f[7] = -0.3;
	eShipNormals3f[8] = 1;
	eShipNormals3f[9] = 0;
	eShipNormals3f[10] = -1;
	eShipNormals3f[11] = -0.3;

	eShipTexCoords2f[0] = 0;
	eShipTexCoords2f[1] = 0;

	eShipTexCoords2f[2] = 1;
	eShipTexCoords2f[3] = 0;

	eShipTexCoords2f[4] = 0.5;
	eShipTexCoords2f[5] = 0.25;

	eShipTexCoords2f[6] = 0.5;
	eShipTexCoords2f[7] = 1;

	eShipColors3f[0] = 1;
	eShipColors3f[1] = 1;
	eShipColors3f[2] = 1;

	eShipColors3f[3] = 1;
	eShipColors3f[4] = 1;
	eShipColors3f[5] = 1;

	eShipColors3f[6] = 1;
	eShipColors3f[7] = 1;
	eShipColors3f[8] = 1;

	eShipColors3f[9] = 1;
	eShipColors3f[10] = 1;
	eShipColors3f[11] = 1;

	eShipTriangles3ui[0] = 0;
	eShipTriangles3ui[1] = 2;
	eShipTriangles3ui[2] = 1;

	eShipTriangles3ui[3] = 0;
	eShipTriangles3ui[4] = 1;
	eShipTriangles3ui[5] = 3;

	eShipTriangles3ui[6] = 0;
	eShipTriangles3ui[7] = 3;
	eShipTriangles3ui[8] = 2;

	eShipTriangles3ui[9] = 2;
	eShipTriangles3ui[10] = 3;
	eShipTriangles3ui[11] = 1;


}

void initSurfaceMesh()
{
	//vertices with 3 coordinates
	SurfaceVertices3f.resize(3 * NbVertX*NbVertY);
	//normals with 3 coordinates
	SurfaceNormals3f.resize(3 * NbVertX*NbVertY);
	//triangles (2 per default)
	SurfaceTriangles3ui.resize(3 * 2 * (NbVertX - 1) * (NbVertY - 1));

	SurfaceTexCoords2f.resize(2 * NbVertX*NbVertY);
	//per vertex colors 
	SurfaceColors3f.resize(3 * NbVertX * NbVertY);
	//define coords
	int count = 0;
	for (int i = 0; i < NbVertY; ++i) {
		for (int j = 0; j < NbVertX; j++) {
			SurfaceVertices3f[count] = (j - 0.5 * NbVertX) / 20;
			SurfaceNormals3f[count] = 0;
			SurfaceColors3f[count] = 1;
			count++;
			SurfaceVertices3f[count] = (i - 0.2 * NbVertY) / 20;
			SurfaceNormals3f[count] = 0;
			SurfaceColors3f[count] = 1;
			count++;
			SurfaceVertices3f[count] = -1;
			SurfaceNormals3f[count] = 1;
			SurfaceColors3f[count] = 1.0;
			count++;
		}
	}

	count = 0;
	for (int i = 0; i < NbVertY; ++i) {
		for (int j = 0; j < NbVertX; j++) {
			SurfaceTexCoords2f[count] = (float)i / 10;
			count++;
			SurfaceTexCoords2f[count] = (float)j / 10;
			count++;
		}
	}

	count = 0;
	for (int i = 0; i < NbVertY - 1; ++i) {
		for (int j = 0; j < NbVertX - 1; ++j) {
			SurfaceTriangles3ui[count] = j + (NbVertX * i);
			count++;
			SurfaceTriangles3ui[count] = j + 1 + (NbVertX * i);
			count++;
			SurfaceTriangles3ui[count] = j + 1 + (NbVertX * (i + 1));
			count++;

			SurfaceTriangles3ui[count] = j + 1 + (NbVertX * (i + 1));
			count++;
			SurfaceTriangles3ui[count] = j + (NbVertX * (i + 1));
			count++;
			SurfaceTriangles3ui[count] = j + (NbVertX * i);
			count++;
		}
	}
}

void addVertices(std::vector<Vertex> obs) {
	for (int i = 0; i < obs.size(); i++) {
		vertices.push_back(obs[i]);
	}
}

void drawSurface() {
	for (int t = 0; t < SurfaceTriangles3ui.size(); t += 3) {
		Vertex vert;
		int count = 0;
		//first vertex
		for (int i = 0; i < 3; i++) {
			int index = i + t;
			int ver = SurfaceTriangles3ui[index];
			glm::vec3 col = glm::vec3(SurfaceColors3f[3 * ver], SurfaceColors3f[3 * ver + 1], SurfaceColors3f[3 * ver + 2]);
			glm::vec3 pos = glm::vec3(SurfaceVertices3f[3 * ver], SurfaceVertices3f[3 * ver + 1], SurfaceVertices3f[3 * ver + 2]);
			glm::vec3 nor = glm::vec3(SurfaceNormals3f[3 * ver], SurfaceNormals3f[3 * ver + 1], SurfaceNormals3f[3 * ver + 2]);
			glm::vec2 uv = glm::vec2(SurfaceTexCoords2f[2 * ver], SurfaceTexCoords2f[2 * ver + 1]);
			landscape.push_back(Vertex{ col, pos, nor, uv, 0 });
		}
	}
	addVertices(landscape);
}

void drawShip() {
	for (int t = 0; t < ShipTriangles3ui.size(); t += 3) {
		Vertex vert;
		int count = 0;
		//first vertex
		for (int i = 0; i < 3; i++) {
			int index = i + t;
			int ver = ShipTriangles3ui[index];
			glm::vec3 col = glm::vec3(ShipColors3f[3 * ver], ShipColors3f[3 * ver + 1], ShipColors3f[3 * ver + 2]);
			glm::vec3 pos = glm::vec3(ShipVertices3f[3 * ver], ShipVertices3f[3 * ver + 1], ShipVertices3f[3 * ver + 2]);
			glm::vec3 nor = glm::vec3(ShipNormals3f[3 * ver], ShipNormals3f[3 * ver + 1], ShipNormals3f[3 * ver + 2]);
			glm::vec2 uv = glm::vec2(ShipTexCoords2f[2 * ver], ShipTexCoords2f[2 * ver + 1]);
			ship.push_back(Vertex{ col, pos, glm::normalize(nor), uv, 1 });
		}
	}
	addVertices(ship);
}

void drawEnemyShip() {
	for (int n = 0; n < 5; n++) {
		for (int t = 0; t < eShipTriangles3ui.size(); t += 3) {
			Vertex vert;
			int count = 0;
			//first vertex
			for (int i = 0; i < 3; i++) {
				int index = i + t;
				int ver = eShipTriangles3ui[index];
				glm::vec3 col = glm::vec3(eShipColors3f[3 * ver], eShipColors3f[3 * ver + 1], eShipColors3f[3 * ver + 2]);
				glm::vec3 pos = glm::vec3(eShipVertices3f[3 * ver], eShipVertices3f[3 * ver + 1], eShipVertices3f[3 * ver + 2]);
				glm::vec3 nor = glm::vec3(eShipNormals3f[3 * ver], eShipNormals3f[3 * ver + 1], eShipNormals3f[3 * ver + 2]);
				glm::vec2 uv = glm::vec2(eShipTexCoords2f[2 * ver], eShipTexCoords2f[2 * ver + 1]);
				enemyShip.push_back(Vertex{ col, pos, glm::normalize(nor), uv, 5, n});
			}
		}
	}
	addVertices(enemyShip);
}

void removeBullets() {
	for (int i = vertices.size() - 1; i >= 0; i += -1) {
		if (vertices[i].ID == 3) {
			vertices.erase(vertices.begin() + i);
		}
		else {
			return;
		}
	}
}

glm::vec2 bulletPosition(int num) {
	glm::vec2 res;
	glm::vec2 traj;
	glm::vec2 offset;
	for (int i = 0; i < bullets.size(); i++) {
		if (bullets[i].bulnum == num) {
			traj = bullets[i].traj;
			traj.y = -traj.y;
			offset = bullets[i].offset;
			break;
		}
	}
	if (num < 4) {
		res = offset + 0.4f * traj * bulletmult[num];
	}
	if (num >= 4) {
		res = offset + 0.4f * traj * bulletmult2[num - 4];
	}
	return res;
}

void ShootBullet() {
	if (prevtime + 0.250 <= glfwGetTime()) {
		initBulletMesh(nextbul, glm::vec2(xpos, ypos), model);
		if (nextbul < 4) {
			bulletmult[nextbul] = 0.0f;
		}
		if (nextbul >= 4) {
			bulletmult2[nextbul - 4] = 0.0f;
		}
		nextbul++;
		if (nextbul >= 8) {
			nextbul = 0;
		}
		if (bullets.size() >= 36 * 8) {
			bullets.erase(bullets.begin(), bullets.begin() + 36);
		}
		removeBullets();
		addVertices(bullets);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
		prevtime = glfwGetTime();
	}
	for (int i = 0; i < 4; i++) {
		bulletmult[i] += 0.1f;
		bulletmult2[i] += 0.1f;
	}
}

void rotateWeapon(GLuint mainProgram) {
	xpos += -(WIDTH / 2);
	ypos += -(HEIGHT / 2);
	xpos += -(shiplocation.x * (WIDTH / 2) / 2.0f);
	ypos += (shiplocation.y * (HEIGHT / 2) / 1.6f);
	float theta = 0.0f;
	if (xpos > 0) {
		theta = glm::acos(glm::dot(normalize(glm::vec2(xpos, ypos)), glm::vec2(0, -1)));
	}
	if (xpos < 0) {
		theta = -glm::acos(glm::dot(normalize(glm::vec2(xpos, ypos)), glm::vec2(0, -1)));
	}
	model = glm::scale(glm::mat4(1.0f), glm::vec3(0.04, 0.1, 1));
	model = glm::rotate(model, theta, glm::vec3(0, 0, 1));
	glUniformMatrix4fv(glGetUniformLocation(mainProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
}

void moveShip() {
	if (apress) {
		shiplocation.x += -speed;
		if (shiplocation.x < -1.4f) {
			shiplocation.x += speed;
		}
	}
	if (dpress) {
		shiplocation.x += speed;
		if (shiplocation.x > 1.4f) {
			shiplocation.x += -speed;
		}
	}
	if (wpress) {
		shiplocation.y += speed;
		if (shiplocation.y > 1.4f) {
			shiplocation.y += -speed;
		}
	}
	if (spress) {
		shiplocation.y += -speed;
		if (shiplocation.y < -1.4f) {
			shiplocation.y += speed;
		}
	}
}

void UpdateEnemies() {
	for (int i = 0; i < 5; i++) {
		//if health = 0, set inactive
		if (enemydata[i].z <= 0.0f) {
			enemydata[i].w = 0;
		}
		//if inactive, remove from screen
		if (enemydata[i].w == 0.0f) {
			enemydata[i].x = -10;
			enemydata[i].y = -10;
			//check for spawn
			float r1 = rand() % 1000;
			if (r1 <= 1000 * spawnrate) {
				float r2 = rand() % 100;
				r2 += -50;
				r2 = r2 / 33;
				enemydata[i].w = 1;
				enemydata[i].z = maxhp;
				enemydata[i].x = r2;
				enemydata[i].y = 3.6;
			}
		}
		// if active, move
		if (enemydata[i].w == 1) {
			if (enemydata[i].y <= shiplocation.y + 1) {
				glm::vec2 enemypos = glm::vec2(enemydata[i].x, enemydata[i].y);
				glm::vec2 move = normalize(shiplocation - enemypos);
				enemydata[i].x += enemyspeed * move.x;
				enemydata[i].y += enemyspeed * move.y;
			}
			else {
				enemydata[i].y += -enemyspeed;
			}
			if (enemydata[i].y <= -2) {
				enemydata[i].w = 0;
			}
		}
	}
}

//checks collision between objects
void checkCollision() {
	for (int s = 0; s < 5; s++) {
		if (enemydata[s].w == 1) {
			for (int i = 0; i < 8; i++) {
				glm::vec2 bulpos = bulletPosition(i);
				if (!(bulpos.x == 0 && bulpos.y == 0)) {
					if (bulpos.x <= enemydata[s].x + 0.1f && bulpos.x >= enemydata[s].x - 0.1f) {
						if (bulpos.y <= enemydata[s].y + 0.15f && bulpos.y >= enemydata[s].y - 0.15f) {
							enemydata[s].z += -1;
							if (i < 4) {
								bulletmult[i] += 10;
							}
							if (i >= 4) {
								bulletmult2[i - 4] += 10;
							}
						}
					}
				}
			}
			if (shiplocation.x <= enemydata[s].x + 0.1f && shiplocation.x >= enemydata[s].x - 0.1f) {
				if (shiplocation.y <= enemydata[s].y + 0.15f && shiplocation.y >= enemydata[s].y - 0.15f) {
					enemydata[s].z += -3;
					hp += -1;
				}
			}
		}
	}
}

void setUniforms(GLuint mainProgram, Camera mainCamera, Camera secondCamera) {
	glm::mat4 mvp = mainCamera.vpMatrix();
	glm::mat4 lightMVP = secondCamera.vpMatrix();
	glUniformMatrix4fv(glGetUniformLocation(mainProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(glGetUniformLocation(mainProgram, "lightMVP"), 1, GL_FALSE, glm::value_ptr(lightMVP));
	// Set view position
	glUniform3fv(glGetUniformLocation(mainProgram, "viewPos"), 1, glm::value_ptr(mainCamera.position));

	glUniform3fv(glGetUniformLocation(mainProgram, "lightPos"), 1, glm::value_ptr(secondCamera.position));

	glUniform4fv(glGetUniformLocation(mainProgram, "bulletmult"), 1, glm::value_ptr(bulletmult));
	glUniform4fv(glGetUniformLocation(mainProgram, "bulletmult2"), 1, glm::value_ptr(bulletmult2));
	glUniform4fv(glGetUniformLocation(mainProgram, "enemydata0"), 1, glm::value_ptr(enemydata[0]));
	glUniform4fv(glGetUniformLocation(mainProgram, "enemydata1"), 1, glm::value_ptr(enemydata[1]));
	glUniform4fv(glGetUniformLocation(mainProgram, "enemydata2"), 1, glm::value_ptr(enemydata[2]));
	glUniform4fv(glGetUniformLocation(mainProgram, "enemydata3"), 1, glm::value_ptr(enemydata[3]));
	glUniform4fv(glGetUniformLocation(mainProgram, "enemydata4"), 1, glm::value_ptr(enemydata[4]));

	//bind shiplocation
	glUniform2fv(glGetUniformLocation(mainProgram, "shiptrans"), 1, glm::value_ptr(shiplocation));

	// Expose current time in shader uniform
	glUniform1f(glGetUniformLocation(mainProgram, "time"), (float)(static_cast<int>(glfwGetTime() * 1000) % 20000) / 1000);
	glUniform1f(glGetUniformLocation(mainProgram, "hp"), (float)(static_cast<int>(maxhp)));
}

void InitParticles(GLuint texLandscape[7])
{
	GLuint particleProgram = glCreateProgram();
	std::string vertexShaderCode = readFile("particle.vert"); 		
	const char* vertexShaderCodePtr = vertexShaderCode.data();
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderCodePtr, nullptr);
	glCompileShader(vertexShader);

	std::string fragmentShaderCode = readFile("particle.frag");
	const char* fragmentShaderCodePtr = fragmentShaderCode.data();
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderCodePtr, nullptr);
	glCompileShader(fragmentShader);

	glAttachShader(particleProgram, vertexShader);
	glAttachShader(particleProgram, fragmentShader);
	glLinkProgram(particleProgram);
	
	//// Create particles
	Particles = new ParticleGenerator(
		particleProgram,
		texLandscape[6],
		500
	);
}

void UpdateParticles()
{
	Particles->Update(glfwGetTime() - particleTime, glm::vec2(0, 0), 2, glm::vec2(0, 0));
	particleTime = glfwGetTime();
}

int main() {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW!" << std::endl;
		return EXIT_FAILURE;
	}
	addEnemyData();
	initSurfaceMesh();
	drawSurface();
	initShipMesh();
	drawShip();
	initWeaponMesh();
	initEnemyShipMesh();
	drawEnemyShip();
	//////////////////// Create window and OpenGL 4.3 debug context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Shadow mapping practical", nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create OpenGL context!" << std::endl;
		std::cout << "Press enter to close."; getchar();
		return EXIT_FAILURE;
	}

	glfwSetKeyCallback(window, keyboardHandler);
	glfwSetMouseButtonCallback(window, mouseButtonHandler);
	glfwSetCursorPosCallback(window, cursorPosHandler);

	// Activate the OpenGL context
	glfwMakeContextCurrent(window);

	// Initialize GLEW extension loader
	glewExperimental = GL_TRUE;
	glewInit();

	// Set up OpenGL debug callback
	glDebugMessageCallback(debugCallback, nullptr);

	GLuint mainProgram = glCreateProgram();
	GLuint shadowProgram = glCreateProgram();


	////////////////// Load and compile main shader program
	{
		std::string vertexShaderCode = readFile("shader.vert");
		const char* vertexShaderCodePtr = vertexShaderCode.data();
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderCodePtr, nullptr);
		glCompileShader(vertexShader);

		std::string fragmentShaderCode = readFile("shader.frag");
		const char* fragmentShaderCodePtr = fragmentShaderCode.data();

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderCodePtr, nullptr);
		glCompileShader(fragmentShader);

		if (!checkShaderErrors(vertexShader) || !checkShaderErrors(fragmentShader)) {
			std::cerr << "Shader(s) failed to compile!" << std::endl;
			std::cout << "Press enter to close."; getchar();
			return EXIT_FAILURE;
		}

		// Combine vertex and fragment shaders into single shader program
		glAttachShader(mainProgram, vertexShader);
		glAttachShader(mainProgram, fragmentShader);
		glLinkProgram(mainProgram);

		if (!checkProgramErrors(mainProgram)) {
			std::cerr << "Main program failed to link!" << std::endl;
			std::cout << "Press enter to close."; getchar();
			return EXIT_FAILURE;
		}
	}

	////////////////// Load and compile shadow shader program
	{
		std::string vertexShaderCode = readFile("shadow.vert");
		const char* vertexShaderCodePtr = vertexShaderCode.data();

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderCodePtr, nullptr);
		glCompileShader(vertexShader);

		std::string fragmentShaderCode = readFile("shadow.frag");
		const char* fragmentShaderCodePtr = fragmentShaderCode.data();

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderCodePtr, nullptr);
		glCompileShader(fragmentShader);

		if (!checkShaderErrors(vertexShader) || !checkShaderErrors(fragmentShader)) {
			std::cerr << "Shader(s) failed to compile!" << std::endl;
			return EXIT_FAILURE;
		}

		// Combine vertex and fragment shaders into single shader program
		glAttachShader(shadowProgram, vertexShader);
		glAttachShader(shadowProgram, fragmentShader);
		glLinkProgram(shadowProgram);

		if (!checkProgramErrors(shadowProgram)) {
			std::cerr << "Shadow program failed to link!" << std::endl;
			return EXIT_FAILURE;
		}
	}

	//////////////////// Load textures
	// Create Texture
	int texwidth, texheight, texchannels;
	stbi_uc* pixels = stbi_load("soil.jpg", &texwidth, &texheight, &texchannels, 3);

	GLuint texLandscape[7];
	glGenTextures(7, texLandscape);
	glBindTexture(GL_TEXTURE_2D, texLandscape[0]);

	// Upload pixels into texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texwidth, texheight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	// Set behaviour for when texture coordinates are outside the [0, 1] range
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set interpolation for texture sampling (GL_NEAREST for no interpolation)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	pixels = stbi_load("ship.png", &texwidth, &texheight, &texchannels, 3);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texLandscape[1]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texwidth, texheight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	pixels = stbi_load("snow.jpg", &texwidth, &texheight, &texchannels, 3);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texLandscape[2]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texwidth, texheight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	pixels = stbi_load("bullet.png", &texwidth, &texheight, &texchannels, 3);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texLandscape[3]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texwidth, texheight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	pixels = stbi_load("bullet2.png", &texwidth, &texheight, &texchannels, 3);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texLandscape[4]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texwidth, texheight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	pixels = stbi_load("metal.jpg", &texwidth, &texheight, &texchannels, 3);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texLandscape[5]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texwidth, texheight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);


	pixels = stbi_load("particle.png", &texwidth, &texheight, &texchannels, 3);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texLandscape[6]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texwidth, texheight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	////////////////////////// Load vertices of model
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, "scene.obj")) {
			std::cerr << err << std::endl;
			return EXIT_FAILURE;
		}
	}


	//////////////////// Create Vertex Buffer Object
	GLuint vbo;
	GLuint vao;
	{
		
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		// Bind vertex data to shader inputs using their index (location)
		// These bindings are stored in the Vertex Array Object
		
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// The position vectors should be retrieved from the specified Vertex Buffer Object with given offset and stride
		// Stride is the distance in bytes between vertices
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
		glEnableVertexAttribArray(0);

		// The normals should be retrieved from the same Vertex Buffer Object (glBindBuffer is optional)
		// The offset is different and the data should go to input 1 instead of 0
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
		glEnableVertexAttribArray(1);

		// color
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));
		glEnableVertexAttribArray(2);

		// UV
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, UV)));
		glEnableVertexAttribArray(3);

		// ID
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, ID)));
		glEnableVertexAttribArray(4);

		// Bulnum
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, bulnum)));
		glEnableVertexAttribArray(5);

		// trajectory
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, traj)));
		glEnableVertexAttribArray(6);

		// orientation
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, ori0)));
		glEnableVertexAttribArray(8);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, ori1)));
		glEnableVertexAttribArray(9);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, ori2)));
		glEnableVertexAttribArray(10);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, ori3)));
		glEnableVertexAttribArray(11);

		// offset
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, offset)));
		glEnableVertexAttribArray(7);
	}
	//////////////////// Create Shadow Texture
	GLuint texShadow;
	const int SHADOWTEX_WIDTH = 1024;
	const int SHADOWTEX_HEIGHT = 1024;
	glGenTextures(1, &texShadow);
	glBindTexture(GL_TEXTURE_2D, texShadow);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOWTEX_WIDTH, SHADOWTEX_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	// Set behaviour for when texture coordinates are outside the [0, 1] range
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Set interpolation for texture sampling (GL_NEAREST for no interpolation)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//////////////////// Create framebuffer for extra texture
	GLuint framebuffer;
	glGenFramebuffers(1, &framebuffer);

	/////////////////// Set shadow texure as depth buffer for this framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texShadow, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/////////////////// Create main camera
	Camera mainCamera;
	mainCamera.aspect = WIDTH / (float)HEIGHT;
	mainCamera.position = glm::vec3(0.0f, -3.0f, 3.3f);
	mainCamera.forward = -mainCamera.position;
	projection = mainCamera.pMatrix();
	view = mainCamera.vMatrix();

	/////////////////// Create second camera
	Camera secondCamera;
	secondCamera.aspect = WIDTH / (float)HEIGHT;
	secondCamera.position = glm::vec3(0.0f, 6.0f, 6.0f);
	secondCamera.forward = -secondCamera.position;

	InitParticles(texLandscape);

	// Main loop
	while (!glfwWindowShouldClose(window)) {
		bultexture = !bultexture;
		glfwGetCursorPos(window, &xpos, &ypos);
		glfwPollEvents();
		moveShip();
		checkCollision();
		UpdateEnemies();
		UpdateParticles();
		////////// Stub code for you to fill in order to render the shadow map
		{
			// Bind the off-screen framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

			// Clear the shadow map and set needed options
			glClearDepth(1.0f);
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			// Bind the shader
			glUseProgram(shadowProgram);

			// Set viewport size
			glViewport(0, 0, SHADOWTEX_WIDTH, SHADOWTEX_HEIGHT);

			// Execute draw command
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());


			// .... HERE YOU MUST ADD THE CORRECT UNIFORMS FOR RENDERING THE SHADOW MAP
			glm::mat4 mvp2 = secondCamera.vpMatrix();
			glUniformMatrix4fv(glGetUniformLocation(shadowProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp2));
			glUniformMatrix4fv(glGetUniformLocation(shadowProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniform4fv(glGetUniformLocation(shadowProgram, "bulletmult"), 1, glm::value_ptr(bulletmult));
			glUniform4fv(glGetUniformLocation(shadowProgram, "bulletmult2"), 1, glm::value_ptr(bulletmult2));
			glUniform4fv(glGetUniformLocation(shadowProgram, "enemydata0"), 1, glm::value_ptr(enemydata[0]));
			glUniform4fv(glGetUniformLocation(shadowProgram, "enemydata1"), 1, glm::value_ptr(enemydata[1]));
			glUniform4fv(glGetUniformLocation(shadowProgram, "enemydata2"), 1, glm::value_ptr(enemydata[2]));
			glUniform4fv(glGetUniformLocation(shadowProgram, "enemydata3"), 1, glm::value_ptr(enemydata[3]));
			glUniform4fv(glGetUniformLocation(shadowProgram, "enemydata4"), 1, glm::value_ptr(enemydata[4]));
			//bind shiplocation
			glUniform2fv(glGetUniformLocation(shadowProgram, "shiptrans"), 1, glm::value_ptr(shiplocation));

			// Bind vertex data
			glBindVertexArray(vao);

			// Execute draw command
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());

			// Unbind the off-screen framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		// Bind the shader
		glUseProgram(mainProgram);

		//rotate the weapon
		rotateWeapon(mainProgram);
		ShootBullet();

		//add uniforms
		updateCamera(mainCamera);
		setUniforms(mainProgram, mainCamera, secondCamera);
		// Bind vertex data
		glBindVertexArray(vao);

		// Bind the shadow map to texture slot 0
		GLint texture_unit = 0;
		glActiveTexture(GL_TEXTURE0 + texture_unit);
		glBindTexture(GL_TEXTURE_2D, texShadow);
		glUniform1i(glGetUniformLocation(mainProgram, "texShadow"), texture_unit);

		// Bind the landscape map to texture slot 1
		GLint texture_unit1 = 1;
		glActiveTexture(GL_TEXTURE0 + texture_unit1);
		glBindTexture(GL_TEXTURE_2D, texLandscape[0]);
		glUniform1i(glGetUniformLocation(mainProgram, "landscape"), 1);

		// Bind the metal to texture slot 2
		GLint texture_unit2 = 2;
		glActiveTexture(GL_TEXTURE0 + texture_unit2);
		glBindTexture(GL_TEXTURE_2D, texLandscape[1]);
		glUniform1i(glGetUniformLocation(mainProgram, "ship"), texture_unit2);

		// Bind the snow to texture slot 3
		GLint texture_unit3 = 3;
		glActiveTexture(GL_TEXTURE0 + texture_unit3);
		glBindTexture(GL_TEXTURE_2D, texLandscape[2]);
		glUniform1i(glGetUniformLocation(mainProgram, "snow"), texture_unit3);

		// Bind the metal to texture slot 5
		GLint texture_unit5 = 5;
		glActiveTexture(GL_TEXTURE0 + texture_unit5);
		glBindTexture(GL_TEXTURE_2D, texLandscape[5]);
		glUniform1i(glGetUniformLocation(mainProgram, "metal"), texture_unit5);

		// swap between two bullet textures
		if (bultexture) {
			// Bind the first bullet texture to texture slot 4
			GLint texture_unit4 = 4;
			glActiveTexture(GL_TEXTURE0 + texture_unit4);
			glBindTexture(GL_TEXTURE_2D, texLandscape[3]);
			glUniform1i(glGetUniformLocation(mainProgram, "texbullet"), texture_unit4);
		}
		else {
			// Bind the second bullet texture to texture slot 4
			GLint texture_unit4 = 4;
			glActiveTexture(GL_TEXTURE0 + texture_unit4);
			glBindTexture(GL_TEXTURE_2D, texLandscape[4]);
			glUniform1i(glGetUniformLocation(mainProgram, "texbullet"), texture_unit4);
		}
		// Set viewport size
		glViewport(0, 0, WIDTH, HEIGHT);

		// Clear the framebuffer to black and depth to maximum value
		glClearDepth(1.0f);
		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		// Execute draw command
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		// Present result to the screen
		glfwSwapBuffers(window);
	}

	glDeleteFramebuffers(1, &framebuffer);

	glDeleteTextures(1, &texShadow);

	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}
