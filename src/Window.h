#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

//crea la ventana con GLFW y guarda lo que pasa con el teclado y el mouse
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

	//getters de cada actividad (misma division que main.cpp)
	//--- personajes ---

	//--- planetas: Xen y planeta 2 ---

	//--- pista de carreras y naves ---

	//--- satelites y cuerpos espaciales ---

	//--- plataforma de lanzamiento y cohete ---

	//--- estacion espacial y naves ---

	//--- avatar ---

	//--- camaras y recorridos ---

	//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---

	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }

	~Window();
private:
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	//valor que cambia con Y/U
	//para agregar otro: la variable aqui, su getter arriba, darle valor en los constructores y la tecla en ManejaTeclado
	GLfloat muevex;

	//variables de cada actividad
	//--- personajes ---

	//--- planetas: Xen y planeta 2 ---

	//--- pista de carreras y naves ---

	//--- satelites y cuerpos espaciales ---

	//--- plataforma de lanzamiento y cohete ---

	//--- estacion espacial y naves ---

	//--- avatar ---

	//--- camaras y recorridos ---

	//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---

	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};
