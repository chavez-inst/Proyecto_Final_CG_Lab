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
	pausaAnimaciones = false;

	//valor inicial de las variables de cada actividad (va en los dos constructores)
	//--- personajes ---
	caminaHeadcrab = false;

	//--- planetas: Xen y planeta 2 ---
	zonaPlaneta = 0;
	reinicioPlaneta = 0;

	//--- planeta 3: Verdania ---
	animaVerdania = false;

	//--- pista de carreras y naves ---

	//--- satelites y cuerpos espaciales ---

	//--- plataforma de lanzamiento y cohete ---

	//--- estacion espacial y naves ---

	//--- avatar ---

	//--- camaras y recorridos ---
	modoCamara = 1;
	objetivoCamara = 0;
	puntoEspacio = 0;

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
	pausaAnimaciones = false;

	//valor inicial de las variables de cada actividad (va en los dos constructores)
	//--- personajes ---
	caminaHeadcrab = false;

	//--- planetas: Xen y planeta 2 ---
	zonaPlaneta = 0;
	reinicioPlaneta = 0;

	//--- planeta 3: Verdania ---
	animaVerdania = false;

	//--- pista de carreras y naves ---

	//--- satelites y cuerpos espaciales ---

	//--- plataforma de lanzamiento y cohete ---

	//--- estacion espacial y naves ---

	//--- avatar ---

	//--- camaras y recorridos ---
	modoCamara = 1;
	objetivoCamara = 0;
	puntoEspacio = 0;

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

//--- planetas: Xen y planeta 2 ---
//lo usan las teclas X, C y V; si ya se esta en ese planeta, se regresa al punto de llegada
//la camara del avatar se queda si ya se estaba usando y cualquier otra cambia a la libre, porque las de las naves solo sirven en el espacio
void Window::IrAPlaneta(int planeta)
{
	if (zonaPlaneta == planeta)
	{
		reinicioPlaneta += 1;
	}
	else
	{
		zonaPlaneta = planeta;
	}
	if (modoCamara != 2)
	{
		modoCamara = 1;
	}
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
	//P pausa o continua todas las animaciones; en main.cpp se pone deltaTime en 0 mientras dure la pausa
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		theWindow->pausaAnimaciones = !theWindow->pausaAnimaciones;
	}

	//teclas de cada actividad (la tabla de que tecla es de quien esta en README.md)
	//la F prende o apaga la animacion basica de la escena donde se esta, por eso cada actividad revisa su zona
	//--- personajes ---
	if (key == GLFW_KEY_F && action == GLFW_PRESS && theWindow->zonaPlaneta == 1)
	{
		theWindow->caminaHeadcrab = !theWindow->caminaHeadcrab;
	}

	//--- planetas: Xen y planeta 2 ---
	//Z va al espacio, X a Xen, C al planeta 2 y V a Verdania
	//estando en el espacio, la Z lleva al siguiente punto de interes; el contador se reinicia al llegar desde un planeta
	//la camara del avatar solo sirve en los planetas, por eso al ir al espacio se cambia a la camara libre
	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		if (theWindow->zonaPlaneta == 0)
		{
			theWindow->puntoEspacio += 1;
		}
		else
		{
			theWindow->zonaPlaneta = 0;
			theWindow->puntoEspacio = 0;
		}
		theWindow->modoCamara = 1;
	}
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		theWindow->IrAPlaneta(1);
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		theWindow->IrAPlaneta(2);
	}
	if (key == GLFW_KEY_V && action == GLFW_PRESS)
	{
		theWindow->IrAPlaneta(3);
	}

	//--- planeta 3: Verdania ---
	//F prende o apaga las animaciones de las plantas cuando se esta en Verdania, igual que con el headcrab en Xen
	if (key == GLFW_KEY_F && action == GLFW_PRESS && theWindow->zonaPlaneta == 3)
	{
		theWindow->animaVerdania = !theWindow->animaVerdania;
	}

	//--- pista de carreras y naves ---

	//--- satelites y cuerpos espaciales ---

	//--- plataforma de lanzamiento y cohete ---

	//--- estacion espacial y naves ---

	//--- avatar ---

	//--- camaras y recorridos ---
	//1 es la camara libre, 2 la del avatar, 3 la de una nave de la carrera, 4 la de la nave que viaja entre planetas
	//y 5 la del lanzamiento y la estacion; los codigos de las teclas 1 a 5 van seguidos, asi que al restarles
	//el de la 1 y sumar 1 sale el numero de la camara
	//la 2 solo sirve en los planetas, por eso desde el espacio lleva a Xen, y de la 3 a la 5 solo sirven en el espacio
	if (key >= GLFW_KEY_1 && key <= GLFW_KEY_5 && action == GLFW_PRESS)
	{
		int modo = key - GLFW_KEY_1 + 1;
		if (modo != theWindow->modoCamara)
		{
			theWindow->objetivoCamara = 0;
		}
		theWindow->modoCamara = modo;
		if (modo == 2 && theWindow->zonaPlaneta == 0)
		{
			theWindow->zonaPlaneta = 1;
		}
		if (modo >= 3 && theWindow->zonaPlaneta != 0)
		{
			theWindow->zonaPlaneta = 0;
			theWindow->puntoEspacio = 0;
		}
	}
	//Q y E cambian a la nave anterior o a la siguiente en la camara 3, y a la vista anterior o a la siguiente en la 5
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		theWindow->objetivoCamara -= 1;
	}
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		theWindow->objetivoCamara += 1;
	}

	//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---


	//guarda que teclas estan presionadas, la camara libre y el avatar lo leen con getsKeys()
	//las teclas WASD ya las usan la camara libre y el avatar
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
