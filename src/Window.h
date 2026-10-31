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
	bool getpausaAnimaciones() { return pausaAnimaciones; }

	//getters de cada actividad (misma division que main.cpp)
	//--- personajes ---
	bool getcaminaHeadcrab() { return caminaHeadcrab; }

	//--- planetas: Xen y planeta 2 ---
	int getzonaPlaneta() { return zonaPlaneta; }
	int getreinicioPlaneta() { return reinicioPlaneta; }

	//--- planeta 3: Verdania ---
	bool getanimaVerdania() { return animaVerdania; }

	//--- pista de carreras y naves ---

	//--- satelites y cuerpos espaciales ---

	//--- plataforma de lanzamiento y cohete ---

	//--- estacion espacial y naves ---

	//--- avatar ---

	//--- camaras y recorridos ---
	int getmodoCamara() { return modoCamara; }
	int getobjetivoCamara() { return objetivoCamara; }
	int getpuntoEspacio() { return puntoEspacio; }

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
	//valor que cambia con Y y U
	//para agregar otro valor se pone la variable aqui, su getter arriba, su valor inicial en los constructores y la tecla en ManejaTeclado
	GLfloat muevex;
	bool pausaAnimaciones; //P pausa o continua todas las animaciones

	//variables de cada actividad
	//--- personajes ---
	bool caminaHeadcrab; //F prende o apaga la caminata del headcrab cuando se esta en Xen

	//--- planetas: Xen y planeta 2 ---
	int zonaPlaneta; //zona donde esta el usuario, vale 0 en el espacio, 1 en Xen, 2 en el planeta 2 y 3 en Verdania, y la cambian las teclas Z, X, C y V
	int reinicioPlaneta; //cuantas veces se ha presionado la tecla del planeta donde ya se esta, para regresar al punto de llegada
	void IrAPlaneta(int planeta);

	//--- planeta 3: Verdania ---
	bool animaVerdania; //F prende o apaga las animaciones de las plantas cuando se esta en Verdania

	//--- pista de carreras y naves ---

	//--- satelites y cuerpos espaciales ---

	//--- plataforma de lanzamiento y cohete ---

	//--- estacion espacial y naves ---

	//--- avatar ---

	//--- camaras y recorridos ---
	int modoCamara; //camara que se esta usando, de la 1 a la 5, y se cambia con las teclas 1 a 5
	int objetivoCamara; //nave o vista que sigue la camara; Q le resta 1 y E le suma 1
	int puntoEspacio; //cuantas veces se ha presionado la Z estando en el espacio, para ir al siguiente punto de interes

	//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---

	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};
