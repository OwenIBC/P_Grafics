/*
4: Modelado Jerárquico - Perro de Minecraft
Controles:
- Teclas 1, 2, 3, 4: Articulaciones superiores de las 4 patas
- Teclas F, G, H, J: Articulaciones inferiores de las 4 patas
- Teclas 5, 6: Articulaciones de la cola (base y punta)
*/

#include <fstream>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include <gtc\random.hpp>
//clases para dar orden y limpieza al còdigo
#include "Mesh.h"
#include "Shader.h"
#include "Sphere.h"
#include "Window.h"
#include "Camera.h"

using std::vector;
const float toRadians = 3.14159265f / 180.0;
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader> shaderList;

static const char* vShader = "shader.vert";
static const char* fShader = "shader.frag";

// Solo se usara el cubo para el perro
void CrearCubo()
{
	unsigned int cubo_indices[] = {
		0, 1, 2, 2, 3, 0, // front
		1, 5, 6, 6, 2, 1, // right
		7, 6, 5, 5, 4, 7, // back
		4, 0, 3, 3, 7, 4, // left
		4, 5, 1, 1, 0, 4, // bottom
		3, 2, 6, 6, 7, 3  // top
	};

	GLfloat cubo_vertices[] = {
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	std::ofstream outVert("shader.vert");
	outVert << "#version 330 core\n"
		<< "layout (location = 0) in vec3 pos;\n"
		<< "uniform mat4 model;\n"
		<< "uniform mat4 projection;\n"
		<< "uniform mat4 view;\n"
		<< "void main(){\n"
		<< "    gl_Position = projection * view * model * vec4(pos, 1.0);\n"
		<< "}\n";
	outVert.close();

	std::ofstream outFrag("shader.frag");
	outFrag << "#version 330 core\n"
		<< "out vec4 colour;\n"
		<< "uniform vec3 color;\n"
		<< "void main(){\n"
		<< "    colour = vec4(color, 1.0);\n"
		<< "}\n";
	outFrag.close();

	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	CrearCubo(); // meshList[0] - El único objeto
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 5.0f, 15.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.2f, 0.2f);
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	glm::mat4 model(1.0);

	// --- VARIABLES DE ARTICULACIÓN DEL PERRO ---
	float angP1_sup = 0.0f; float angP1_inf = 0.0f;
	float angP2_sup = 0.0f; float angP2_inf = 0.0f;
	float angP3_sup = 0.0f; float angP3_inf = 0.0f;
	float angP4_sup = 0.0f; float angP4_inf = 0.0f;
	float angCola_base = 0.0f; float angCola_punta = 0.0f;

	// Colores
	glm::vec3 colorGris = glm::vec3(0.85f, 0.85f, 0.85f);
	glm::vec3 colorRojo = glm::vec3(0.8f, 0.1f, 0.1f);
	glm::vec3 colorBeige = glm::vec3(0.9f, 0.8f, 0.7f);
	glm::vec3 colorGrisOscuro = glm::vec3(0.3f, 0.3f, 0.3f);
	glm::vec3 colorNegro = glm::vec3(0.1f, 0.1f, 0.1f);

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		glfwPollEvents();

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// --- CONTROL DE TECLADO ---
		bool* keys = mainWindow.getsKeys();
		float vel = 100.0f * deltaTime;

		if (keys[GLFW_KEY_1]) angP1_sup += vel;
		if (keys[GLFW_KEY_2]) angP2_sup += vel;
		if (keys[GLFW_KEY_3]) angP3_sup += vel;
		if (keys[GLFW_KEY_4]) angP4_sup += vel;

		if (keys[GLFW_KEY_F]) angP1_inf += vel;
		if (keys[GLFW_KEY_G]) angP2_inf += vel;
		if (keys[GLFW_KEY_H]) angP3_inf += vel;
		if (keys[GLFW_KEY_J]) angP4_inf += vel;

		if (keys[GLFW_KEY_5]) angCola_base += vel;
		if (keys[GLFW_KEY_6]) angCola_punta += vel;

		glClearColor(0.4f, 0.6f, 0.9f, 1.0f); // Cielo azul claro
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// ========================================================
		// 1. TORSO
		// ========================================================
		glm::mat4 modelCuerpo = glm::mat4(1.0);
		modelCuerpo = glm::translate(modelCuerpo, glm::vec3(0.0f, 5.0f, -5.0f));
		glm::mat4 modelCuerpoAux = modelCuerpo;

		model = glm::scale(modelCuerpoAux, glm::vec3(3.0f, 3.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorGris));
		meshList[0]->RenderMesh();

		// ========================================================
		// 2. CUELLO Y CABEZA
		// ========================================================
		
		glm::mat4 modelCuello = modelCuerpoAux;

		modelCuello = glm::translate(modelCuello, glm::vec3(0.0f, 1.2f, 2.0f));

		// Collar Rojo 
		model = glm::scale(modelCuello, glm::vec3(3.6f, 3.6f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorRojo));
		meshList[0]->RenderMesh();

		glm::mat4 modelCabezaAux = modelCuello;

		modelCabezaAux = glm::translate(modelCabezaAux, glm::vec3(0.0f, 0.5f, 2.0f)); 

		// Cráneo principal
		model = glm::scale(modelCabezaAux, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorGris));
		meshList[0]->RenderMesh();

		// Hocico Superior (Beige/Café) 

		glm::mat4 modelHocicoAux = modelCabezaAux;
		modelHocicoAux = glm::translate(modelHocicoAux, glm::vec3(0.0f, -0.4f, 1.5f)); // Mantener Y baja, adelantar a la cara frontal
		model = glm::scale(modelHocicoAux, glm::vec3(1.5f, 0.6f, 2.5f)); // Achatado (Y=0.6f), profundo (proyectado Z=2.5f)
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorBeige));
		meshList[0]->RenderMesh();

		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorNegro));
		meshList[0]->RenderMesh();

		// Ojos y Orejas (centrados en el cráneo gris)
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorNegro)); // Ojos
		model = glm::translate(modelCabezaAux, glm::vec3(-0.8f, 0.5f, 1.51f)); // Ojo Izq
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		model = glm::translate(modelCabezaAux, glm::vec3(0.8f, 0.5f, 1.51f)); // Ojo Der
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		glUniform3fv(uniformColor, 1, glm::value_ptr(colorGrisOscuro)); // Orejas
		model = glm::translate(modelCabezaAux, glm::vec3(-1.0f, 1.8f, -0.5f)); // Oreja Izq
		model = glm::scale(model, glm::vec3(0.6f, 1.2f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		model = glm::translate(modelCabezaAux, glm::vec3(1.0f, 1.8f, -0.5f)); // Oreja Der
		model = glm::scale(model, glm::vec3(0.6f, 1.2f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();


		// ========================================================
		// 3. PATAS (4 patas, 2 partes cada una)
		// ========================================================
		// --- Pata 1: Frontal Izquierda ---
		glm::mat4 modelPata1 = modelCuerpoAux;
		modelPata1 = glm::translate(modelPata1, glm::vec3(-1.0f, -1.5f, 2.0f));
		modelPata1 = glm::rotate(modelPata1, glm::radians(angP1_sup), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelPata1_Aux = modelPata1;

		model = glm::translate(modelPata1, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 2.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorGris));
		meshList[0]->RenderMesh();

		modelPata1_Aux = glm::translate(modelPata1_Aux, glm::vec3(0.0f, -2.0f, 0.0f));
		modelPata1_Aux = glm::rotate(modelPata1_Aux, glm::radians(angP1_inf), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(modelPata1_Aux, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 2.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// --- Pata 2: Frontal Derecha ---
		glm::mat4 modelPata2 = modelCuerpoAux;
		modelPata2 = glm::translate(modelPata2, glm::vec3(1.0f, -1.5f, 2.0f));
		modelPata2 = glm::rotate(modelPata2, glm::radians(angP2_sup), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelPata2_Aux = modelPata2;

		model = glm::translate(modelPata2, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 2.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		modelPata2_Aux = glm::translate(modelPata2_Aux, glm::vec3(0.0f, -2.0f, 0.0f));
		modelPata2_Aux = glm::rotate(modelPata2_Aux, glm::radians(angP2_inf), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(modelPata2_Aux, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 2.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// --- Pata 3: Trasera Izquierda ---
		glm::mat4 modelPata3 = modelCuerpoAux;
		modelPata3 = glm::translate(modelPata3, glm::vec3(-1.0f, -1.5f, -2.0f));
		modelPata3 = glm::rotate(modelPata3, glm::radians(angP3_sup), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelPata3_Aux = modelPata3;

		model = glm::translate(modelPata3, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 2.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		modelPata3_Aux = glm::translate(modelPata3_Aux, glm::vec3(0.0f, -2.0f, 0.0f));
		modelPata3_Aux = glm::rotate(modelPata3_Aux, glm::radians(angP3_inf), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(modelPata3_Aux, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 2.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// --- Pata 4: Trasera Derecha ---
		glm::mat4 modelPata4 = modelCuerpoAux;
		modelPata4 = glm::translate(modelPata4, glm::vec3(1.0f, -1.5f, -2.0f));
		modelPata4 = glm::rotate(modelPata4, glm::radians(angP4_sup), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelPata4_Aux = modelPata4;

		model = glm::translate(modelPata4, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 2.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		modelPata4_Aux = glm::translate(modelPata4_Aux, glm::vec3(0.0f, -2.0f, 0.0f));
		modelPata4_Aux = glm::rotate(modelPata4_Aux, glm::radians(angP4_inf), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(modelPata4_Aux, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 2.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// ========================================================
		// 4. COLA
		// ========================================================
		glm::mat4 modelColaBase = modelCuerpoAux;
		modelColaBase = glm::translate(modelColaBase, glm::vec3(0.0f, 1.0f, -2.5f));
		modelColaBase = glm::rotate(modelColaBase, glm::radians(-30.0f + angCola_base), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelColaAux = modelColaBase;

		model = glm::translate(modelColaBase, glm::vec3(0.0f, 0.0f, -1.0f));
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorGris));
		meshList[0]->RenderMesh();

		modelColaAux = glm::translate(modelColaAux, glm::vec3(0.0f, 0.0f, -2.0f));
		modelColaAux = glm::rotate(modelColaAux, glm::radians(angCola_punta), glm::vec3(1.0f, 0.0f, 0.0f));

		model = glm::translate(modelColaAux, glm::vec3(0.0f, 0.0f, -1.0f));
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}