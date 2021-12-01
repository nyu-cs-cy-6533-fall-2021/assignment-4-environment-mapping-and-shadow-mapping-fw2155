// This example is heavily based on the tutorial at https://open.gl

// OpenGL Helpers to reduce the clutter
#include "Helpers.h"
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <sstream>
#include <string>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#else
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#endif

// OpenGL Mathematics Library
#include <glm/glm.hpp> // glm::vec3
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
// Timer
#include <chrono>
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include <filesystem>
VertexBufferObject cubeVBO;
VertexBufferObject cubeColorVBO;
std::vector<glm::vec3> V_cube(3000);
std::vector<glm::vec3> C_cube(3000);

VertexBufferObject bunnyVBO;
VertexBufferObject bunnyColorVBO;
std::vector<glm::vec3> V_bunny(20000);
std::vector<glm::vec3> C_bunny(20000);

VertexBufferObject bumpyCubeVBO;
VertexBufferObject bumpyCubeColorVBO;
std::vector<glm::vec3> V_bumpyCube(20000);
std::vector<glm::vec3> C_bumpyCube(20000);

//three normal buffer
VertexBufferObject cubeNormalVBO;
std::vector<glm::vec3> cubeNormalVector(20000);
std::vector<glm::vec3> cubePhongNormal(20000);
VertexBufferObject bunnyNormalVBO;
std::vector<glm::vec3> bunnyNormalVector(20000);
VertexBufferObject bumpyNormalVBO;
std::vector<glm::vec3> bumpyNormalVector(20000);
VertexBufferObject cubeTexCoordVBO;
std::vector<glm::vec2> cubeTexCoord(20000);

VertexBufferObject planeVBO;
std::vector<glm::vec3> V_plane(100);
VertexBufferObject planeColorVBO;
std::vector<glm::vec3> C_plane(100);
VertexBufferObject planeNormalVBO;
std::vector<glm::vec3> planeNormal(100);
VertexBufferObject planeTexCoordVBO;
std::vector<glm::vec2> planeTexCoord(100);

//VBO for skybox-environment mapping
VertexBufferObject skyBoxVBO;
std::vector<glm::vec3> skyBox(100);
vector<string> faceSky(6,"");
float drawPlaneMode = 0.0;

vector<float> cubeReflection;
vector<float> bunnyReflection;
vector<float> bumpyReflection;


glm::mat4x4 MVPCubeMatrix = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
glm::mat4x4 MVPBunnyMatrix = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
glm::mat4x4 MVPBumpyCubeMatrix = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
glm::mat4x4 view = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
glm::mat4x4 projection = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
vector<glm::mat4x4> modelCube;
vector<glm::mat4x4> modelBumpyCube;
vector<glm::mat4x4> modelBunny;

int vertexNum = 0, faceNum = 0, edgeNum = 0;
int totalNumCube = 0;
int totalNumBumpyCube = 0;
int totalNumBunny = 0;

int category = 0;
int whichObject = 0;
float xmin, ymin, zmin = 0;
float xmax, ymax, zmax = 0;

float shadowModeControl = 0.0;

vector<int> currentCubeMode;
vector<int> currentBunnyMode;
vector<int> currentBumpyMode;
bool PEnable = true;

vector<int> faceBumpyVector;
vector<int> faceBunnyVector;
//float centerBunnyx, centerBunnyy, centerBunnyz=0;

// camera learned from openGL website


glm::vec3 cameraPosP = glm::vec3(0.0f, 1.0f, 3.0f);
//glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
//glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
//glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f;

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data =stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
void phong(std::vector<glm::vec3> V, std::vector<glm::vec3> N, int start, int end, std::vector<glm::vec3>& N_new);
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


std::vector<glm::vec3> calculateAverageNormal(vector<int>faceVector, std::vector<glm::vec3> V, std::vector<glm::vec3> N, int start, int end) {
	std::map<int, glm::vec3> m;
	std::map<int, int>index;
	std::vector<glm::vec3> N_new = N;
	for (int i = start; i < end; i++) {
		if (m.find(faceVector[i]) == m.end()) {
			m.insert(std::pair<int, glm::vec3>(faceVector[i], N[i]));
			index.insert(std::pair<int, int>(faceVector[i], 1));
		}
		else {
			m.at(faceVector[i]) = m.at(faceVector[i]) + N[i];
			index.at(faceVector[i]) = index.at(faceVector[i]) + 1;
		}

	}
	for (int i = start; i < end; i++) {
		N_new[i].x = m[faceVector[i]].x / index.at(faceVector[i]);
		N_new[i].y = m[faceVector[i]].y / index.at(faceVector[i]);
		N_new[i].z = m[faceVector[i]].z / index.at(faceVector[i]);
		N_new[i] = glm::normalize(N_new[i]);
	}
/*
	if (m.find(0) == m.end()) {
		m.insert(std::pair<int, glm::vec3>(0, N[0]));
	}
	
	for (int i = 0; i < 3000; i++) {
		if (m.find(V[i]) == m.end()) {
			m.insert(std::pair<glm::vec3, glm::vec3>(V[i], N[i]));
			index.insert(std::pair<glm::vec3, int>(V[i], 1));
		}
		else {
			m.at(V[i]) = m.at(V[i]) + N[i];
			index.at(V[i]) = index.at(V[i]) + 1;
		}
	}
	
	
*/


	return N_new;
}

void phong(std::vector<glm::vec3> V, std::vector<glm::vec3> N, int start, int end, std::vector<glm::vec3>& N_new) {
	vector<int> already_calculated(V.size(), 0);
	vector<int> count(V.size(), 1);
	N_new = N;

	for (int i = start; i <= end; i++) {
		if (already_calculated[i] == 0) {
			already_calculated[i] = 1;
			for (int j = i + 1; j < V.size(); j++) {
				if ((V[i] == V[j])) {
					already_calculated[j] = 1;
					N[i] += N[j];
					count[i]++;
				}
			}
		}
		else {
			for (int j = 0; j < i; j++) {
				if (V[i] == V[j]) {
					N[i] = N[j];
					count[j] = count[i];
				}
			}
		}
	}
	//cout << "count of V[0] is " << count[0]<<endl;
	for (int i = start; i <= end; i++) {
		N_new[i].x = N[i].x / count[i];
		N_new[i].y = N[i].y / count[i];
		N_new[i].z = N[i].z / count[i];
		if (N_new[i].x != 0 || N_new[i].y != 0 || N_new[i].z != 0) {
			N_new[i] = glm::normalize(N[i]);
		}
		//cout << N_new[i].x << " " << N_new[i].y<<" " << N_new[i].z << " " << i << endl;
	}
}

void readFile(string filename, vector<glm::vec3>& vertex, vector<glm::vec4>& face) {
	ifstream offFile;
	offFile.open(filename);
	if (!offFile) {
		cout << "The file cannot be opened!" << endl;
	}
	string newLine;

	getline(offFile, newLine);
	if (newLine == "OFF") {
		getline(offFile, newLine);
		istringstream in(newLine);
		in >> vertexNum >> faceNum >> edgeNum;
	}
	else {
		cout << "Not start with OFF" << endl;
		cout << newLine;
	}

	for (int i = 0; i < vertexNum; i++) {
		getline(offFile, newLine);
		istringstream in(newLine);
		glm::vec3 vertexPoint;
		in >> vertexPoint.x >> vertexPoint.y >> vertexPoint.z;
		vertex.push_back(vertexPoint);
	}
	if (filename == "C:\\Users\\Fangyi Wang\\Desktop\\base\\Assignment_4\\data\\bumpy_cube.off") {
		for (int j = 0; j < faceNum; j++) {
			getline(offFile, newLine);
			istringstream in(newLine);
			glm::vec4 facePoint;
			in >> facePoint[0] >> facePoint[1] >> facePoint[2] >> facePoint[3];
			face.push_back(facePoint);
			faceBumpyVector.push_back(facePoint[1]);
			faceBumpyVector.push_back(facePoint[2]);
			faceBumpyVector.push_back(facePoint[3]);
			//cout << faceVector[0] << faceVector[1] << faceVector[2] << endl;
		}
	}
	else {
		for (int j = 0; j < faceNum; j++) {
			getline(offFile, newLine);
			istringstream in(newLine);
			glm::vec4 facePoint;
			in >> facePoint[0] >> facePoint[1] >> facePoint[2] >> facePoint[3];
			face.push_back(facePoint);
			faceBunnyVector.push_back(facePoint[1]);
			faceBunnyVector.push_back(facePoint[2]);
			faceBunnyVector.push_back(facePoint[3]);
			//cout << faceVector[0] << faceVector[1] << faceVector[2] << endl;
		}
	}
}

void addNewCube() {

	V_cube[0 + 36 * totalNumCube] = glm::vec3(-0.5f, -0.5f, -0.5f);
	V_cube[1 + 36 * totalNumCube] = glm::vec3(0.5f, -0.5f, -0.5f);
	V_cube[2 + 36 * totalNumCube] = glm::vec3(0.5f, 0.5f, -0.5f);
	V_cube[3 + 36 * totalNumCube] = glm::vec3(0.5f, 0.5f, -0.5f);
	V_cube[4 + 36 * totalNumCube] = glm::vec3(-0.5f, 0.5f, -0.5f);
	V_cube[5 + 36 * totalNumCube] = glm::vec3(-0.5f, -0.5f, -0.5f);

	V_cube[6 + 36 * totalNumCube] = glm::vec3(-0.5f, -0.5f, 0.5f);
	V_cube[7 + 36 * totalNumCube] = glm::vec3(0.5f, -0.5f, 0.5f);
	V_cube[8 + 36 * totalNumCube] = glm::vec3(0.5f, 0.5f, 0.5f);
	V_cube[9 + 36 * totalNumCube] = glm::vec3(0.5f, 0.5f, 0.5f);
	V_cube[10 + 36 * totalNumCube] = glm::vec3(-0.5f, 0.5f, 0.5f);
	V_cube[11 + 36 * totalNumCube] = glm::vec3(-0.5f, -0.5f, 0.5f);

	V_cube[12 + 36 * totalNumCube] = glm::vec3(-0.5f, 0.5f, 0.5f);
	V_cube[13 + 36 * totalNumCube] = glm::vec3(-0.5f, 0.5f, -0.5f);
	V_cube[14 + 36 * totalNumCube] = glm::vec3(-0.5f, -0.5f, -0.5f);
	V_cube[15 + 36 * totalNumCube] = glm::vec3(-0.5f, -0.5f, -0.5f);
	V_cube[16 + 36 * totalNumCube] = glm::vec3(-0.5f, -0.5f, 0.5f);
	V_cube[17 + 36 * totalNumCube] = glm::vec3(-0.5f, 0.5f, 0.5f);

	V_cube[18 + 36 * totalNumCube] = glm::vec3(0.5f, 0.5f, 0.5f);
	V_cube[19 + 36 * totalNumCube] = glm::vec3(0.5f, 0.5f, -0.5f);
	V_cube[20 + 36 * totalNumCube] = glm::vec3(0.5f, -0.5f, -0.5f);
	V_cube[21 + 36 * totalNumCube] = glm::vec3(0.5f, -0.5f, -0.5f);
	V_cube[22 + 36 * totalNumCube] = glm::vec3(0.5f, -0.5f, 0.5f);
	V_cube[23 + 36 * totalNumCube] = glm::vec3(0.5f, 0.5f, 0.5f);

	V_cube[24 + 36 * totalNumCube] = glm::vec3(-0.5f, -0.5f, -0.5f);
	V_cube[25 + 36 * totalNumCube] = glm::vec3(0.5f, -0.5f, -0.5f);
	V_cube[26 + 36 * totalNumCube] = glm::vec3(0.5f, -0.5f, 0.5f);
	V_cube[27 + 36 * totalNumCube] = glm::vec3(0.5f, -0.5f, 0.5f);
	V_cube[28 + 36 * totalNumCube] = glm::vec3(-0.5f, -0.5f, 0.5f);
	V_cube[29 + 36 * totalNumCube] = glm::vec3(-0.5f, -0.5f, -0.5f);

	V_cube[30 + 36 * totalNumCube] = glm::vec3(-0.5f, 0.5f, -0.5f);
	V_cube[31 + 36 * totalNumCube] = glm::vec3(0.5f, 0.5f, -0.5f);
	V_cube[32 + 36 * totalNumCube] = glm::vec3(0.5f, 0.5f, 0.5f);
	V_cube[33 + 36 * totalNumCube] = glm::vec3(0.5f, 0.5f, 0.5f);
	V_cube[34 + 36 * totalNumCube] = glm::vec3(-0.5f, 0.5f, 0.5f);
	V_cube[35 + 36 * totalNumCube] = glm::vec3(-0.5f, 0.5f, -0.5f);

	
	cubeNormalVector[0 + 36 * totalNumCube] = glm::vec3(0.0f, 0.0f, -1.0f);
	cubeNormalVector[1 + 36 * totalNumCube] = glm::vec3(0.0f, 0.0f, -1.0f);
	cubeNormalVector[2 + 36 * totalNumCube] = glm::vec3(0.0f, 0.0f, -1.0f);
	cubeNormalVector[3 + 36 * totalNumCube] = glm::vec3(0.0f, 0.0f, -1.0f);
	cubeNormalVector[4 + 36 * totalNumCube] = glm::vec3(0.0f, 0.0f, -1.0f);
	cubeNormalVector[5 + 36 * totalNumCube] = glm::vec3(0.0f, 0.0f, -1.0f);

	cubeNormalVector[6 + 36 * totalNumCube] = glm::vec3(0.0f, 0.0f, 1.0f);
	cubeNormalVector[7 + 36 * totalNumCube] = glm::vec3(0.0f, 0.0f, 1.0f);
	cubeNormalVector[8 + 36 * totalNumCube] = glm::vec3(0.0f, 0.0f, 1.0f);
	cubeNormalVector[9 + 36 * totalNumCube] = glm::vec3(0.0f, 0.0f, 1.0f);
	cubeNormalVector[10 + 36 * totalNumCube] = glm::vec3(0.0f, 0.0f, 1.0f);
	cubeNormalVector[11 + 36 * totalNumCube] = glm::vec3(0.0f, 0.0f, 1.0f);

	cubeNormalVector[12 + 36 * totalNumCube] = glm::vec3(-1.0f, 0.0f, 0.0f);
	cubeNormalVector[13 + 36 * totalNumCube] = glm::vec3(-1.0f, 0.0f, 0.0f);
	cubeNormalVector[14 + 36 * totalNumCube] = glm::vec3(-1.0f, 0.0f, 0.0f);
	cubeNormalVector[15 + 36 * totalNumCube] = glm::vec3(-1.0f, 0.0f, 0.0f);
	cubeNormalVector[16 + 36 * totalNumCube] = glm::vec3(-1.0f, 0.0f, 0.0f);
	cubeNormalVector[17 + 36 * totalNumCube] = glm::vec3(-1.0f, 0.0f, 0.0f);

	cubeNormalVector[18 + 36 * totalNumCube] = glm::vec3(1.0, 0.0f, 0.0f);
	cubeNormalVector[19 + 36 * totalNumCube] = glm::vec3(1.0, 0.0f, 0.0f);
	cubeNormalVector[20 + 36 * totalNumCube] = glm::vec3(1.0, 0.0f, 0.0f);
	cubeNormalVector[21 + 36 * totalNumCube] = glm::vec3(1.0, 0.0f, 0.0f);
	cubeNormalVector[22 + 36 * totalNumCube] = glm::vec3(1.0, 0.0f, 0.0f);
	cubeNormalVector[23 + 36 * totalNumCube] = glm::vec3(1.0, 0.0f, 0.0f);

	cubeNormalVector[24 + 36 * totalNumCube] = glm::vec3(0.0f, -1.0f, 0.0f);
	cubeNormalVector[25 + 36 * totalNumCube] = glm::vec3(0.0f, -1.0f, 0.0f);
	cubeNormalVector[26 + 36 * totalNumCube] = glm::vec3(0.0f, -1.0f, 0.0f);
	cubeNormalVector[27 + 36 * totalNumCube] = glm::vec3(0.0f, -1.0f, 0.0f);
	cubeNormalVector[28 + 36 * totalNumCube] = glm::vec3(0.0f, -1.0f, 0.0f);
	cubeNormalVector[29 + 36 * totalNumCube] = glm::vec3(0.0f, -1.0f, 0.0f);

	cubeNormalVector[30 + 36 * totalNumCube] = glm::vec3(0.0f, 1.0f, 0.0f);
	cubeNormalVector[31 + 36 * totalNumCube] = glm::vec3(0.0f, 1.0f, 0.0f);
	cubeNormalVector[32 + 36 * totalNumCube] = glm::vec3(0.0f, 1.0f, 0.0f);
	cubeNormalVector[33 + 36 * totalNumCube] = glm::vec3(0.0f, 1.0f, 0.0f);
	cubeNormalVector[34 + 36 * totalNumCube] = glm::vec3(0.0f, 1.0f, 0.0f);
	cubeNormalVector[35 + 36 * totalNumCube] = glm::vec3(0.0f, 1.0f, 0.0f);
	phong(V_cube, cubeNormalVector, 36 * totalNumCube, 36 * (totalNumCube + 1) - 1, cubePhongNormal);
	totalNumCube++;
	currentCubeMode.push_back(0);
	cubeVBO.update(V_cube);
	cubeNormalVBO.update(cubePhongNormal);

	cubeTexCoord[0 + 36 * totalNumCube] = glm::vec2(0.0f, 0.0f);
	cubeTexCoord[1 + 36 * totalNumCube] = glm::vec2(1.0f, 0.0f);
	cubeTexCoord[2 + 36 * totalNumCube] = glm::vec2(1.0f, 1.0f);
	cubeTexCoord[3 + 36 * totalNumCube] = glm::vec2(1.0f, 1.0f);
	cubeTexCoord[4 + 36 * totalNumCube] = glm::vec2(0.0f, 1.0f);
	cubeTexCoord[5 + 36 * totalNumCube] = glm::vec2(0.0f, 0.0f);

	cubeTexCoord[6 + 36 * totalNumCube] = glm::vec2(0.0f, 0.0f);
	cubeTexCoord[7 + 36 * totalNumCube] = glm::vec2(1.0f, 0.0f);
	cubeTexCoord[8 + 36 * totalNumCube] = glm::vec2(1.0f, 1.0f);
	cubeTexCoord[9 + 36 * totalNumCube] = glm::vec2(1.0f, 1.0f);
	cubeTexCoord[10 + 36 * totalNumCube] = glm::vec2(0.0f, 1.0f);
	cubeTexCoord[11 + 36 * totalNumCube] = glm::vec2(0.0f, 0.0f);

	cubeTexCoord[12 + 36 * totalNumCube] = glm::vec2(1.0f, 0.0f);
	cubeTexCoord[13 + 36 * totalNumCube] = glm::vec2(1.0f, 1.0f);
	cubeTexCoord[14 + 36 * totalNumCube] = glm::vec2(0.0f, 1.0f);
	cubeTexCoord[15 + 36 * totalNumCube] = glm::vec2(0.0f, 1.0f);
	cubeTexCoord[16 + 36 * totalNumCube] = glm::vec2(0.0f, 0.0f);
	cubeTexCoord[17 + 36 * totalNumCube] = glm::vec2(1.0f, 0.0f);

	cubeTexCoord[18 + 36 * totalNumCube] = glm::vec2(1.0f, 0.0f);
	cubeTexCoord[19 + 36 * totalNumCube] = glm::vec2(1.0f, 1.0f);
	cubeTexCoord[20 + 36 * totalNumCube] = glm::vec2(0.0f, 1.0f);
	cubeTexCoord[21 + 36 * totalNumCube] = glm::vec2(0.0f, 1.0f);
	cubeTexCoord[22 + 36 * totalNumCube] = glm::vec2(0.0f, 0.0f);
	cubeTexCoord[23 + 36 * totalNumCube] = glm::vec2(1.0f, 0.0f);

	cubeTexCoord[24 + 36 * totalNumCube] = glm::vec2(0.0f, 1.0f);
	cubeTexCoord[25 + 36 * totalNumCube] = glm::vec2(1.0f, 1.0f);
	cubeTexCoord[26 + 36 * totalNumCube] = glm::vec2(1.0f, 0.0f);
	cubeTexCoord[27 + 36 * totalNumCube] = glm::vec2(1.0f, 0.0f);
	cubeTexCoord[28 + 36 * totalNumCube] = glm::vec2(0.0f, 0.0f);
	cubeTexCoord[29 + 36 * totalNumCube] = glm::vec2(0.0f, 1.0f);

	cubeTexCoord[30 + 36 * totalNumCube] = glm::vec2(0.0f, 1.0f);
	cubeTexCoord[31 + 36 * totalNumCube] = glm::vec2(1.0f, 1.0f);
	cubeTexCoord[32 + 36 * totalNumCube] = glm::vec2(1.0f, 0.0f);
	cubeTexCoord[33 + 36 * totalNumCube] = glm::vec2(1.0f, 0.0f);
	cubeTexCoord[34 + 36 * totalNumCube] = glm::vec2(0.0f, 0.0f);
	cubeTexCoord[35 + 36 * totalNumCube] = glm::vec2(0.0f, 1.0f);

	cubeTexCoordVBO.update(cubeTexCoord);
	cubeReflection.push_back(0.0);

}

void addNewBunny() {
	vector<glm::vec3> vertex;
	vector<glm::vec4> face;
	readFile("C:\\Users\\Fangyi Wang\\Desktop\\base\\Assignment_4\\data\\bunny.off", vertex, face);
	for (int i = 0; i < face.size(); i++) {
		glm::vec4 vec = face[i];
		V_bunny[(3 * i) + 3 * 1000 * (totalNumBunny)] = vertex[vec[1]];
		V_bunny[(3 * i + 1) + 3 * 1000 * (totalNumBunny)] = vertex[vec[2]];
		V_bunny[(3 * i + 2) + 3 * 1000 * (totalNumBunny)] = vertex[vec[3]];
		//cout << "Current vertex index used up to " << (3 * i + 2) + 3 * 1000 * (totalNumBunny) << endl;
	}

	xmin = V_bunny[0].x;
	ymin = V_bunny[0].y;
	zmin = V_bunny[0].z;

	xmax = V_bunny[0].x;
	ymax = V_bunny[0].y;
	zmax = V_bunny[0].z;

	int count = 0;
	for (int i = 0; i < V_bunny.size(); i++) {
		//centerBunnyx = V_bunny[i].x + centerBunnyx;
		//centerBunnyy = V_bunny[i].y + centerBunnyy;
		//centerBunnyz = V_bunny[i].z + centerBunnyz;
		if (V_bunny[i].x <= xmin) {
			xmin = V_bunny[i].x;
		}
		else if (V_bunny[i].x > xmax) {
			xmax = V_bunny[i].x;
		}
		if (V_bunny[i].y <= ymin) {
			ymin = V_bunny[i].y;
		}
		else if (V_bunny[i].y > ymax) {
			ymax = V_bunny[i].y;
		}
		if (V_bunny[i].z <= zmin) {
			zmin = V_bunny[i].z;
		}
		else if (V_bunny[i].z > zmax) {
			zmax = V_bunny[i].z;
		}

	}
	cout << "count is " << vertexNum << endl;
	/*centerBunnyx = centerBunnyx / vertex_count;
	centerBunnyy = centerBunnyy / vertex_count;
	centerBunnyz = centerBunnyz / vertex_count;
	cout << centerBunnyx << endl;
	cout << centerBunnyy << endl;
	cout << centerBunnyz << endl;*/

	totalNumBunny++;
	currentBunnyMode.push_back(0);
	bunnyVBO.update(V_bunny);

	bunnyReflection.push_back(0.0);
	
	/*for (int i = 0; i < V_bunny.size(); i = i + 3) {
		glm::vec3 firstVec = V_bunny[i + 1] - V_bunny[i];
		glm::vec3 secondVec = V_bunny[i + 2] - V_bunny[i];
		normalVector[i].x = firstVec.y * secondVec.z - firstVec.z * secondVec.y;
		normalVector[i].y = firstVec.z * secondVec.x - firstVec.x * secondVec.z;
		normalVector[i].z = firstVec.x * secondVec.y - firstVec.y * secondVec.x;

		//float length = sqrt(normalVector[i].x * normalVector[i].x + normalVector[i].y * normalVector[i].y + normalVector[i].z * normalVector[i].z);

		//normalVector[i].x = normalVector[i].x / length;
		//normalVector[i].y = normalVector[i].y / length;
		//normalVector[i].z = normalVector[i].z / length;
	}

	VBO_normal.update(normalVector);
	*/
}


void bunnyNormal() {
	int i = 0;
	while (i < 3000*totalNumBunny) {
		//check each vertex in each triangle
		if (i % 3 == 0){
			//cout << "bunny 0 is" << endl;
			//cout << V_bunny[0].x << " " << V_bunny[0].y << endl;
			glm::vec3 firstVec = V_bunny[i + 1] - V_bunny[i];
			glm::vec3 secondVec = V_bunny[i + 2] - V_bunny[i];
			bunnyNormalVector[i].x = firstVec.y * secondVec.z - firstVec.z * secondVec.y;
			bunnyNormalVector[i].y = firstVec.z * secondVec.x - firstVec.x * secondVec.z;
			bunnyNormalVector[i].z = firstVec.x * secondVec.y - firstVec.y * secondVec.x;

			float length = sqrt(bunnyNormalVector[i].x * bunnyNormalVector[i].x + bunnyNormalVector[i].y * bunnyNormalVector[i].y + bunnyNormalVector[i].z * bunnyNormalVector[i].z);
			//cout << "length is" << length << endl;
			bunnyNormalVector[i].x = bunnyNormalVector[i].x / length;
			bunnyNormalVector[i].y = bunnyNormalVector[i].y / length;
			bunnyNormalVector[i].z = bunnyNormalVector[i].z / length;
			i = i + 1;
		}
		else if (i % 3 == 1) {
			glm::vec3 firstVec = V_bunny[i + 1] - V_bunny[i];
			glm::vec3 secondVec = V_bunny[i - 1] - V_bunny[i];
			bunnyNormalVector[i].x = firstVec.y * secondVec.z - firstVec.z * secondVec.y;
			bunnyNormalVector[i].y = firstVec.z * secondVec.x - firstVec.x * secondVec.z;
			bunnyNormalVector[i].z = firstVec.x * secondVec.y - firstVec.y * secondVec.x;

			float length = sqrt(bunnyNormalVector[i].x * bunnyNormalVector[i].x + bunnyNormalVector[i].y * bunnyNormalVector[i].y + bunnyNormalVector[i].z * bunnyNormalVector[i].z);
		    //cout << "length is" << length << endl;
			bunnyNormalVector[i].x = bunnyNormalVector[i].x / length;
			bunnyNormalVector[i].y = bunnyNormalVector[i].y / length;
			bunnyNormalVector[i].z = bunnyNormalVector[i].z / length;
			i = i + 1;
		}
		else if (i % 3 == 2) {
			glm::vec3 firstVec = V_bunny[i - 2] - V_bunny[i];
			glm::vec3 secondVec = V_bunny[i - 1] - V_bunny[i];
			bunnyNormalVector[i].x = firstVec.y * secondVec.z - firstVec.z * secondVec.y;
			bunnyNormalVector[i].y = firstVec.z * secondVec.x - firstVec.x * secondVec.z;
			bunnyNormalVector[i].z = firstVec.x * secondVec.y - firstVec.y * secondVec.x;

			float length = sqrt(bunnyNormalVector[i].x * bunnyNormalVector[i].x + bunnyNormalVector[i].y * bunnyNormalVector[i].y + bunnyNormalVector[i].z * bunnyNormalVector[i].z);
		    //cout << "length is" << length << endl;
			bunnyNormalVector[i].x = bunnyNormalVector[i].x / length;
			bunnyNormalVector[i].y = bunnyNormalVector[i].y / length;
			bunnyNormalVector[i].z = bunnyNormalVector[i].z / length;
			i = i + 1;
		}
		
	}
	vector<glm::vec3> N_new = calculateAverageNormal(faceBunnyVector, V_bunny, bunnyNormalVector, 0, totalNumBunny * 3000);
	for (int i = 0; i < totalNumBunny * 3000; i++) {
		bunnyNormalVector[i] = N_new[i];
		bunnyNormalVBO.update(bunnyNormalVector);
	}
	bunnyNormalVBO.update(bunnyNormalVector);
}

void addBumpyCube() {
	vector<glm::vec3> vertex;
	vector<glm::vec4> face;
	readFile("C:\\Users\\Fangyi Wang\\Desktop\\base\\Assignment_4\\data\\bumpy_cube.off", vertex, face);


	for (int i = 0; i < face.size(); i++) {
		glm::vec4 vec = face[i];
		V_bumpyCube[(3 * i) + 3 * 1000 * (totalNumBumpyCube)] = vertex[vec[1]];
		V_bumpyCube[(3 * i + 1) + 3 * 1000 * (totalNumBumpyCube)] = vertex[vec[2]];
		V_bumpyCube[(3 * i + 2) + 3 * 1000 * (totalNumBumpyCube)] = vertex[vec[3]];
		//cout << "Current vertex index used up to " << (3 * i + 2) + 36 * totalNumBumpyCube + 3 * 1000 * (totalNumBumpyCube + totalNumBumpyCube) << endl;
	}
	//cout << "read successfully" << endl;
	xmin = V_bumpyCube[0].x;
	ymin = V_bumpyCube[0].y;
	zmin = V_bumpyCube[0].z;

	xmax = V_bumpyCube[0].x;
	ymax = V_bumpyCube[0].y;
	zmax = V_bumpyCube[0].z;

	int count = 0;
	for (int i = 0; i < V_bumpyCube.size(); i++) {
		//centerBunnyx = V_bunny[i].x + centerBunnyx;
		//centerBunnyy = V_bunny[i].y + centerBunnyy;
		//centerBunnyz = V_bunny[i].z + centerBunnyz;
		if (V_bumpyCube[i].x <= xmin) {
			xmin = V_bumpyCube[i].x;
		}
		else if (V_bumpyCube[i].x > xmax) {
			xmax = V_bumpyCube[i].x;
		}
		if (V_bumpyCube[i].y <= ymin) {
			ymin = V_bumpyCube[i].y;
		}
		else if (V_bumpyCube[i].y > ymax) {
			ymax = V_bumpyCube[i].y;
		}
		if (V_bumpyCube[i].z <= zmin) {
			zmin = V_bumpyCube[i].z;
		}
		else if (V_bumpyCube[i].z > zmax) {
			zmax = V_bumpyCube[i].z;
		}

	}
	//cout << "count is " << vertexNum << endl;
	/*centerBunnyx = centerBunnyx / vertex_count;
	centerBunnyy = centerBunnyy / vertex_count;
	centerBunnyz = centerBunnyz / vertex_count;
	cout << centerBunnyx << endl;
	cout << centerBunnyy << endl;
	cout << centerBunnyz << endl;*/
	//for (int i = 3000 * totalNumBumpyCube; i < 3000; i++) {
		//C_bumpyCube[i] = glm::vec3(0.0f, totalNumBumpyCube / 255.0f, 0.0f);
	//}

	totalNumBumpyCube++;
	currentBumpyMode.push_back(0);
	bumpyCubeVBO.update(V_bumpyCube);
	bumpyReflection.push_back(0.0);

}

void bumpyCubeNormal() {
	int i = 0;
	while (i < 3000 * totalNumBumpyCube) {
		//check each vertex in each triangle
		if (i % 3 == 0) {
			glm::vec3 firstVec = V_bumpyCube[i + 1] - V_bumpyCube[i];
			glm::vec3 secondVec = V_bumpyCube[i + 2] - V_bumpyCube[i];
			bumpyNormalVector[i].x = firstVec.y * secondVec.z - firstVec.z * secondVec.y;
			bumpyNormalVector[i].y = firstVec.z * secondVec.x - firstVec.x * secondVec.z;
			bumpyNormalVector[i].z = firstVec.x * secondVec.y - firstVec.y * secondVec.x;

			float length = sqrt(bumpyNormalVector[i].x * bumpyNormalVector[i].x + bumpyNormalVector[i].y * bumpyNormalVector[i].y + bunnyNormalVector[i].z * bumpyNormalVector[i].z);
			//cout << "length is" << length << endl;
			bumpyNormalVector[i].x = bumpyNormalVector[i].x / length;
			bumpyNormalVector[i].y = bumpyNormalVector[i].y / length;
			bumpyNormalVector[i].z = bumpyNormalVector[i].z / length;
			i = i + 1;
		}
		else if (i % 3 == 1) {
			glm::vec3 firstVec = V_bumpyCube[i + 1] - V_bumpyCube[i];
			glm::vec3 secondVec = V_bumpyCube[i - 1] - V_bumpyCube[i];
			bumpyNormalVector[i].x = firstVec.y * secondVec.z - firstVec.z * secondVec.y;
			bumpyNormalVector[i].y = firstVec.z * secondVec.x - firstVec.x * secondVec.z;
			bumpyNormalVector[i].z = firstVec.x * secondVec.y - firstVec.y * secondVec.x;

			float length = sqrt(bumpyNormalVector[i].x * bumpyNormalVector[i].x + bumpyNormalVector[i].y * bumpyNormalVector[i].y + bunnyNormalVector[i].z * bumpyNormalVector[i].z);
			//cout << "length is" << length << endl;
			bumpyNormalVector[i].x = bumpyNormalVector[i].x / length;
			bumpyNormalVector[i].y = bumpyNormalVector[i].y / length;
			bumpyNormalVector[i].z = bumpyNormalVector[i].z / length;
			i = i + 1;
		}
		else if (i % 3 == 2) {
			glm::vec3 firstVec = V_bumpyCube[i - 2] - V_bumpyCube[i];
			glm::vec3 secondVec = V_bumpyCube[i - 1] - V_bumpyCube[i];
			bumpyNormalVector[i].x = firstVec.y * secondVec.z - firstVec.z * secondVec.y;
			bumpyNormalVector[i].y = firstVec.z * secondVec.x - firstVec.x * secondVec.z;
			bumpyNormalVector[i].z = firstVec.x * secondVec.y - firstVec.y * secondVec.x;

			float length = sqrt(bumpyNormalVector[i].x * bumpyNormalVector[i].x + bumpyNormalVector[i].y * bumpyNormalVector[i].y + bunnyNormalVector[i].z * bumpyNormalVector[i].z);
			//cout << "length is" << length << endl;
			bumpyNormalVector[i].x = bumpyNormalVector[i].x / length;
			bumpyNormalVector[i].y = bumpyNormalVector[i].y / length;
			bumpyNormalVector[i].z = bumpyNormalVector[i].z / length;
			i = i + 1;
		}

	}
	
	
	vector<glm::vec3> N_new = calculateAverageNormal(faceBumpyVector, V_bumpyCube, bumpyNormalVector, 0, totalNumBumpyCube*3000);
	for (int i = 0; i < totalNumBumpyCube * 3000; i++) {
		bumpyNormalVector[i] = N_new[i];
		bumpyNormalVBO.update(bumpyNormalVector);
	}
	bumpyNormalVBO.update(bumpyNormalVector);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		// Get the size of the window
		int width, height;
		glfwGetWindowSize(window, &width, &height);

		// Convert screen position to world coordinates
		double xworld = ((xpos / double(width)) * 2) - 1;
		double yworld = (((height - 1 - ypos) / double(height)) * 2) - 1;

		glm::vec4 newWorldCord = glm::inverse(view) * glm::vec4(xworld, yworld, 0.0, 1.0);
		xworld = newWorldCord.x;
		yworld = newWorldCord.y;
		//glFlush();
		//glFinish();

		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		float getId[4];
		glReadPixels(xpos, height - 1 - ypos, 1, 1, GL_STENCIL_INDEX, GL_FLOAT, &getId);


		category = ((int)getId[0]) / 10;
		whichObject= ((int)getId[0]) % 10;
		std::cout << "category and object is "<<category << " " << whichObject << endl;

		if (category == 0) {
			for (int i = whichObject*36; i < (whichObject + 1) * 36; i++) {
				C_cube[i] = glm::vec3( 0.69026, 0.18824, 0.37647);
			}
			cubeColorVBO.update(C_cube);
		}
		else if (category == 2) {
			for (int i = whichObject * 3000; i < (whichObject + 1) * 3000; i++) {
				C_bumpyCube[i] = glm::vec3(0.19608, 0.80392, 0.19608);
			}
			bumpyCubeColorVBO.update(C_bumpyCube);
		}
		else if (category == 4) {
			for (int i = whichObject * 3000; i < (whichObject + 1) * 3000; i++) {
				C_bunny[i] = glm::vec3(0.80392, 0.36078, 0.36078);
			}
			bunnyColorVBO.update(C_bunny);
		}



		/*
		float depth = 0.0;
		glReadPixels(xpos, height - 1 - ypos, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
		//depth range is 0 to 1, we need to change it to -1 and 1
		depth = 2 * depth - 1;
		depth = new_cord.z;
		cout << xworld << " " << yworld << " " << depth << endl;
		*/
	}
}





/*
void drawStuff(Program program, vector<glm::mat4x4> models, VertexBufferObject VBO, VertexBufferObject VBO_normal,VertexBufferObject VBO_color) {
	for (int i = 0; i < models.size(); i++) {
		glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, &models[i][0][0]);
		program.bindVertexAttribArray("normal", VBO_normal);
		program.bindVertexAttribArray("position", VBO);
		program.bindVertexAttribArray("color", VBO_color);
		glDrawArrays(GL_TRIANGLES, 0, 30000);
	}

}
*/

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		addNewCube();
		//cubeNormal();
		glm::mat4x4 whichCube = glm::mat4(1.f);
		modelCube.push_back(whichCube);
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		addBumpyCube();
		bumpyCubeNormal();
		float sizex, sizey, sizez = 0.0;
		float centerx, centery, centerz = 0.0;

		cout << xmax << " " << xmin;
		cout << ymax << " " << ymin;
		cout << zmax << " " << zmin;

		sizex = 1 / (xmax - xmin);
		sizey = 1 / (ymax - ymin);
		sizez = 1 / (zmax - zmin);

		cout << sizex << endl;
		cout << sizey << endl;
		cout << sizez << endl;

		centerx = (xmax + xmin) / (2 * (xmax - xmin));
		centery = (ymax + ymin) / (2 * (ymax - ymin));
		centerz = (zmax + zmin) / (2 * (zmax - zmin));

		glm::mat4x4 scaleMatrix = { {sizex,0,0,0},{0,sizey,0,0},{0,0,sizez,0},{0,0,0,1} };
		glm::mat4x4 transMatrix = { {1.0,0,0,0},{0,1.0,0,0},{0,0,1.0,0},{-centerx,-centery,-centerz,1.0} };

		MVPBumpyCubeMatrix = transMatrix * scaleMatrix;

		modelBumpyCube.push_back(MVPBumpyCubeMatrix);
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		addNewBunny();
		bunnyNormal();
		float sizex, sizey, sizez = 0.0;
		float centerx, centery, centerz = 0.0;

		cout << xmax << " " << xmin;
		cout << ymax << " " << ymin;
		cout << zmax << " " << zmin;

		sizex = 1 / (xmax - xmin);
		sizey = 1 / (ymax - ymin);
		sizez = 1 / (zmax - zmin);

		cout << sizex << endl;
		cout << sizey << endl;
		cout << sizez << endl;

		centerx = (xmax + xmin) / (2 * (xmax - xmin));
		centery = (ymax + ymin) / (2 * (ymax - ymin));
		centerz = (zmax + zmin) / (2 * (zmax - zmin));

		glm::mat4x4 scaleMatrix = { {sizex,0,0,0},{0,sizey,0,0},{0,0,sizez,0},{0,0,0,1} };
		glm::mat4x4 transMatrix = { {1.0,0,0,0},{0,1.0,0,0},{0,0,1.0,0},{-centerx,-centery,-centerz,1.0} };

		MVPBunnyMatrix = transMatrix * scaleMatrix;

		modelBunny.push_back(MVPBunnyMatrix);
	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
		//cube
		if (category == 0) {
			cubeReflection[whichObject] = 1.0;
		}
		//bumpy
		if (category == 2) {
			bumpyReflection[whichObject] = 1.0;
		}
		//bunny
		if (category == 4) {
			bunnyReflection[whichObject] = 1.0;
		}

	}
	if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
		//cube
		if (category == 0) {
			cubeReflection[whichObject] = 0.0;
		}
		//bumpy
		if (category == 2) {
			bumpyReflection[whichObject] = 0.0;
		}
		//bunny
		if (category == 4) {
			bunnyReflection[whichObject] = 0.0;
		}

	}
	if (key == GLFW_KEY_6 && action == GLFW_PRESS) {
		if (category == 0) {
			cubeReflection[whichObject] = 2.0;
		}
		//bumpy
		if (category == 2) {
			bumpyReflection[whichObject] = 2.0;
		}
		//bunny
		if (category == 4) {
			bunnyReflection[whichObject] = 2.0;
		}
	}
	
	//keys to control perspective camera
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		if (PEnable == true) {
			cameraPosP.y = cameraPosP.y + 0.1f;
		}
		
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		if (PEnable == true) {
			cameraPosP.y = cameraPosP.y - 0.1f;
		}
		
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		if (PEnable == true) { 
			cameraPosP.x = cameraPosP.x - 0.1f; 
		}
		
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		if (PEnable == true) {
			cameraPosP.x = cameraPosP.x + 0.1f;
		}
	}
	if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		if (PEnable == true) {
			cameraPosP.z = cameraPosP.z - 0.1f;
		}
		
	}
	if (key == GLFW_KEY_E && action == GLFW_PRESS) {
		if (PEnable == true) {
			cameraPosP.z = cameraPosP.z + 0.1f;
		}
	}
	
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		if (category == 0) {
			for (int i = whichObject*36; i < 36*whichObject+36; i++) {
				V_cube[i] = glm::vec3(0,0,0);
			}
			cubeVBO.update(V_cube);
		}
		if (category == 2) {
			for (int i = whichObject * 3000; i < 3000 * whichObject + 3000; i++) {
				V_bumpyCube[i] = glm::vec3(0, 0, 0);
			}
			bumpyCubeVBO.update(V_bumpyCube);
		}
		if (category == 4) {
			for (int i = whichObject * 3000; i < 3000 * whichObject + 3000; i++) {
				V_bunny[i] = glm::vec3(0, 0, 0);
			}
			bunnyVBO.update(V_bunny);
		}
	}
	//control shadow
	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		shadowModeControl = 1.0;
	}
	if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
		shadowModeControl = 0.0;
	}
	//translate left
	if (key == GLFW_KEY_J && action == GLFW_PRESS) {
		if (category == 0) {
			modelCube[whichObject] = glm::translate(glm::mat4(1.0f), glm::vec3(-0.2f, 0.0f, 0.0f)) * modelCube[whichObject];
		}
		else if (category==2) {
			modelBumpyCube[whichObject] = glm::translate(glm::mat4(1.0f), glm::vec3(-0.2f, 0.0f, 0.0f)) * modelBumpyCube[whichObject];
		}
		else if (category == 4) {
			modelBunny[whichObject] = glm::translate(glm::mat4(1.0f), glm::vec3(-0.2f, 0.0f, 0.0f)) * modelBunny[whichObject];
		}
	}
	//translate right
	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		if (category == 0) {
			modelCube[whichObject] = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.0f, 0.0f)) * modelCube[whichObject];
		}
		else if (category == 2) {
			modelBumpyCube[whichObject] = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.0f, 0.0f)) * modelBumpyCube[whichObject];
		}
		else if (category == 4) {
			modelBunny[whichObject] = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.0f, 0.0f)) * modelBunny[whichObject];
		}
	}
	//translate back
	if (key == GLFW_KEY_U && action == GLFW_PRESS) {
		if (category == 0) {
			modelCube[whichObject] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.2f)) * modelCube[whichObject];
		}
		else if (category == 2) {
			modelBumpyCube[whichObject] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.2f)) * modelBumpyCube[whichObject];
		}
		else if (category == 4) {
			modelBunny[whichObject] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.2f)) * modelBunny[whichObject];
		}
	}
	//translate front
	if (key == GLFW_KEY_O && action == GLFW_PRESS) {
		if (category == 0) {
			modelCube[whichObject] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.2f)) * modelCube[whichObject];
		}
		else if (category == 2) {
			modelBumpyCube[whichObject] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.2f)) * modelBumpyCube[whichObject];
		}
		else if (category == 4) {
			modelBunny[whichObject] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.2f)) * modelBunny[whichObject];
		}
	}
	//translate up
	if (key == GLFW_KEY_I && action == GLFW_PRESS) {
		if (category == 0) {
			modelCube[whichObject] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.2f, 0.0f)) * modelCube[whichObject];
		}
		else if (category == 2) {
			modelBumpyCube[whichObject] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.2f, 0.0f)) * modelBumpyCube[whichObject];
		}
		else if (category == 4) {
			modelBunny[whichObject] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.2f, 0.0f)) * modelBunny[whichObject];
		}
	}
	//translate down
	if (key == GLFW_KEY_K && action == GLFW_PRESS) {
		if (category == 0) {
			modelCube[whichObject] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.2f, 0.0f)) * modelCube[whichObject];
		}
		else if (category == 2) {
			modelBumpyCube[whichObject] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.2f, 0.0f)) * modelBumpyCube[whichObject];
		}
		else if (category == 4) {
			modelBunny[whichObject] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.2f, 0.0f)) * modelBunny[whichObject];
		}
	}
	//scale up
	if (key == GLFW_KEY_7 && action == GLFW_PRESS) {
		if (category == 0) {
			modelCube[whichObject] = glm::scale(glm::mat4(1.0f), glm::vec3(1.2f, 1.2f, 1.2f)) * modelCube[whichObject];
		}
		else if (category == 2) {
			modelBumpyCube[whichObject] = glm::scale(glm::mat4(1.0f), glm::vec3(1.2f, 1.2f, 1.2f)) * modelBumpyCube[whichObject];
		}
		else if (category == 4) {
			modelBunny[whichObject] = glm::scale(glm::mat4(1.0f), glm::vec3(1.2f, 1.2f, 1.2f)) * modelBunny[whichObject];
		}
	}

	//Scale down
	if (key == GLFW_KEY_8 && action == GLFW_PRESS) {
		if (category == 0) {
			modelCube[whichObject] = glm::scale(glm::mat4(1.0f), glm::vec3(0.7f, 0.7, 0.7f)) * modelCube[whichObject];
		}
		else if (category == 2) {
			modelBumpyCube[whichObject] = glm::scale(glm::mat4(1.0f), glm::vec3(0.7f, 0.7, 0.7f)) * modelBumpyCube[whichObject];
		}
		else if (category == 4) {
			modelBunny[whichObject] = glm::scale(glm::mat4(1.0f), glm::vec3(0.7f, 0.7, 0.7f)) * modelBunny[whichObject];
		}
	}
	//rotate clockwise
	if (key == GLFW_KEY_9 && action == GLFW_PRESS) {
		if (category == 0) {
			glm::vec4 total = glm::vec4(0.f, 0.f, 0.f, 0.f);
			for (int i = 36 * whichObject; i < 36 * (whichObject + 1); i++) {
				total += modelCube[whichObject] * glm::vec4(V_cube[i], 1.0f);
			}
			glm::vec3 barycenter = glm::vec3(total[0] / total[3], total[1] / total[3], total[2] / total[3]);
			modelCube[whichObject] = glm::translate(glm::mat4(1), barycenter) * glm::rotate(glm::mat4(1), glm::radians(20.f), glm::vec3(1.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1), -barycenter) * modelCube[whichObject];
		}
		else if (category == 2) {
			glm::vec4 total = glm::vec4(0.f, 0.f, 0.f, 0.f);
			for (int i = 3000 * whichObject; i < 3000 * (whichObject + 1); i++) {
				total += modelBumpyCube[whichObject] * glm::vec4(V_bumpyCube[i], 1.0f);
			}
			glm::vec3 barycenter = glm::vec3(total[0] / total[3], total[1] / total[3], total[2] / total[3]);
			modelBumpyCube[whichObject] = glm::translate(glm::mat4(1), barycenter) * glm::rotate(glm::mat4(1), glm::radians(20.f), glm::vec3(1.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1), -barycenter) * modelBumpyCube[whichObject];
		}
		else if (category == 4) {
			glm::vec4 total = glm::vec4(0.f, 0.f, 0.f, 0.f);
			for (int i = 3000 * whichObject; i < 3000 * (whichObject + 1); i++) {
				total += modelBunny[whichObject] * glm::vec4(V_bunny[i], 1.0f);
			}
			glm::vec3 barycenter = glm::vec3(total[0] / total[3], total[1] / total[3], total[2] / total[3]);
			modelBunny[whichObject] = glm::translate(glm::mat4(1), barycenter) * glm::rotate(glm::mat4(1), glm::radians(20.f), glm::vec3(1.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1), -barycenter) * modelBunny[whichObject];
		}
	}
	//rotate other way
	if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
		if (category == 0) {
			glm::vec4 total = glm::vec4(0.f, 0.f, 0.f, 0.f);
			for (int i = 36 * whichObject; i < 36 * (whichObject + 1); i++) {
				total += modelCube[whichObject] * glm::vec4(V_cube[i], 1.0f);
			}
			glm::vec3 barycenter = glm::vec3(total[0] / total[3], total[1] / total[3], total[2] / total[3]);
			modelCube[whichObject] = glm::translate(glm::mat4(1), barycenter) * glm::rotate(glm::mat4(1), -glm::radians(20.f), glm::vec3(1.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1), -barycenter) * modelCube[whichObject];
		}
		else if (category == 2) {
			glm::vec4 total = glm::vec4(0.f, 0.f, 0.f, 0.f);
			for (int i = 3000 * whichObject; i < 3000 * (whichObject + 1); i++) {
				total += modelBumpyCube[whichObject] * glm::vec4(V_bumpyCube[i], 1.0f);
			}
			glm::vec3 barycenter = glm::vec3(total[0] / total[3], total[1] / total[3], total[2] / total[3]);
			modelBumpyCube[whichObject] = glm::translate(glm::mat4(1), barycenter) * glm::rotate(glm::mat4(1), -glm::radians(20.f), glm::vec3(1.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1), -barycenter) * modelBumpyCube[whichObject];
		}
		else if (category == 4) {
			glm::vec4 total = glm::vec4(0.f, 0.f, 0.f, 0.f);
			for (int i = 3000 * whichObject; i < 3000 * (whichObject + 1); i++) {
				total += modelBunny[whichObject] * glm::vec4(V_bunny[i], 1.0f);
			}
			glm::vec3 barycenter = glm::vec3(total[0] / total[3], total[1] / total[3], total[2] / total[3]);
			modelBunny[whichObject] = glm::translate(glm::mat4(1), barycenter) * glm::rotate(glm::mat4(1), -glm::radians(20.f), glm::vec3(1.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1), -barycenter) * modelBunny[whichObject];
		}
	}
	
	/*
	//wirteframe
	if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
		//wireframe
		if (category == 0) {
			currentCubeMode[whichObject] = 1;
		}
		else if (category == 2) {
			currentBumpyMode[whichObject] = 1;
		}
		else if (category == 4) {
			currentBunnyMode[whichObject] = 1;
		}
		
	}

	//flat shading 
	if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		
		if (category == 0) {
			currentCubeMode[whichObject] = 2;
		}
		else if (category == 2) {
			currentBumpyMode[whichObject] = 2;
		}
		else if (category == 4) {
			currentBunnyMode[whichObject] = 2;
		}

	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		cout << "C IS PRESSED" << endl;
		
		if (category == 0) {
			currentCubeMode[whichObject] = 3;
		}

		if (category == 2) {
			currentBumpyMode[whichObject] = 3;
			vector<glm::vec3> N_new = calculateAverageNormal(faceBumpyVector,V_bumpyCube, bumpyNormalVector, whichObject * 3000, (whichObject + 1) * 3000);
			for (int i = whichObject * 3000; i < (whichObject + 1) * 3000; i++) {
				bumpyNormalVector[i] = N_new[i];
				bumpyNormalVBO.update(bumpyNormalVector);
			}
		}

		if (category == 4) {
			currentBunnyMode[whichObject] = 3;
			vector<glm::vec3>N_new =calculateAverageNormal(faceBunnyVector,V_bunny, bunnyNormalVector, whichObject*3000, (whichObject+1)*3000);
			for (int i = whichObject * 3000; i < (whichObject + 1) * 3000; i++) {
				bunnyNormalVector[i] = N_new[i];
			bunnyNormalVBO.update(bunnyNormalVector);
		}
		}

	}*/

	

}

int main(void)
{
	GLFWwindow* window;

	// Initialize the library
	if (!glfwInit())
		return -1;

	// Activate supersampling
	glfwWindowHint(GLFW_SAMPLES, 8);

	// Ensure that we get at least a 3.2 context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// On apple we have to load a core profile with forward compatibility
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);



#ifndef __APPLE__
	glewExperimental = true;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	int major, minor, rev;
	major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
	minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
	rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
	printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
	printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
	printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

	VertexArrayObject VAO;
	VAO.init();
	VAO.bind();

	cubeVBO.init();
	cubeColorVBO.init();
	C_cube.resize(2000);

	bunnyVBO.init();
	bunnyColorVBO.init();
	C_bunny.resize(30000);

	bumpyCubeVBO.init();
	bumpyCubeColorVBO.init();
	C_bumpyCube.resize(30000);
	
	//normal init
	cubeNormalVBO.init();
	cubeNormalVector.resize(2000);
	cubeNormalVBO.update(cubeNormalVector);

	bunnyNormalVBO.init();
	bunnyNormalVector.resize(30000);
	bunnyNormalVBO.update(bunnyNormalVector);

	bumpyNormalVBO.init();
	bumpyNormalVector.resize(30000);
	bumpyNormalVBO.update(bumpyNormalVector);

	cubeTexCoordVBO.init();
	cubeTexCoord.resize(30000);
	cubeTexCoordVBO.update(cubeTexCoord);


	for (int i = 0; i < C_cube.size(); i++) {
		C_cube[i] = glm::vec3(1.f, 0.84314f, 0.f);
	}
	for (int i = 0; i < V_cube.size(); i++) {
		V_cube[i] = glm::vec3(0, 0, 0);
	}
	cubeVBO.update(V_cube);
	cubeColorVBO.update(C_cube);

	for (int i = 0; i < V_bunny.size(); i++) {
		V_bunny[i] = glm::vec3(0, 0, 0);
	}
	for (int i = 0; i < 30000; i++) {
		C_bunny[i] = glm::vec3(1.f, 0.84314f, 0.f);
	}
	bunnyVBO.update(V_bunny);
	bunnyColorVBO.update(C_bunny);


	for (int i = 0; i < V_bumpyCube.size(); i++) {
		V_bumpyCube[i] = glm::vec3(0, 0, 0);
	}
	for (int i = 0; i < 30000; i++) {
		C_bumpyCube[i] = glm::vec3(1.f, 0.84314f, 0.f);
	}
	bumpyCubeVBO.update(V_bumpyCube);
	bumpyCubeColorVBO.update(C_bumpyCube);

	planeVBO.init();
	V_plane[0] = glm::vec3(3.0f, -0.5f, 4.0f);
	V_plane[1] = glm::vec3(-3.0f, -0.5f, 4.0f);
	V_plane[2] = glm::vec3(-3.0f, -0.5f, -4.0f);

	V_plane[3] = glm::vec3(3.0f, -0.5f, 4.0f);
	V_plane[4] = glm::vec3(-3.0f, -0.5f, -4.0f);
	V_plane[5] = glm::vec3(3.0f, -0.5f, -4.0f);
	planeVBO.update(V_plane);

	planeColorVBO.init();
	C_plane.resize(100);
	for (int i = 0; i < 6; i++) {
		//C_plane[i] = glm::vec3(0.282f,0.239f,0.545f);
		C_plane[i] = glm::vec3(0.867, 0.627, 0.867);
	}
	planeColorVBO.update(C_plane);

	planeNormalVBO.init();
	planeNormal[0] = glm::vec3(0.0f, 1.0f, 0.0f);
	planeNormal[1] = glm::vec3(0.0f, 1.0f, 0.0f);
	planeNormal[2] = glm::vec3(0.0f, 1.0f, 0.0f);
	planeNormal[3] = glm::vec3(0.0f, 1.0f, 0.0f);
	planeNormal[4] = glm::vec3(0.0f, 1.0f, 0.0f);
	planeNormal[5] = glm::vec3(0.0f, 1.0f, 0.0f);
	planeNormalVBO.update(planeNormal);


	planeTexCoordVBO.init();
	planeTexCoord[0] = glm::vec2(1.0f, 0.0f);
	planeTexCoord[1] = glm::vec2(0.0f, 0.0f);
	planeTexCoord[2] = glm::vec2(0.0f, 1.0f);
	planeTexCoord[3] = glm::vec2(1.0f, 0.0f);
	planeTexCoord[4] = glm::vec2(0.0f, 1.0f);
	planeTexCoord[5] = glm::vec2(1.0f, 1.0f);
	planeTexCoordVBO.update(planeTexCoord);

	skyBoxVBO.init();
	skyBox[0] = glm::vec3(-1.0f, 1.0f, -1.0f);
	skyBox[1] = glm::vec3(-1.0f, -1.0f, -1.0f);
	skyBox[2] = glm::vec3(1.0f, -1.0f, -1.0f);
	skyBox[3] = glm::vec3(1.0f, -1.0f, -1.0f);
	skyBox[4] = glm::vec3(1.0f, 1.0f, -1.0f);
	skyBox[5] = glm::vec3(-1.0f, 1.0f, -1.0f);

	skyBox[6] = glm::vec3(-1.0f, -1.0f, 1.0f);
	skyBox[7] = glm::vec3(-1.0f, -1.0f, -1.0f);
	skyBox[8] = glm::vec3(-1.0f, 1.0f, -1.0f);
	skyBox[9] = glm::vec3(-1.0f, 1.0f, -1.0f);
	skyBox[10] = glm::vec3(-1.0f, 1.0f, 1.0f);
	skyBox[11] = glm::vec3(-1.0f, -1.0f, 1.0f);

	skyBox[12] = glm::vec3(1.0f, -1.0f, -1.0f);
	skyBox[13] = glm::vec3(1.0f, -1.0f, 1.0f);
	skyBox[14] = glm::vec3(1.0f, 1.0f, 1.0f);
	skyBox[15] = glm::vec3(1.0f, 1.0f, 1.0f);
	skyBox[16] = glm::vec3(1.0f, 1.0f, -1.0f);
	skyBox[17] = glm::vec3(1.0f, -1.0f, -1.0f);

	skyBox[18] = glm::vec3(-1.0f, -1.0f, 1.0f);
	skyBox[19] = glm::vec3(-1.0f, 1.0f, 1.0f);
	skyBox[20] = glm::vec3(1.0f, 1.0f, 1.0f);
	skyBox[21] = glm::vec3(1.0f, 1.0f, 1.0f);
	skyBox[22] = glm::vec3(1.0f, -1.0f, 1.0f);
	skyBox[23] = glm::vec3(-1.0f, -1.0f, 1.0f);

	skyBox[24] = glm::vec3(-1.0f, 1.0f, -1.0f);
	skyBox[25] = glm::vec3(1.0f, 1.0f, -1.0f);
	skyBox[26] = glm::vec3(1.0f, 1.0f, 1.0f);
	skyBox[27] = glm::vec3(1.0f, 1.0f, 1.0f);
	skyBox[28] = glm::vec3(-1.0f, 1.0f, 1.0f);
	skyBox[29] = glm::vec3(-1.0f, 1.0f, -1.0f);

	skyBox[30] = glm::vec3(-1.0f, -1.0f, -1.0f);
	skyBox[31] = glm::vec3(-1.0f, -1.0f, 1.0f);
	skyBox[32] = glm::vec3(1.0f, -1.0f, -1.0f);
	skyBox[33] = glm::vec3(1.0f, -1.0f, -1.0f);
	skyBox[34] = glm::vec3(-1.0f, -1.0f, 1.0f);
	skyBox[35] = glm::vec3(1.0f, -1.0f, 1.0f);
	skyBoxVBO.update(skyBox);

	faceSky[0] = "C:\\Users\\Fangyi Wang\\Desktop\\base\\Assignment_4\\data\\night_posx.png";
	faceSky[1] = "C:\\Users\\Fangyi Wang\\Desktop\\base\\Assignment_4\\data\\night_negx.png";
	faceSky[2] = "C:\\Users\\Fangyi Wang\\Desktop\\base\\Assignment_4\\data\\night_posy.png";
	faceSky[3] = "C:\\Users\\Fangyi Wang\\Desktop\\base\\Assignment_4\\data\\night_negy.png";
	faceSky[4] = "C:\\Users\\Fangyi Wang\\Desktop\\base\\Assignment_4\\data\\night_posz.png";
	faceSky[5] = "C:\\Users\\Fangyi Wang\\Desktop\\base\\Assignment_4\\data\\night_negz.png";

	
	unsigned int cubemapTexture = loadCubemap(faceSky);
	
	/*
	float planeVertices[] = {
		// positions            // normals         // texcoords
		 1.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
		-1.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-1.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,

		 1.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
		-1.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
		 1.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f
	};
	// plane VAO
	unsigned int planeVAO;
	unsigned int planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	
	glBindVertexArray(0);
	*/
	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	

	Program program;
	const GLchar* vertex_shader =
		"#version 150 core\n"
		"in vec3 position;"
		"in vec3 normal;"
		"in vec2 texcoords;"
		"in vec3 color;"
		"uniform mat4 model;"
		"uniform mat4 projection;"
		"uniform mat4 view;"
		"uniform mat4 lightSpaceMatrix;"
		"out vec3 f_color;"
		"out vec3 Normal;"
		"out vec2 TexCoords;"
		"out vec3 FragPos;"
		"out vec4 FragPosLightSpace;"
		"out vec3 Position;"
		"void main()"
		"{"
		"    Position=vec3(model*vec4(position,1.0));"
		"    gl_Position = projection*view*model*vec4(position, 1.0);"
		"    FragPos=vec3(model*vec4(position,1.0));"
		"    Normal=mat3(transpose(inverse(model)))*normal;"
		"    TexCoords=texcoords;"
		"    FragPosLightSpace=lightSpaceMatrix*vec4(FragPos,1.0);"
		"    f_color = color;"
		"}";
	const GLchar* fragment_shader =
		//code learned from openGL website
		"#version 150 core\n"
		//color of the object
		"in vec3 f_color;"
		"in vec3 FragPos;"
		"in vec3 Normal;"
		"in vec3 Position;"
		"in vec2 TexCoords;"
		"in vec4 FragPosLightSpace;"

		"uniform vec3 lightPos;"
		"uniform vec3 viewPos;"
		"uniform sampler2D diffuseTexture;"
		"uniform sampler2D shadowMap;"
		"uniform float shadowMode;"
		"uniform vec3 lightColor;"
		"uniform samplerCube skybox;"
		"uniform float drawPlane;"
		"uniform float mirror;"
		//"uniform float transparent;"
		"out vec4 outColor;"
		//"uniform vec3 triangleColor;"
		"float ShadowCalculation(vec4 fragPosLightSpace)"
	    "{"
		// perform perspective divide
			"vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;"
		// transform to [0,1] range
			"projCoords = projCoords * 0.5 + 0.5;"
		// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
			"float closestDepth = texture(shadowMap, projCoords.xy).r;"
		    //"if(projCoords.z>1.0){projCoords.z=1.0;};"
		// get depth of current fragment from light's perspective
			"float currentDepth = projCoords.z;"
		// calculate bias (based on depth map resolution and slope)
		    "vec3 normal = normalize(Normal);"
		    "vec3 lightDir = normalize(lightPos - FragPos);"
	        "float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);"
		// check whether current frag pos is in shadow
			//"float shadow = currentDepth > closestDepth ? 1.0 : 0.0;"
			//"return shadow;"
		    "float shadow=0.0;"
		    "vec2 texelSize=1.0/textureSize(shadowMap,0);"
		    "for(int x=-1; x<=1; ++x)"
				"{"
					"for(int y = -1; y <= 1; ++y)"
						"{"
							"float pcfDepth=texture(shadowMap,projCoords.xy+vec2(x,y)*texelSize).r;"
							"shadow+=currentDepth-bias>pcfDepth ? 1.0:0.0;"
						"}"
				"}"
			"shadow/=9.0;"
		    "if(projCoords.z>1.0){shadow=0.0;};"
		    "return shadow;"
		"}"

		"void main()"
		"{"
		"    vec3 color = texture(diffuseTexture, TexCoords).rgb;"
		"    vec3 norm=normalize(Normal);"
		//"vec3 lightColor = vec3(0.3);"
		//ambient
		"    float ambientStrength=0.3f;"
		"    vec3 ambient=ambientStrength*lightColor;"
		//diffuse
		"    vec3 lightDirection=normalize(lightPos-FragPos);"
		"    float diff=max(dot(lightDirection,norm),0.0);"
		"    vec3 diffuse=diff*lightColor;"
		//specular
		"    vec3 viewDirection=normalize(viewPos-FragPos);"
		"    vec3 reflectDirection=reflect(-lightDirection,norm);"
		"    float spec=0.0;"
		"    vec3 halfwayDir=normalize(lightDirection+viewDirection);"
		"    spec=pow(max(dot(norm,halfwayDir),0.0),64);"
		"    vec3 specular=spec*lightColor;"
		//"    if(diff>0){lightSide=1.0;};"
		//"    vec3 specular=specularStrength*spec*lightColor;"
		"    float shadow=ShadowCalculation(FragPosLightSpace);"
		"    vec3 result=(ambient+(1-shadow)*(diffuse+specular))*f_color;"
		//"    if(isOffScreen==0.0f){ outColor = vec4(result,1.0);};"
		//"    if(shadowMode==0)"
	        //"{"
			//"   outColor=vec4(result,1.0);"
		    //"}"
		//"    if(shadowMode==1.0)"
		    //"{"
		   // "	if(shadow==1){outColor=vec4(1.0,0,0,1.0);}"
		    //"   if(shadow!=1){ outColor = vec4(result,1.0);};"
		    //"}"
		
		"   if(drawPlane==1.0)"
			"{"
			"    if(shadowMode==0.0)"
				"{"
				"   outColor=vec4(result,1.0);"
				"}"
			"    if(shadowMode==1.0)"
				"{"
					"	if(shadow==1){outColor=vec4(1.0,0,0,1.0);}"
					"   if(shadow!=1){ outColor = vec4(result,1.0);};"
				"}"
			"}"
			
		"   if(drawPlane==0.0)"
			"{"
			"   if(mirror==1.0)"
				"{"
				"   vec3 I=normalize(Position-viewPos);"
				"   vec3 R=reflect(I,normalize(Normal));"
				"   outColor=vec4(texture(skybox,R).rgb,1.0);"
			    "}"
			"   if(mirror==0.0)"
				"{"
					"if(shadowMode==0.0)"
						"{"
							"outColor=vec4(result,1.0);"
						"}"
					"if(shadowMode==1.0)"
						"{"
							"if(shadow==1){outColor=vec4(1.0,0,0,1.0);}"
							"if(shadow!=1){ outColor = vec4(result,1.0);};"
						"}"
				"}"
			"if (mirror == 2.0)"
				"{"
				"   float ratio=1.00;"
				"   vec3 I=normalize(Position-viewPos);"
				"   vec3 R=refract(I,normalize(Normal),ratio);"
				"   outColor=vec4(texture(skybox,R).rgb,1.0);"
				"}"
			"}"
		//"    outColor = vec4(result,1.0);"
		"}";



	program.init(vertex_shader, fragment_shader, "outColor");
	program.bind();
	//program.bindVertexAttribArray("position", VBO_bunny);
	//program.bindVertexAttribArray("position", VBO_cube);
	program.bindVertexAttribArray("color", bunnyColorVBO);
	program.bindVertexAttribArray("color", cubeColorVBO);
	//program.bindVertexAttribArray("normal", VBO_normal);


	


	auto t_start = std::chrono::high_resolution_clock::now();

	glfwSetKeyCallback(window, key_callback);

	glfwSetMouseButtonCallback(window, mouse_button_callback);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//glfwSetCursorPosCallback(window, cursor_position_callback);

	glUniform1i(program.uniform("diffuseTexture"), 0);
	glUniform1i(program.uniform("shadowMap"), 1);

	Program program2;
	const GLchar* vertex_shader_depth =
		"#version 150 core\n"
		"in vec3 position;"
		
		"uniform mat4 model;"
		"uniform mat4 lightSpaceMatrix;"
		
		"void main()"
		"{"
		"    gl_Position = lightSpaceMatrix*model*vec4(position, 1.0);"
		"}";
	const GLchar* fragment_shader_depth =
		//code learned from openGL website
		"#version 150 core\n"
		"out vec4 outColor2;"
		"void main()"
		"{"
		// gl_FragDepth = gl_FragCoord.z;
		"}";

	program2.init(vertex_shader_depth, fragment_shader_depth, "outColor2");
	//program2.bind();
	
	Program program3;
	const GLchar* vertex_shader_skyBox =
		"#version 150 core\n"
		"in vec3 position;"
		"out vec3 TexCoords;"
		"uniform mat4 projection;"
		"uniform mat4 view;"

		"void main()"
		"{"
		"     TexCoords = position;"
		"     vec4 pos = projection * view * vec4(position, 1.0);"
	    "     gl_Position = pos.xyww;"
		"}";
	const GLchar* fragment_shader_skyBox =
		//code learned from openGL website
		"#version 150 core\n"
		"out vec4 outColor3;"
		"in vec3 TexCoords;"
		"uniform samplerCube skybox;"
		"void main()"
		"{"
		"	outColor3 = texture(skybox,TexCoords);"
		"}";

	program3.init(vertex_shader_skyBox, fragment_shader_skyBox, "outColor3");
	//program3.bind();
	

	while (!glfwWindowShouldClose(window))
	{
		glUniform1f(program.uniform("shadowMode"), shadowModeControl);
		glUniform3f(program.uniform("lightColor"), 1.0f, 1.0f, 1.0f);

		glm::vec3 lightPos = glm::vec3(-2.f*cos(float(glfwGetTime()))+1.0, 6.0f, -2.0f*sin(float(glfwGetTime())));
		//auto t_now = std::chrono::high_resolution_clock::now();
		//float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
		//view = glm::rotate(glm::mat4(1.f), 0 * (float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 0.0f));

		glUniform3f(program.uniform("lightPos"), lightPos.x, lightPos.y, lightPos.z);
		
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 4.0f, far_plane = 17.5f;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		VAO.bind();
		program2.bind();

		glUniformMatrix4fv(program2.uniform("lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);//woodTexture=0
			//draw cube
			for (int i = 0; i < modelCube.size(); i++) {
				glUniformMatrix4fv(program2.uniform("model"), 1, GL_FALSE, &modelCube[i][0][0]);
				program2.bindVertexAttribArray("position", cubeVBO);
				if (currentCubeMode[i] == 0) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glDrawArrays(GL_TRIANGLES, 36 * i, 36);
					
				}
			}
			//draw bumpycube
			for (int i = 0; i < modelBumpyCube.size(); i++) {
				glUniformMatrix4fv(program2.uniform("model"), 1, GL_FALSE, &modelBumpyCube[i][0][0]);
				program2.bindVertexAttribArray("position", bumpyCubeVBO);
				if (currentBumpyMode[i] == 0) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glDrawArrays(GL_TRIANGLES, i * 3000, 3000);
				}
			}
			//draw bunny
			for (int i = 0; i < modelBunny.size(); i++) {
				glUniformMatrix4fv(program2.uniform("model"), 1, GL_FALSE, &modelBunny[i][0][0]);
				program2.bindVertexAttribArray("position", bunnyVBO);
				if (currentBunnyMode[i] == 0) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glDrawArrays(GL_TRIANGLES, i * 3000, 3000);
				}
			}
			drawPlaneMode = 1.0;
			//glUniform1f(program.uniform("drawPlane"), drawPlaneMode);
			//draw floor
			glm::mat4 floorModel = glm::mat4(1.0f);
			glUniformMatrix4fv(program2.uniform("model"), 1, GL_FALSE, &floorModel[0][0]);
			program2.bindVertexAttribArray("position", planeVBO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			//glBindVertexArray(0);
			drawPlaneMode = 0.0;
			//glUniform1f(program.uniform("drawPlane"), drawPlaneMode);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		int height = 0;
		int width = 0;
		glfwGetWindowSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 2. render scene as normal using the generated depth/shadow map  
		// --------------------------------------------------------------
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//VAO.bind();
		program.bind();
		//glUniform1i(program.uniform("diffuseTexture"), 0);
		//glUniform1i(program.uniform("shadowMap"), 1);
		//glUniform3f(program.uniform("viewPos"), cameraPosP.x, cameraPosP.y, cameraPosP.z);
		//view = glm::lookAt(glm::vec3(cameraPosP.x, cameraPosP.y, cameraPosP.z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform1i(program.uniform("skybox"), 0);
		
		//place of camera
		
		if (PEnable == true) {
			
			glUniform3f(program.uniform("viewPos"), cameraPosP.x, cameraPosP.y, cameraPosP.z);
			//glUniform3f(program.uniform("viewPos"), 0, 0, 3.0);
			//view = glm::rotate(glm::mat4(1.f), 1 * (float)glfwGetTime() / 3, glm::vec3(1.0f, 1.0f, 0.0f));
			view = glm::lookAt(glm::vec3(cameraPosP.x, cameraPosP.y, cameraPosP.z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(program.uniform("projection"), 1, GL_FALSE, &projection[0][0]);
			glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, &view[0][0]);
			glUniformMatrix4fv(program.uniform("lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);
			


			int height = 0;
			int width = 0;
			glfwGetWindowSize(window, &width, &height);
			//projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 1.5f, 100.f);
			projection = glm::perspective(glm::radians(73.f), (float)width / (float)height, 1.5f, 100.f);
		}
		

		//glUniform3f(program.uniform("viewPos"), 0.0f,0.0f,3.0f);
		glUniformMatrix4fv(program.uniform("projection"), 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, &view[0][0]);
		//glUniform1f(program.uniform("isOffScreen"), 1.0f);
		//auto t_now = std::chrono::high_resolution_clock::now();
		//float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
		//view = glm::rotate(glm::mat4(1.f), 0 * (float)glfwGetTime() / 3, glm::vec3(1.0f, 1.0f, 0.0f));
		const float radius = 0.02f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		//view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
		//view=glm::lookAt(glm::vec3(cameraPosP.x, cameraPosP.y, cameraPosP.z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//view = glm::lookAt(glm::vec3(0.0f,0.0f,0.3f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		/*
		int height = 0;
		int width = 0;
		glfwGetWindowSize(window, &width, &height);
		projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 1.5f, 100.f);*/
		//projection = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
		//projection = glm::ortho(-1.5f, 1.5f, -1.5f, 1.5f, -1.5f, 1.5f);
		//glm::mat4x4 rotateMatrix = glm::rotate(glm::mat4(1.f), 1 * (float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 0.0f));
		//glm::mat4x4 MVPBunny = rotateMatrix * MVPBunnyMatrix;
		//glm::mat4x4 MVPCubeMatrix = rotateMatrix * MVPCubeMatrix;
		//glm::mat4x4 MVPCubeMatrix = glm::rotate(glm::mat4(1.f), 1 * (float)glfwGetTime()/3, glm::vec3(1.0f, 1.0f, 0.0f));
		//MVP = view * model;
		//glUniformMatrix4fv(program.uniform("MVP"), 1, GL_FALSE, &MVPMatrix[0][0]);

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Enable depth test
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);//woodTexture=0
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		
		//VAO.bind();
		//draw floor
		//glm::mat4 floorModel = glm::mat4(1.0f);
		
		drawPlaneMode = 1.0;
		glUniform1f(program.uniform("drawPlane"), drawPlaneMode);
		glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, &floorModel[0][0]);
		//glBindVertexArray(planeVAO);
		program.bindVertexAttribArray("position", planeVBO);
		program.bindVertexAttribArray("normal", planeNormalVBO);
		program.bindVertexAttribArray("texcoords", planeTexCoordVBO);
		program.bindVertexAttribArray("color", planeColorVBO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		drawPlaneMode = 0.0;
		glUniform1f(program.uniform("drawPlane"), drawPlaneMode);
		
		for (int i = 0; i < modelCube.size(); i++) {
			glStencilMask(0xFF);
			glStencilFunc(GL_ALWAYS, i, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, &modelCube[i][0][0]);
			program.bindVertexAttribArray("normal", cubeNormalVBO);
			program.bindVertexAttribArray("texcoords", cubeTexCoordVBO);
			program.bindVertexAttribArray("position", cubeVBO);
			program.bindVertexAttribArray("color", cubeColorVBO);
			glUniform1f(program.uniform("mirror"), cubeReflection[i]);
			//glUniform1f(program.uniform("transparent"), cubeMirror[i]);
			if (currentCubeMode[i] == 0) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawArrays(GL_TRIANGLES, 36 * i, 36);
			}
			if (currentCubeMode[i] == 1) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDrawArrays(GL_TRIANGLES, 36 * i, 36);
			}

			if (currentCubeMode[i] == 2) {
				vector<glm::vec3> copyOriginalColor;
				copyOriginalColor = C_cube;
				for (int j = 0; j < 2000; j++) {
					C_cube[j] = glm::vec3(0.0f, 0.0f, 0.0f);
				}
				cubeColorVBO.update(C_cube);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDrawArrays(GL_TRIANGLES, i * 36, 36);
				C_cube = copyOriginalColor;
				cubeColorVBO.update(C_cube);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawArrays(GL_TRIANGLES, i * 36, 36);
			}
			if (currentCubeMode[i] == 3) {
				cubeNormalVBO.update(cubePhongNormal);
				glDrawArrays(GL_TRIANGLES, 36 * i, 36);
			}
			
		}
		
		for (int i = 0; i < modelBumpyCube.size(); i++) {
			glStencilMask(0xFF);
			//bumpy cube id starts from 21
			glStencilFunc(GL_ALWAYS, i+20, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, &modelBumpyCube[i][0][0]);
			program.bindVertexAttribArray("normal", bumpyNormalVBO);
			program.bindVertexAttribArray("position", bumpyCubeVBO);
			program.bindVertexAttribArray("color", bumpyCubeColorVBO);
			glUniform1f(program.uniform("mirror"),bumpyReflection[i]);
			//glUniform1f(program.uniform("transparent"), bumpyMirror[i]);
			if (currentBumpyMode[i] == 0) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawArrays(GL_TRIANGLES, i * 3000, 3000);
			}
			if (currentBumpyMode[i] == 1) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDrawArrays(GL_TRIANGLES, i * 3000, 3000);
			}
			
			if (currentBumpyMode[i] == 2) {
				vector<glm::vec3> copyOriginalColor;
				copyOriginalColor = C_bumpyCube;
				for (int j = 0; j < 30000; j++) {
					C_bumpyCube[j] = glm::vec3(0.0f, 0.0f, 0.0f);
				}
				bumpyCubeColorVBO.update(C_bumpyCube);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDrawArrays(GL_TRIANGLES, i * 3000, 3000);
				C_bumpyCube = copyOriginalColor;
				bumpyCubeColorVBO.update(C_bumpyCube);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawArrays(GL_TRIANGLES, i * 3000, 3000);
			}
			if (currentBumpyMode[i] == 3) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawArrays(GL_TRIANGLES, i * 3000, 3000);
			}
			
		}
		
		for (int i = 0; i < modelBunny.size(); i++) {
			glStencilMask(0xFF);
			//bunny id starts from 31
			glStencilFunc(GL_ALWAYS, i+40, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, &modelBunny[i][0][0]);
			program.bindVertexAttribArray("normal", bunnyNormalVBO);
			program.bindVertexAttribArray("position", bunnyVBO);
			program.bindVertexAttribArray("color", bunnyColorVBO);
			glUniform1f(program.uniform("mirror"), bunnyReflection[i]);
			//glUniform1f(program.uniform("transparent"), bunnyMirror[i]);
			if (currentBunnyMode[i] == 0) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawArrays(GL_TRIANGLES, i * 3000, 3000);
			}
			if (currentBunnyMode[i] == 1) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDrawArrays(GL_TRIANGLES, i * 3000, 3000);
			}
			if (currentBunnyMode[i] == 2) {
				vector<glm::vec3> copyOriginalColor;
				copyOriginalColor = C_bunny;
				for (int j = 0; j < 30000; j++) {
					C_bunny[j] = glm::vec3(0.0f, 0.0f, 0.0f);
				}
				bunnyColorVBO.update(C_bunny);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDrawArrays(GL_TRIANGLES, i * 3000, 3000);
				C_bunny = copyOriginalColor;
				bunnyColorVBO.update(C_bunny);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawArrays(GL_TRIANGLES, i * 3000, 3000);
			}
			if (currentBunnyMode[i] == 3) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawArrays(GL_TRIANGLES, i * 3000, 3000);
			}

		}
		
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		
		
		program3.bind();
		glUniform1i(program3.uniform("skybox"), 0);
		
		glm::mat4 newView = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
		//view = glm::lookAt(glm::vec3(cameraPosP.x, cameraPosP.y, cameraPosP.z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(program3.uniform("projection"), 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(program3.uniform("view"), 1, GL_FALSE, &newView[0][0]);

		// skybox cube
		program3.bindVertexAttribArray("position", skyBoxVBO);
		//glBindVertexArray(skyboxVBO);
		//glDepthRange(1, 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		//glDepthFunc(GL_LESS); // set depth function back to default
		//glDepthRange(0, 1);
		program.bind();
		//VAO.bind();
		//program2.bind();
		//glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, &MVPCubeMatrix[0][0]);
		//program.bindVertexAttribArray("position", VBO_cube);
		//program.bindVertexAttribArray("color", VBO_cube_color);
		//program.bindVertexAttribArray("normal", VBO_normal);
		//glDrawArrays(GL_TRIANGLES, 0, 10000);

		//glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, &MVPBunny[0][0]);
		//program.bindVertexAttribArray("position", VBO_bunny);
		//glDrawArrays(GL_TRIANGLES, 0, 10000);
		//glClear(GL_STENCIL_BUFFER_BIT);
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	program.free();
	VAO.free();
	cubeVBO.free();
	cubeColorVBO.free();

	bunnyVBO.free();
	bunnyColorVBO.free();

	glfwTerminate();
	return 0;
}