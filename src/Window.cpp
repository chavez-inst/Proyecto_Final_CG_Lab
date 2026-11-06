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
	pausaAnimaciones = false;

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
	despegaCohete = false;

	//--- estacion espacial y naves ---

	//--- avatar ---
	personajeAvatar = 0; //empieza con Kirby

	//--- camaras y recorridos ---
	modoCamara = 1;
	objetivoCamara = 0;
	puntoEspacio = 0;
	viajeCamara = 1;
	reinicioViaje = 0;

	//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---
	linterna = false;
	lucesEscenario = true;
	saltoDiaNoche = 0;

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
	pausaAnimaciones = false;

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
	despegaCohete = false;

	//--- estacion espacial y naves ---

	//--- avatar ---
	personajeAvatar = 0; //empieza con Kirby

	//--- camaras y recorridos ---
	modoCamara = 1;
	objetivoCamara = 0;
	puntoEspacio = 0;
	viajeCamara = 1;
	reinicioViaje = 0;

	//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---
	linterna = false;
	lucesEscenario = true;
	saltoDiaNoche = 0;

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
//lo usan X, C y V; si ya se esta en ese planeta se regresa al punto de llegada, y si no es la camara 2 pasa a la libre
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
void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	//P pausa o continua todas las animaciones; en main.cpp se pone deltaTime en 0 mientras dure la pausa
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		theWindow->pausaAnimaciones = !theWindow->pausaAnimaciones;
		if (theWindow->pausaAnimaciones)
			printf("animaciones en pausa\n");
		else
			printf("animaciones corriendo\n");
	}

	//la F prende o apaga la animacion basica de la zona donde se esta
	//--- personajes ---
	if (key == GLFW_KEY_F && action == GLFW_PRESS && theWindow->zonaPlaneta == 1)
	{
		theWindow->caminaHeadcrab = !theWindow->caminaHeadcrab;
		if (theWindow->caminaHeadcrab)
			printf("headcrab caminando\n");
		else
			printf("headcrab detenido\n");
	}

	//--- planetas: Xen y planeta 2 ---
	//en el espacio la Z pasa al siguiente punto de interes; desde un planeta regresa al espacio y pasa a la camara libre
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
	if (key == GLFW_KEY_F && action == GLFW_PRESS && theWindow->zonaPlaneta == 3)
	{
		theWindow->animaVerdania = !theWindow->animaVerdania;
		if (theWindow->animaVerdania)
			printf("animaciones de Verdania prendidas\n");
		else
			printf("animaciones de Verdania apagadas\n");
	}

	//--- pista de carreras y naves ---

	//--- satelites y cuerpos espaciales ---

	//--- plataforma de lanzamiento y cohete ---
	//la F prende o apaga los despegues del cohete en el planeta 2, que se repiten mientras esta prendido
	if (key == GLFW_KEY_F && action == GLFW_PRESS && theWindow->zonaPlaneta == 2)
	{
		theWindow->despegaCohete = !theWindow->despegaCohete;
		if (theWindow->despegaCohete)
			printf("despegue del cohete prendido\n");
		else
			printf("despegue del cohete apagado, el cohete se queda en la plataforma al aterrizar\n");
	}

	//--- estacion espacial y naves ---

	//--- avatar ---
	//la T cambia el personaje de la camara 2 entre Kirby y la carnivora, y despues del ultimo regresa al primero
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		theWindow->personajeAvatar += 1;
		if (theWindow->personajeAvatar > 1)
			theWindow->personajeAvatar = 0;
		if (theWindow->personajeAvatar == 0)
			printf("el avatar es Kirby\n");
		else
			printf("el avatar es la carnivora\n");
	}

	//--- camaras y recorridos ---
	//los codigos de las teclas 1 a 5 van seguidos, asi que restar el de la 1 y sumar 1 da el numero de la camara
	//la 4 solo se entra con G, H o J; la 2 lleva a Xen desde el espacio, la 3 al espacio y la 5 al planeta 2 para ver el despegue
	if (key >= GLFW_KEY_1 && key <= GLFW_KEY_5 && key != GLFW_KEY_4 && action == GLFW_PRESS)
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
		if (modo == 3 && theWindow->zonaPlaneta != 0)
		{
			theWindow->zonaPlaneta = 0;
			theWindow->puntoEspacio = 0;
		}
		if (modo == 5)
		{
			theWindow->zonaPlaneta = 2;
		}
	}
	//Q y E cambian a la nave anterior o a la siguiente en la camara 3
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		theWindow->objetivoCamara -= 1;
	}
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		theWindow->objetivoCamara += 1;
	}
	//G, H y J ponen la camara 4 en el viaje a Xen, Pop Star o Verdania; si ya se iba en ese viaje, empieza otra vez
	if ((key == GLFW_KEY_G || key == GLFW_KEY_H || key == GLFW_KEY_J) && action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_G)
			theWindow->viajeCamara = 0;
		else if (key == GLFW_KEY_H)
			theWindow->viajeCamara = 1;
		else
			theWindow->viajeCamara = 2;
		theWindow->reinicioViaje += 1;
		theWindow->modoCamara = 4;
		theWindow->zonaPlaneta = 0;
		theWindow->puntoEspacio = 0;
	}

	//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---
	//L prende o apaga la linterna y K los spotlights de la zona; en main.cpp solo se mandan al shader si estan prendidos
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		theWindow->linterna = !theWindow->linterna;
		if (theWindow->linterna)
			printf("linterna prendida\n");
		else
			printf("linterna apagada\n");
	}
	if (key == GLFW_KEY_K && action == GLFW_PRESS)
	{
		theWindow->lucesEscenario = !theWindow->lucesEscenario;
		if (theWindow->lucesEscenario)
			printf("spotlights de la zona prendidos\n");
		else
			printf("spotlights de la zona apagados\n");
	}
	//N cambia entre dia y noche, pero solo en los planetas porque en el espacio el sol esta fijo
	if (key == GLFW_KEY_N && action == GLFW_PRESS)
	{
		if (theWindow->zonaPlaneta != 0)
			theWindow->saltoDiaNoche += 1;
		else
			printf("en el espacio el sol esta fijo, la N solo cambia el dia y la noche en los planetas\n");
	}

	//guarda las teclas presionadas para getsKeys(); WASD ya las usan la camara libre y el avatar
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
