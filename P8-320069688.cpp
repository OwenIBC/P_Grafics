/*
Práctica 8: Iluminación 2
*/

#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture cocheTexture;
Texture dadoTexture;
Texture aguaTexture;
Texture logofiTexture;
Texture naveTexture;
Texture pezTexture; 

Model Kitt_M;
Model Llanta_M;
Model Llanta_M2;
Model Llanta_M3;
Model Llanta_M4;
Model Parb;
Model Lamp_M;


Model Nave_M;
Model Pez_M;

Skybox skybox;

Material Material_brillante;
Material Material_opaco;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

GLfloat heliX = 0.0f;
GLfloat pezOffset = 0.0f;

bool heliMovingForward = false;
bool heliMovingBackward = false;

bool lampLightOn = true;
bool lampKeyPressed = false;
bool pezLightOn = true;
bool pezKeyPressed = false;

// Direccion del Spotlight del Pez
GLfloat pezSpotDirX = 0.0f;
GLfloat pezSpotDirY = -1.0f;
GLfloat pezSpotDirZ = 0.0f;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";

void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = { 0, 3, 1, 1, 3, 2, 2, 3, 0, 0, 1, 2 };
	GLfloat vertices[] = {
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};
	unsigned int floorIndices[] = { 0, 2, 1, 1, 2, 3 };
	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
	unsigned int vegetacionIndices[] = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7 };
	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);
	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	Mesh* obj1 = new Mesh(); obj1->CreateMesh(vertices, indices, 32, 12); meshList.push_back(obj1);
	Mesh* obj2 = new Mesh(); obj2->CreateMesh(vertices, indices, 32, 12); meshList.push_back(obj2);
	Mesh* obj3 = new Mesh(); obj3->CreateMesh(floorVertices, floorIndices, 32, 6); meshList.push_back(obj3);
	Mesh* obj4 = new Mesh(); obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12); meshList.push_back(obj4);
}

void CrearDado()
{
	unsigned int octa_indices[] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23
	};
	GLfloat octa_vertices[] = {
		 0.0f,  0.5f,  0.0f,    0.5f, 0.25f,     -0.577f, 0.577f, 0.577f, -0.5f,  0.0f,  0.0f,    0.75f, 0.5f,     -0.577f, 0.577f, 0.577f,  0.0f,  0.0f,  0.5f,    0.25f, 0.5f,     -0.577f, 0.577f, 0.577f,
		 0.0f,  0.5f,  0.0f,    0.0f, 0.25f,     0.577f, 0.577f, 0.577f,  0.0f,  0.0f,  0.5f,    0.5f, 0.25f,     0.577f, 0.577f, 0.577f,  0.5f,  0.0f,  0.0f,    0.25f, 0.5f,     0.577f, 0.577f, 0.577f,
		 0.0f,  0.5f,  0.0f,    0.25f, 0.0f,     0.577f, 0.577f, -0.577f,  0.5f,  0.0f,  0.0f,    0.5f, 0.25f,     0.577f, 0.577f, -0.577f,  0.0f,  0.0f, -0.5f,    0.0f, 0.25f,     0.577f, 0.577f, -0.577f,
		 0.0f,  0.5f,  0.0f,    1.0f, 0.25f,     -0.577f, 0.577f, -0.577f,  0.0f,  0.0f, -0.5f,    0.75f, 0.5f,     -0.577f, 0.577f, -0.577f, -0.5f,  0.0f,  0.0f,    0.5f, 0.25f,     -0.577f, 0.577f, -0.577f,
		 0.0f, -0.5f,  0.0f,    0.5f, 0.75f,     -0.577f, -0.577f, 0.577f,  0.0f,  0.0f,  0.5f,    0.25f, 0.5f,     -0.577f, -0.577f, 0.577f, -0.5f,  0.0f,  0.0f,    0.75f, 0.5f,     -0.577f, -0.577f, 0.577f,
		0.0f, -0.5f,  0.0f,    0.0f, 0.75f,     0.577f, -0.577f, 0.577f, 0.5f,  0.0f,  0.0f,    0.25f, 0.5f,     0.577f, -0.577f, 0.577f, 0.0f,  0.0f,  0.5f,    0.5f, 0.75f,     0.577f, -0.577f, 0.577f,
		0.0f, -0.5f,  0.0f,    0.25f, 1.0f,     0.577f, -0.577f, -0.577f, 0.0f,  0.0f, -0.5f,    0.0f, 0.75f,     0.577f, -0.577f, -0.577f, 0.5f,  0.0f,  0.0f,    0.5f, 0.75f,     0.577f, -0.577f, -0.577f,
		0.0f, -0.5f,  0.0f,    1.0f, 0.75f,     -0.577f, -0.577f, -0.577f, -0.5f,  0.0f,  0.0f,    0.5f, 0.75f,     -0.577f, -0.577f, -0.577f, 0.0f,  0.0f, -0.5f,    0.75f, 0.5f,     -0.577f, -0.577f, -0.577f
	};
	Mesh* dado = new Mesh(); dado->CreateMesh(octa_vertices, octa_indices, 192, 24); meshList.push_back(dado);
}

void CrearPecera()
{
	unsigned int indicesVidrio[] = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19 };
	GLfloat verticesVidrio[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,    0.0f, -1.0f,  0.0f,  0.5f, -0.5f, -0.5f,  1.0f, 0.0f,    0.0f, -1.0f,  0.0f,  0.5f, -0.5f,  0.5f,  1.0f, 1.0f,    0.0f, -1.0f,  0.0f, -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,    0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,    0.0f,  0.0f,  1.0f,  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,    0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,    0.0f,  0.0f,  1.0f, -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,    0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f,    0.0f,  0.0f, -1.0f,  0.5f, -0.5f, -0.5f,  0.0f, 0.0f,    0.0f,  0.0f, -1.0f,  0.5f,  0.5f, -0.5f,  0.0f, 1.0f,    0.0f,  0.0f, -1.0f, -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,    0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   -1.0f,  0.0f,  0.0f, -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   -1.0f,  0.0f,  0.0f, -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   -1.0f,  0.0f,  0.0f, -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,    1.0f,  0.0f,  0.0f,  0.5f, -0.5f,  0.5f,  0.0f, 0.0f,    1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f, 1.0f,    1.0f,  0.0f,  0.0f,  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,    1.0f,  0.0f,  0.0f,
	};
	Mesh* vidrio = new Mesh(); vidrio->CreateMesh(verticesVidrio, indicesVidrio, 160, 30); meshList.push_back(vidrio);

	unsigned int indicesAgua[] = { 0, 1, 2, 0, 2, 3 };
	GLfloat verticesAgua[] = {
		-0.5f,  0.5f, -0.5f,  0.0f, 0.0f,    0.0f,  1.0f,  0.0f,  0.5f,  0.5f, -0.5f,  1.0f, 0.0f,    0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,    0.0f,  1.0f,  0.0f, -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,    0.0f,  1.0f,  0.0f,
	};
	Mesh* agua = new Mesh(); agua->CreateMesh(verticesAgua, indicesAgua, 32, 6); meshList.push_back(agua);
}

void CreateShaders()
{
	Shader* shader1 = new Shader(); shader1->CreateFromFiles(vShader, fShader); shaderList.push_back(*shader1);
}

void ProcesarTeclado()
{
	bool* keys = mainWindow.getsKeys();

	heliMovingForward = false;
	heliMovingBackward = false;

	if (keys[GLFW_KEY_T]) {
		heliX -= 1.5f * deltaTime;
		heliMovingForward = true;
	}
	if (keys[GLFW_KEY_G]) {
		heliX += 1.5f * deltaTime;
		heliMovingBackward = true;
	}

	if (keys[GLFW_KEY_I]) { pezOffset += 2.0f * deltaTime; }
	if (keys[GLFW_KEY_K]) { pezOffset -= 2.0f * deltaTime; }

	if (pezOffset > 1.2f) pezOffset = 1.2f;
	if (pezOffset < -1.2f) pezOffset = -1.2f;

	// Lámpara de calle (L) 
	if (keys[GLFW_KEY_L]) {
		if (!lampKeyPressed) {
			lampLightOn = !lampLightOn;
			lampKeyPressed = true;
		}
	}
	else { lampKeyPressed = false; }

	// Bulbo del Pez (P)
	if (keys[GLFW_KEY_P]) {
		if (!pezKeyPressed) {
			pezLightOn = !pezLightOn;
			pezKeyPressed = true;
		}
	}
	else { pezKeyPressed = false; }

	// Dirección Spotlight Pez
	// Eje X
	if (keys[GLFW_KEY_Z]) { pezSpotDirX += 1.0f * deltaTime; }
	if (keys[GLFW_KEY_X]) { pezSpotDirX -= 1.0f * deltaTime; }
	// Eje Y
	if (keys[GLFW_KEY_C]) { pezSpotDirY += 1.0f * deltaTime; }
	if (keys[GLFW_KEY_V]) { pezSpotDirY -= 1.0f * deltaTime; }
	// Eje Z
	if (keys[GLFW_KEY_B]) { pezSpotDirZ += 1.0f * deltaTime; }
	if (keys[GLFW_KEY_N]) { pezSpotDirZ -= 1.0f * deltaTime; }
}

int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();
	CreateObjects(); CrearDado(); CrearPecera(); CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png"); brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png"); dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png"); plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga"); pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga"); AgaveTexture.LoadTextureA();
	cocheTexture = Texture("Textures/bob.png"); cocheTexture.LoadTextureA();
	dadoTexture = Texture("Textures/dadomeme.png"); dadoTexture.LoadTextureA();
	logofiTexture = Texture("Textures/logofi.png"); logofiTexture.LoadTextureA();
	aguaTexture = Texture("Textures/agua2.png"); aguaTexture.LoadTextureA();

	naveTexture = Texture("Textures/naveT.png"); naveTexture.LoadTextureA();
	pezTexture = Texture("Textures/colorbase.jpg"); pezTexture.LoadTexture(); 

	Kitt_M = Model(); Kitt_M.LoadModel("Models/Coche.fbx");
	Llanta_M = Model(); Llanta_M.LoadModel("Models/L1.fbx");
	Llanta_M2 = Model(); Llanta_M2.LoadModel("Models/L2.fbx");
	Llanta_M3 = Model(); Llanta_M3.LoadModel("Models/L3.fbx");
	Llanta_M4 = Model(); Llanta_M4.LoadModel("Models/L4.fbx");
	Parb = Model(); Parb.LoadModel("Models/CoPa.fbx");
	Lamp_M = Model(); Lamp_M.LoadModel("Models/lamp.fbx");


	Nave_M = Model(); Nave_M.LoadModel("Models/nave.fbx");
	Pez_M = Model(); Pez_M.LoadModel("Models/peztrue.fbx");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");
	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(10.0f, 32);
	Material_opaco = Material(0.3f, 4);

	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 0.3f, 0.3f, 0.0f, 0.0f, -1.0f);

	unsigned int pointLightCount = 0;
	// Luz puntual 0: General
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -6.0f, 1.5f, 1.5f, 0.3f, 0.2f, 0.1f);
	pointLightCount++;
	// Luz puntual 1: Lámpara de calle ('L')
	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -5.0f, 2.0f, -5.0f, 0.3f, 0.2f, 0.1f);
	pointLightCount++;
	// Luz puntual 2: Bulbo del pez ('P')
	pointLights[2] = PointLight(0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 8.0f, 2.5f, -2.0f, 0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	// Spotlight 0: Linterna de cámara
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f, 0.5f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 5.0f);
	spotLightCount++;
	// Spotlight 1: Nave 
	spotLights[1] = SpotLight(1.0f, 1.0f, 0.0f, 1.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 20.0f);
	spotLightCount++;
	// Spotlight 2: Pez Abisal bulbo
	spotLights[2] = SpotLight(1.0f, 0.0f, 1.0f, 1.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 20.0f);
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	GLfloat now = 0.0f;
	glm::mat4 model(1.0f);
	glm::mat4 modelaux(1.0f);
	glm::vec3 color(1.0f, 1.0f, 1.0f);
	float escalaCoche = 1.2f;
	float escalaLlanta = 0.425f;
	glm::mat4 modelBaseCoche(1.0f);
	glm::mat4 modelChasis(1.0f);

	while (!mainWindow.getShouldClose())
	{
		now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		ProcesarTeclado();

		// Lámpara de calle estado
		if (lampLightOn) {
			pointLights[1] = PointLight(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -3.0f, 2.18f, -5.0f, 0.3f, 0.2f, 0.1f);
		}
		else {
			pointLights[1] = PointLight(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -3.0f, 2.18f, -5.0f, 0.3f, 0.2f, 0.1f);
		}

		// Posiciones base objetos móviles
		glm::mat4 modelHelicopteroBase = glm::mat4(1.0f);
		modelHelicopteroBase = glm::translate(modelHelicopteroBase, glm::vec3(heliX, 5.0f, 6.0f));
		glm::vec3 worldHeliLightPos = glm::vec3(modelHelicopteroBase * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

		glm::vec3 pezPos = glm::vec3(8.0f, 2.5f + pezOffset, -2.0f + pezOffset);
		glm::vec3 bulboPos = pezPos + glm::vec3(0.0f, 0.8f, 0.5f); // Bulbo compensado

		// Lógica Spotlight Nave 
		glm::vec3 dirAdelante = glm::normalize(glm::vec3(-1.0f, -1.0f, 0.0f));
		glm::vec3 dirAtras = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));
		glm::vec3 dirAbajo = glm::vec3(0.0f, -1.0f, 0.0f);

		if (heliMovingForward) {
			spotLights[1].SetFlash(worldHeliLightPos, dirAdelante);
			spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f, 1.0f, 4.0f, worldHeliLightPos.x, worldHeliLightPos.y, worldHeliLightPos.z, dirAdelante.x, dirAdelante.y, dirAdelante.z, 1.0f, 0.0f, 0.0f, 20.0f);
		}
		else if (heliMovingBackward) {
			spotLights[1].SetFlash(worldHeliLightPos, dirAtras);
			spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f, 1.0f, 4.0f, worldHeliLightPos.x, worldHeliLightPos.y, worldHeliLightPos.z, dirAtras.x, dirAtras.y, dirAtras.z, 1.0f, 0.0f, 0.0f, 20.0f);
		}
		else {
			spotLights[1].SetFlash(worldHeliLightPos, dirAbajo);
			spotLights[1] = SpotLight(1.0f, 1.0f, 0.0f, 1.0f, 4.0f, worldHeliLightPos.x, worldHeliLightPos.y, worldHeliLightPos.z, dirAbajo.x, dirAbajo.y, dirAbajo.z, 1.0f, 0.0f, 0.0f, 20.0f);
		}

		// Lógica de Luces del Pez 
		glm::vec3 bulbSpotDir = glm::vec3(pezSpotDirX, pezSpotDirY, pezSpotDirZ);
		if (glm::length(bulbSpotDir) > 0.01f) {
			bulbSpotDir = glm::normalize(bulbSpotDir);
		}
		else {
			bulbSpotDir = glm::vec3(0.0f, -1.0f, 0.0f);
		}

		if (pezLightOn) {
			pointLights[2] = PointLight(0.0f, 0.0f, 1.0f, 1.0f, 1.0f, bulboPos.x, bulboPos.y, bulboPos.z, 0.3f, 0.2f, 0.1f);
			spotLights[2].SetFlash(bulboPos, bulbSpotDir);
			spotLights[2] = SpotLight(1.0f, 0.0f, 1.0f, 1.0f, 4.0f, bulboPos.x, bulboPos.y, bulboPos.z, bulbSpotDir.x, bulbSpotDir.y, bulbSpotDir.z, 1.0f, 0.0f, 0.0f, 20.0f);
		}
		else {
			pointLights[2] = PointLight(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, bulboPos.x, bulboPos.y, bulboPos.z, 0.3f, 0.2f, 0.1f);
			spotLights[2] = SpotLight(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, bulboPos.x, bulboPos.y, bulboPos.z, bulbSpotDir.x, bulbSpotDir.y, bulbSpotDir.z, 1.0f, 0.0f, 0.0f, 20.0f);
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		glm::mat4 modelLinterna = glm::mat4(1.0f);
		modelLinterna = glm::translate(modelLinterna, camera.getCameraPosition());
		glm::vec4 localLinternaPos(0.0f, -0.3f, 0.0f, 1.0f);
		glm::vec3 worldLinternaPos = glm::vec3(modelLinterna * localLinternaPos);
		spotLights[0].SetFlash(worldLinternaPos, camera.getCameraDirection());

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		// Objetos Opacos

		model = glm::mat4(1.0f);
		modelaux = glm::mat4(1.0f);
		color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		cocheTexture.UseTexture();

		modelBaseCoche = glm::mat4(1.0f);
		modelBaseCoche = glm::translate(modelBaseCoche, glm::vec3(0.0f + mainWindow.getmuevex(), -0.5f, -3.0f));
		modelBaseCoche = glm::rotate(modelBaseCoche, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));

		modelChasis = glm::scale(modelBaseCoche, glm::vec3(escalaCoche, escalaCoche, escalaCoche));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelChasis));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kitt_M.RenderModel();
		Parb.RenderModel();

		model = modelBaseCoche;
		model = glm::translate(model, glm::vec3(-1.23f, 1.35f, 0.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(escalaLlanta));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cocheTexture.UseTexture();
		Llanta_M.RenderModel();

		model = modelBaseCoche;
		model = glm::translate(model, glm::vec3(1.23f, 1.35f, 0.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(escalaLlanta));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cocheTexture.UseTexture();
		Llanta_M2.RenderModel();

		model = modelBaseCoche;
		model = glm::translate(model, glm::vec3(-1.23f, -1.25f, 0.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaLlanta));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cocheTexture.UseTexture();
		Llanta_M3.RenderModel();

		model = modelBaseCoche;
		model = glm::translate(model, glm::vec3(1.23f, -1.25f, 0.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaLlanta));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cocheTexture.UseTexture();
		Llanta_M4.RenderModel();

		// Nave
		model = modelHelicopteroBase;
		model = glm::scale(model, glm::vec3(1.8f, 1.8f, 1.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		naveTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Nave_M.RenderModel();

		// Dado
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.5f, 2.5f, -2.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		// Lámpara de calle
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.0f, 2.18f, -5.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Lamp_M.RenderModel();

		// PEZ 
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(8.0f, 2.5f + pezOffset, -2.0f + pezOffset));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pezTexture.UseTexture(); // Textura explícita del pez
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pez_M.RenderModel();

		// Pecera (SÓLIDA)
		aguaTexture.UseTexture();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(8.0f, 2.5f, -2.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[6]->RenderMesh();



		// OBJETOS CON TRANSPARENCIA 

		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);

		// Agave 
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[3]->RenderMesh();

		// Pecera (VIDRIO)
		glBlendColor(0.0f, 0.0f, 0.0f, 0.4f);
		glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);

		aguaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(8.0f, 2.5f, -2.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[5]->RenderMesh();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}