// práctica 3: Modelado Geométrico y Cámara Sintética.
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Sphere.h"
#include "Window.h"
#include "Camera.h"

using std::vector;

const float toRadians = 3.14159265f / 180.0f;
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;

static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
Sphere sp = Sphere(1.0, 20, 20);

void CrearCubo() {
	unsigned int cubo_indices[] = {
		0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1, 7, 6, 5, 5, 4, 7,
		4, 0, 3, 3, 7, 4, 4, 5, 1, 1, 0, 4, 3, 2, 6, 6, 7, 3
	};
	GLfloat cubo_vertices[] = {
		-0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

void CrearPiramideTriangular() {
	unsigned int indices_piramide_triangular[] = { 0,1,2, 1,3,2, 3,0,2, 1,0,3 };
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f, 0.0f,  0.5f, -0.5f, 0.0f,  0.0f, 0.5f, -0.25f,  0.0f, -0.5f, -0.5f
	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);
}

void CrearCilindro(int res, float R) {
	int n, i;
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;
	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	for (n = 0; n <= (res); n++) {
		if (n != res) { x = R * cos((n)*dt); z = R * sin((n)*dt); }
		else { x = R * cos((0) * dt); z = R * sin((0) * dt); }
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break;
			case 1: vertices.push_back(y); break;
			case 2: vertices.push_back(z); break;
			case 3: vertices.push_back(x); break;
			case 4: vertices.push_back(0.5); break;
			case 5: vertices.push_back(z); break;
			}
		}
	}

	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt); z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break;
			case 1: vertices.push_back(-0.5f); break;
			case 2: vertices.push_back(z); break;
			}
		}
	}

	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt); z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break;
			case 1: vertices.push_back(0.5); break;
			case 2: vertices.push_back(z); break;
			}
		}
	}

	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

void CrearCono(int res, float R) {
	int n, i;
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;
	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	vertices.push_back(0.0); vertices.push_back(0.5); vertices.push_back(0.0);

	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt); z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break;
			case 1: vertices.push_back(y); break;
			case 2: vertices.push_back(z); break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);

	for (i = 0; i < res + 2; i++) indices.push_back(i);

	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

void CrearPiramideCuadrangular() {
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4, 3,2,4, 2,1,4, 1,0,4, 0,1,2, 0,2,4
	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f,  0.5f,-0.5f,-0.5f,  -0.5f,-0.5f,-0.5f,  -0.5f,-0.5f,0.5f,  0.0f,0.5f,0.0f,
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}


// =========================================================
// RUBICK PIRAMIDE
// =========================================================

// Crea una pirámide pequeña (Para los centros y las aristas)
void CrearPiramidePequena(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3, glm::vec3 faceNormal, Mesh* mesh) {
	glm::vec3 centroid = (t1 + t2 + t3) / 3.0f;
	float S = 0.88f;
	glm::vec3 p1 = centroid + (t1 - centroid) * S;
	glm::vec3 p2 = centroid + (t2 - centroid) * S;
	glm::vec3 p3 = centroid + (t3 - centroid) * S;

	p1 += faceNormal * 0.02f;
	p2 += faceNormal * 0.02f;
	p3 += faceNormal * 0.02f;

	glm::vec3 tip = centroid - faceNormal * 0.3f;

	vector<GLfloat> vertices = {
		p1.x, p1.y, p1.z,
		p2.x, p2.y, p2.z,
		p3.x, p3.y, p3.z,
		tip.x, tip.y, tip.z
	};

	vector<unsigned int> indices = {
		0, 1, 2,
		0, 3, 1,
		1, 3, 2,
		2, 3, 0
	};

	mesh->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
}

// Crea la porción de una esquina sólida correspondiente a una cara coloreada
void CrearCaraEsquina(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3, glm::vec3 centroidCorner, glm::vec3 faceNormal, Mesh* mesh) {
	// En lugar de encoger hacia el centro de la cara, encogemos hacia el centro de la ESQUINA 3D.
	// Esto genera un bloque sólido perfecto.
	float S = 0.88f;
	glm::vec3 p1 = centroidCorner + (t1 - centroidCorner) * S;
	glm::vec3 p2 = centroidCorner + (t2 - centroidCorner) * S;
	glm::vec3 p3 = centroidCorner + (t3 - centroidCorner) * S;

	// Alineación visual con el resto de las piezas
	p1 += faceNormal * 0.02f;
	p2 += faceNormal * 0.02f;
	p3 += faceNormal * 0.02f;

	// La punta es exactamente el centro del tetraedro de la esquina
	glm::vec3 tip = centroidCorner + faceNormal * 0.02f;

	vector<GLfloat> vertices = {
		p1.x, p1.y, p1.z,       // Vértice 0
		p2.x, p2.y, p2.z,       // Vértice 1
		p3.x, p3.y, p3.z,       // Vértice 2
		tip.x, tip.y, tip.z		// Vértice 3 (Centro interno de la esquina)
	};

	vector<unsigned int> indices = {
		0, 1, 2,                // Cara principal
		0, 3, 1,                // Pared interna
		1, 3, 2,                // Pared interna
		2, 3, 0                 // Pared interna
	};

	mesh->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
}

// Genera los 9 triángulos de una cara del RUBICK
void GenerarPiramidesCara(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 cA, glm::vec3 cB, glm::vec3 cC) {
	int N = 3;
	vector<glm::vec3> pts;

	glm::vec3 faceNormal = glm::normalize((A + B + C) / 3.0f);

	for (int i = 0; i <= N; ++i) {
		for (int j = 0; j <= N - i; ++j) {
			int k = N - i - j;
			pts.push_back((float(i) * A + float(j) * B + float(k) * C) / float(N));
		}
	}

	auto getIdx = [](int i, int j) {
		int offset = 0;
		for (int r = 0; r < i; ++r) offset += (4 - r);
		return offset + j;
		};

	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N - i; ++j) {
			Mesh* m1 = new Mesh();
			glm::vec3 p1 = pts[getIdx(i, j)];
			glm::vec3 p2 = pts[getIdx(i + 1, j)];
			glm::vec3 p3 = pts[getIdx(i, j + 1)];

			// Detectar si el triángulo actual es una de las 3 esquinas
			if (i == 2 && j == 0) {
				CrearCaraEsquina(p1, p2, p3, cA, faceNormal, m1); // Esquina A
			}
			else if (i == 0 && j == 2) {
				CrearCaraEsquina(p1, p2, p3, cB, faceNormal, m1); // Esquina B
			}
			else if (i == 0 && j == 0) {
				CrearCaraEsquina(p1, p2, p3, cC, faceNormal, m1); // Esquina C
			}
			else {
				CrearPiramidePequena(p1, p2, p3, faceNormal, m1); // Pieza regular
			}
			meshList.push_back(m1);

			if (i + j < N - 1) {
				Mesh* m2 = new Mesh();
				glm::vec3 p4 = pts[getIdx(i + 1, j)];
				glm::vec3 p5 = pts[getIdx(i + 1, j + 1)];
				glm::vec3 p6 = pts[getIdx(i, j + 1)];
				CrearPiramidePequena(p4, p5, p6, faceNormal, m2);
				meshList.push_back(m2);
			}
		}
	}
}


void CrearPyraminx() {
	float R = 2.0f;
	glm::vec3 V0(0.0f, R, 0.0f);
	glm::vec3 V1(R * 0.9428f, -R * 0.3333f, 0.0f);
	glm::vec3 V2(-R * 0.4714f, -R * 0.3333f, R * 0.8165f);
	glm::vec3 V3(-R * 0.4714f, -R * 0.3333f, -R * 0.8165f);

	// Función lambda para calcular el centro volumétrico de las 4 esquinas principales
	auto calcCentroideEsquina = [](glm::vec3 vPrincipal, glm::vec3 vA, glm::vec3 vB, glm::vec3 vC) {
		float ratio = 1.0f / 3.0f;
		glm::vec3 pA = vPrincipal + (vA - vPrincipal) * ratio;
		glm::vec3 pB = vPrincipal + (vB - vPrincipal) * ratio;
		glm::vec3 pC = vPrincipal + (vC - vPrincipal) * ratio;
		return (vPrincipal + pA + pB + pC) / 4.0f;
		};

	glm::vec3 C0 = calcCentroideEsquina(V0, V1, V2, V3); // Esquina Top
	glm::vec3 C1 = calcCentroideEsquina(V1, V0, V2, V3); // Esquina Derecha
	glm::vec3 C2 = calcCentroideEsquina(V2, V0, V1, V3); // Esquina Frente-Izquierda
	glm::vec3 C3 = calcCentroideEsquina(V3, V0, V1, V2); // Esquina Atrás-Izquierda

	// 1. Crear la piramude negra grande
	Mesh* nucleoNegro = new Mesh();
	glm::vec3 n0 = V0 * 0.95f; glm::vec3 n1 = V1 * 0.95f;
	glm::vec3 n2 = V2 * 0.95f; glm::vec3 n3 = V3 * 0.95f;
	vector<GLfloat> vertN = {
		n0.x, n0.y, n0.z,  n1.x, n1.y, n1.z,  n2.x, n2.y, n2.z,  n3.x, n3.y, n3.z
	};
	vector<unsigned int> indN = { 0, 1, 2,  0, 2, 3,  0, 3, 1,  1, 3, 2 };
	nucleoNegro->CreateMeshGeometry(vertN, indN, vertN.size(), indN.size());
	meshList.push_back(nucleoNegro);

	// 2. Generar las caras pasándoles el centro de las esquinas para armar bloques sólidos

	GenerarPiramidesCara(V0, V2, V1, C0, C2, C1);	// Verde (índices 6 al 14)
	GenerarPiramidesCara(V0, V3, V2, C0, C3, C2);	// Azul (índices 15 al 23)
	GenerarPiramidesCara(V0, V1, V3, C0, C1, C3);	// Rojo (índices 24 al 32)
	GenerarPiramidesCara(V1, V2, V3, C1, C2, C3);	// Amarillo (índices 33 al 41) (Mencionados en clase :P)
}



void CreateShaders() {
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	Shader* shader2 = new Shader();
	shader2->CreateFromFiles(vShaderColor, fShader);
	shaderList.push_back(*shader2);
}

int main() {
	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	CrearCubo();
	CrearPiramideTriangular();
	CrearCilindro(5, 1.0f);
	CrearCono(25, 2.0f);
	CrearPiramideCuadrangular();

	CrearPyraminx(); // Crea los índices 5 al 41 en meshList

	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.3f);

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	sp.init();
	sp.load();

	glm::mat4 model(1.0);
	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);


	glEnable(GL_DEPTH_TEST);

	while (!mainWindow.getShouldClose()) {
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, -6.0f));

		// Animación de rotación
		model = glm::rotate(model, (float)glfwGetTime() * 0.8f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, (float)glfwGetTime() * 0.3f, glm::vec3(1.0f, 0.0f, 0.0f));

		// Controles por teclado por si quieres intervenir
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// ==========================================
		// RENDERIZADO DEL RUBICK
		// ==========================================

		// 1. Dibujar la piramide negra
		color = glm::vec3(0.05f, 0.05f, 0.05f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMeshGeometry();

		// 2. Dibujar las pirámides pequeñas de colores
		glm::vec3 colores[4] = {
			glm::vec3(0.1f, 0.8f, 0.2f), // Verde
			glm::vec3(0.1f, 0.3f, 0.9f), // Azul
			glm::vec3(0.9f, 0.1f, 0.1f), // Rojo
			glm::vec3(0.9f, 0.9f, 0.1f)  // Amarillo
		};

		int meshIndex = 6;
		for (int c = 0; c < 4; c++) {
			glUniform3fv(uniformColor, 1, glm::value_ptr(colores[c]));
			for (int i = 0; i < 9; i++) {
				meshList[meshIndex++]->RenderMeshGeometry();
			}
		}

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}

	
		