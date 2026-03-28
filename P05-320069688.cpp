/*
Práctica 5: Optimización y Carga de Modelos
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
#include "Shader_m.h"
#include "Camera.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

// Modelos del Coche
Model Chasis, Cofre, Llanta1, Llanta2, Llanta3, Llanta4;

// Ángulos y traslaciones
float rotCofre = 0.0f;
float rotLlantas = 0.0f; 
float movCoche = 0.0f;   // Traslación en el eje Z

// Velocidades
float rotSpeed = 6.0f;  
float movSpeed = 2.0f;   

Skybox skybox;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

static const char* vShader = "shaders/shader_m.vert";
static const char* fShader = "shaders/shader_m.frag";

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

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.5f, 7.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 1.0f);

	Chasis = Model(); Chasis.LoadModel("Models/Coche.obj");
	Cofre = Model(); Cofre.LoadModel("Models/Cofre.obj");
	Llanta1 = Model(); Llanta1.LoadModel("Models/Llanta1.obj"); // Enfrente Izq
	Llanta2 = Model(); Llanta2.LoadModel("Models/Llanta2.obj"); // Enfrente Der
	Llanta3 = Model(); Llanta3.LoadModel("Models/Llanta3.obj"); // Atras Der
	Llanta4 = Model(); Llanta4.LoadModel("Models/Llanta4.obj"); // Atras Izq

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		// TECLA 1: Abrir Cofre 
		if (mainWindow.getsKeys()[GLFW_KEY_1]) {
			rotCofre += rotSpeed * deltaTime;
			if (rotCofre > 45.0f) rotCofre = 45.0f;
		}
		else {
			rotCofre -= rotSpeed * deltaTime;
			if (rotCofre < 0.0f) rotCofre = 0.0f;
		}

		// TECLA 2 y 3: Rotación de Llantas 
		if (mainWindow.getsKeys()[GLFW_KEY_2]) {
			rotLlantas -= rotSpeed * deltaTime * 3.0f; 
		}
		if (mainWindow.getsKeys()[GLFW_KEY_3]) {
			rotLlantas += rotSpeed * deltaTime * 3.0f;
		}

		// TECLA 4 y 5: Traslación de coche
		if (mainWindow.getsKeys()[GLFW_KEY_4]) {
			movCoche -= movSpeed * deltaTime; 
		}
		if (mainWindow.getsKeys()[GLFW_KEY_5]) {
			movCoche += movSpeed * deltaTime;
		}

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);


		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// PISO
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMesh();






		// --- VECTORES DE PIVOTE ---
		glm::vec3 pivCofre(0.0f, 1.0f, 1.5f);
		glm::vec3 pivLlanta1(0.25f, 0.3f, 1.08f);
		glm::vec3 pivLlanta2(0.25f, 0.3f, 1.08f);
		glm::vec3 pivLlanta3(-0.25f, 0.3f, -1.22f);
		glm::vec3 pivLlanta4(-0.25f, 0.3f, -1.22f);

		// --- 1. CHASIS (EL PADRE) ---
		color = glm::vec3(0.4f, 0.7f, 1.0f); //Azul claro
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		model = glm::mat4(1.0);
		// Aplicamos la traslación 
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, movCoche));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Chasis.RenderModel();

		// --- 2. COFRE (HIJO) ---
		color = glm::vec3(1.0f, 1.0f, 1.0f); // Color Blanco
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		modelaux = model; 
		modelaux = glm::translate(modelaux, pivCofre);
		modelaux = glm::rotate(modelaux, glm::radians(rotCofre), glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = glm::translate(modelaux, -pivCofre);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		Cofre.RenderModel();

		// --- 3. LLANTAS (HIJOS) ---
		color = glm::vec3(0.0f, 0.0f, 0.0f);// Color Negro
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		// Llanta 1
		modelaux = model;
		modelaux = glm::translate(modelaux, pivLlanta1);
		modelaux = glm::rotate(modelaux, glm::radians(rotLlantas), glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = glm::translate(modelaux, -pivLlanta1);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		Llanta1.RenderModel();

		// Llanta 2
		modelaux = model;
		modelaux = glm::translate(modelaux, pivLlanta2);
		modelaux = glm::rotate(modelaux, glm::radians(rotLlantas), glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = glm::translate(modelaux, -pivLlanta2);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		Llanta2.RenderModel();

		// Llanta 3
		modelaux = model;
		modelaux = glm::translate(modelaux, pivLlanta3);
		modelaux = glm::rotate(modelaux, glm::radians(rotLlantas), glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = glm::translate(modelaux, -pivLlanta3);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		Llanta3.RenderModel();

		// Llanta 4
		modelaux = model;
		modelaux = glm::translate(modelaux, pivLlanta4);
		modelaux = glm::rotate(modelaux, glm::radians(rotLlantas), glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = glm::translate(modelaux, -pivLlanta4);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		Llanta4.RenderModel();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}
