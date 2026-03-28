#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }

	// Esta función devuelve todo el arreglo de teclas, es la que usamos en el main
	bool* getsKeys() { return keys; }

	// Opcional: Si prefieres usar funciones con nombres específicos para el coche
	bool getCofreAction() { return keys[GLFW_KEY_1]; }
	bool getLlantasAdelanteAction() { return keys[GLFW_KEY_2]; }
	bool getLlantasAtrasAction() { return keys[GLFW_KEY_3]; }
	bool getCocheAdelanteAction() { return keys[GLFW_KEY_4]; }
	bool getCocheAtrasAction() { return keys[GLFW_KEY_5]; }

	// Corregido: glfwSwapBuffers devuelve void, no necesita "return"
	void swapBuffers() { glfwSwapBuffers(mainWindow); }

	~Window();
private:
	GLFWwindow* mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);
};


