#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	mainWindow = NULL;
	bufferWidth = 0;
	bufferHeight = 0;
	lastX = 0.0f;
	lastY = 0.0f;
	xChange = 0.0f;
	yChange = 0.0f;
	muevex = 2.0f;

	//valor inicial de las variables de cada actividad (va en los dos constructores)
	//--- personajes ---
	caminaHeadcrab = false;

	//--- planetas: Xen y planeta 2 ---
	zonaPlaneta = 0;

	//--- planeta 3: Verdania (Abraham) ---
	animaVerdania = false;

	//--- pista de carreras y naves ---

	//--- satelites y cuerpos espaciales ---

	//--- plataforma de lanzamiento y cohete ---

	//--- estacion espacial y naves ---

	//--- avatar ---

	//--- camaras y recorridos ---

	//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---

	mouseFirstMoved = true;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	mainWindow = NULL;
	bufferWidth = 0;
	bufferHeight = 0;
	lastX = 0.0f;
	lastY = 0.0f;
	xChange = 0.0f;
	yChange = 0.0f;
	muevex = 2.0f;

	//valor inicial de las variables de cada actividad (va en los dos constructores)
	//--- personajes ---
	caminaHeadcrab = false;

	//--- planetas: Xen y planeta 2 ---
	zonaPlaneta = 0;

	//--- planeta 3: Verdania (Abraham) ---
	animaVerdania = false;

	//--- pista de carreras y naves ---

	//--- satelites y cuerpos espaciales ---

	//--- plataforma de lanzamiento y cohete ---

	//--- estacion espacial y naves ---

	//--- avatar ---

	//--- camaras y recorridos ---

	//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---

	mouseFirstMoved = true;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
int Window::Initialise()
{
	//iniciar GLFW
	if (!glfwInit())
	{
		printf("Fallo al inicializar GLFW\n");
		glfwTerminate();
		return 1;
	}
	//version de OpenGL que pedimos (4.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//con el core profile solo se usa OpenGL moderno, sin las funciones viejas
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//crear la ventana (el texto es el titulo)
	mainWindow = glfwCreateWindow(width, height, "Proyecto Final", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo al crear la ventana con GLFW\n");
		glfwTerminate();
		return 1;
	}
	//medidas reales del area donde se dibuja
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//decirle a OpenGL que vamos a dibujar en esta ventana
	glfwMakeContextCurrent(mainWindow);

	//teclado y mouse
	createCallbacks();

	//permitir extensiones nuevas
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Fallo al inicializar GLEW\n");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //buffer de profundidad, para que lo de atras no tape lo de enfrente

	glViewport(0, 0, bufferWidth, bufferHeight);
	//guarda esta clase dentro de la ventana para poder usarla en los callbacks (que son static)
	glfwSetWindowUserPointer(mainWindow, this);

	return 0;
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}

//regresan cuanto se movio el mouse desde la ultima vez y lo reinician en 0
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

//se llama una vez por cada evento de teclado, ya sea al presionar, mantener o soltar una tecla
//aqui se agregan las teclas nuevas
void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	//Y y U cambian muevex al presionar la tecla y mientras se deja presionada
	if (key == GLFW_KEY_Y && action != GLFW_RELEASE)
	{
		theWindow->muevex += 1.0;
	}
	if (key == GLFW_KEY_U && action != GLFW_RELEASE)
	{
		theWindow->muevex -= 1.0;
	}

	//teclas de cada actividad (la tabla de que tecla es de quien esta en README.md)
	//--- personajes ---
	if (key == GLFW_KEY_H && action == GLFW_PRESS)
	{
		theWindow->caminaHeadcrab = !theWindow->caminaHeadcrab;
	}

	//Z y X rotan entre los planetas: X va al siguiente y Z al anterior (1 Xen, 2 planeta 2, 3 Verdania)
	//al pasar del ultimo se regresa al primero y al reves; C regresa al espacio
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		theWindow->zonaPlaneta += 1;
		if (theWindow->zonaPlaneta > 3)
			theWindow->zonaPlaneta = 1;
	}
	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		theWindow->zonaPlaneta -= 1;
		if (theWindow->zonaPlaneta < 1)
			theWindow->zonaPlaneta = 3;
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		theWindow->zonaPlaneta = 0;
	}

	//--- planeta 3: Verdania (Abraham) ---
	//V prende o apaga las animaciones de las plantas de Verdania, como la H con el headcrab
	if (key == GLFW_KEY_V && action == GLFW_PRESS)
	{
		theWindow->animaVerdania = !theWindow->animaVerdania;
	}

	//--- pista de carreras y naves ---

	//--- satelites y cuerpos espaciales ---

	//--- plataforma de lanzamiento y cohete ---

	//--- estacion espacial y naves ---

	//--- avatar ---

	//--- camaras y recorridos ---

	//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---


	//guarda que teclas estan presionadas, la camara lo lee con getsKeys()
	//las teclas WASD ya las usa la camara
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	//la primera vez no hay posicion anterior, asi que se toma la actual para que la camara no brinque
	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}


Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}
