/*
Proyecto Final - Computacion Grafica
En este archivo se arma toda la escena. Sale de la base del lab, que ya traia la ventana, la camara,
la iluminacion, las texturas, los modelos con Assimp y el skybox. La camara se mueve con WASD y el
mouse, el cubo de prueba con Y y U, y el programa se cierra con ESC.

El archivo esta partido en 4 bloques y cada uno empieza con una linea de ===. Primero van las
variables globales, luego la geometria que se hace a mano, como el piso y el cubo, despues el main,
donde se cargan las texturas, los modelos y las luces, y al final el while, que dibuja todo cada frame.
Dentro de cada bloque hay una seccion //--- ... --- para cada actividad, y cada quien escribe solo en
las suyas para que casi no haya conflictos al juntar las ramas. Las teclas de cada actividad estan en README.md
*/

//esto solo va aqui y en ningun otro archivo, porque si no da error de simbolos repetidos
#define STB_IMAGE_IMPLEMENTATION

#include <vector>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Model.h"
#include "Skybox.h"

//iluminacion
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;

// ================== 1. VARIABLES GLOBALES ==================

Window mainWindow;
std::vector<MeshModel*> meshListModel; //mallas hechas a mano, cada vertice lleva x y z de la posicion, u v de la textura y nx ny nz de la normal
std::vector<Shader> shaderList;

Camera camera;

//texturas, sus archivos van en la carpeta Textures
Texture pruebaTexture;
Texture vegetacionTexture;

//modelos, sus archivos van en Models y sus texturas en Textures
Model Prueba_M;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

//luz direccional, es como el sol y solo hay una
DirectionalLight mainLight;
//arreglos de luces, el maximo de cada una esta en CommonValues.h
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

//shaders que usan todos los objetos; los del skybox los carga Skybox.cpp
static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";

//cada actividad escribe solo en su seccion //--- ... ---, en los 4 bloques de este archivo y en Window.h/.cpp
//asi cada quien cambia lineas distintas y al juntar las ramas casi no hay conflictos

//--- personajes ---
Model capitanFalcon;

//los modelos con jerarquia estan separados en piezas .obj que comparten el mismo origen, todas con
//Y hacia arriba, los pies en y = 0 y mirando hacia Z positivo. "der" e "izq" son la derecha y la
//izquierda del personaje, y su derecha queda hacia X negativo
//por cada pieza se guarda el nombre del archivo, el padre, que es el indice de la pieza de la que cuelga
//y vale -1 si es la raiz, el pivote, que es el punto de la articulacion donde gira, y la rotacion en grados
//en x, y, z, que se puede cambiar en el while
//el padre siempre va antes que sus hijos en los arreglos

//la planta carnivora mide como 1.4 de alto
const int NUM_PIEZAS_CARNIVORA = 15;
Model Carnivora_M[NUM_PIEZAS_CARNIVORA];
const char* piezasCarnivora[NUM_PIEZAS_CARNIVORA] = {
	"base", "tallo1", "tallo2", "tallo3", "cabeza", "mandibula", "lengua",
	"hoja_del_der", "hoja_del_izq", "hoja_tras_der", "hoja_tras_izq",
	"liana_atras", "liana_der", "liana_frente", "liana_izq" };
int padresCarnivora[NUM_PIEZAS_CARNIVORA] = {
	-1, 0, 1, 2, 3, 4, 5,
	0, 0, 0, 0,
	0, 0, 0, 0 };
glm::vec3 pivotesCarnivora[NUM_PIEZAS_CARNIVORA] = {
	glm::vec3(0.0f, 0.0f, 0.0f),            //base
	glm::vec3(0.0113f, 0.2274f, 0.0774f),   //tallo1
	glm::vec3(0.0197f, 0.4567f, -0.1489f),  //tallo2
	glm::vec3(0.0265f, 0.6000f, -0.3316f),  //tallo3
	glm::vec3(0.0137f, 0.7786f, -0.2390f),  //cabeza
	glm::vec3(0.0f, 0.8332f, 0.0054f),      //mandibula
	glm::vec3(0.0125f, 0.7558f, -0.1502f),  //lengua
	glm::vec3(-0.0691f, 0.1764f, 0.1596f),  //hoja_del_der
	glm::vec3(0.0921f, 0.1770f, 0.1610f),   //hoja_del_izq
	glm::vec3(-0.0642f, 0.1621f, -0.0007f), //hoja_tras_der
	glm::vec3(0.0865f, 0.1632f, -0.0009f),  //hoja_tras_izq
	glm::vec3(0.0110f, 0.1359f, -0.0315f),  //liana_atras
	glm::vec3(-0.0573f, 0.2234f, 0.0081f),  //liana_der
	glm::vec3(0.0437f, 0.2970f, 0.1427f),   //liana_frente
	glm::vec3(0.0550f, 0.2254f, -0.0065f)   //liana_izq
};
glm::vec3 rotCarnivora[NUM_PIEZAS_CARNIVORA]; //las globales empiezan en 0

//Gordon mide como 1.4 de alto
const int NUM_PIEZAS_GORDON = 14;
Model Gordon_M[NUM_PIEZAS_GORDON];
const char* piezasGordon[NUM_PIEZAS_GORDON] = {
	"torso", "cabeza",
	"brazo_der", "antebrazo_der", "mano_der",
	"brazo_izq", "antebrazo_izq", "mano_izq",
	"muslo_der", "pantorrilla_der", "pie_der",
	"muslo_izq", "pantorrilla_izq", "pie_izq" };
int padresGordon[NUM_PIEZAS_GORDON] = {
	-1, 0,
	0, 2, 3,
	0, 5, 6,
	0, 8, 9,
	0, 11, 12 };
glm::vec3 pivotesGordon[NUM_PIEZAS_GORDON] = {
	glm::vec3(0.0f, 0.73277f, 0.01014f),       //torso, gira en la cadera
	glm::vec3(0.0f, 1.22766f, -0.01482f),      //cabeza, gira en el cuello
	glm::vec3(-0.14675f, 1.11121f, -0.03717f), //brazo_der, gira en el hombro
	glm::vec3(-0.29002f, 0.94174f, -0.04760f), //antebrazo_der, gira en el codo
	glm::vec3(-0.43085f, 0.77516f, -0.04468f), //mano_der, gira en la muneca
	glm::vec3(0.14675f, 1.11121f, -0.03717f),  //brazo_izq
	glm::vec3(0.29002f, 0.94174f, -0.04760f),  //antebrazo_izq
	glm::vec3(0.43085f, 0.77516f, -0.04469f),  //mano_izq
	glm::vec3(-0.07392f, 0.73277f, 0.01014f),  //muslo_der, gira en la cadera
	glm::vec3(-0.07569f, 0.39412f, -0.00743f), //pantorrilla_der, gira en la rodilla
	glm::vec3(-0.07732f, 0.08130f, -0.03441f), //pie_der, gira en el tobillo
	glm::vec3(0.07392f, 0.73277f, 0.01014f),   //muslo_izq
	glm::vec3(0.07569f, 0.39412f, -0.00743f),  //pantorrilla_izq
	glm::vec3(0.07732f, 0.08130f, -0.03441f)   //pie_izq
};
glm::vec3 rotGordon[NUM_PIEZAS_GORDON];

//Kirby mide como 0.73 de alto
const int NUM_PIEZAS_KIRBY = 5;
Model Kirby_M[NUM_PIEZAS_KIRBY];
const char* piezasKirby[NUM_PIEZAS_KIRBY] = {
	"cuerpo", "mano_der", "mano_izq", "pie_der", "pie_izq" };
int padresKirby[NUM_PIEZAS_KIRBY] = { -1, 0, 0, 0, 0 };
glm::vec3 pivotesKirby[NUM_PIEZAS_KIRBY] = {
	glm::vec3(0.0f, 0.4158f, 0.0f),         //cuerpo, gira en su centro
	glm::vec3(-0.2024f, 0.3899f, 0.0164f),  //mano_der
	glm::vec3(0.2014f, 0.3897f, 0.0170f),   //mano_izq
	glm::vec3(-0.1323f, 0.1498f, -0.0135f), //pie_der
	glm::vec3(0.1317f, 0.1505f, -0.0139f)   //pie_izq
};
glm::vec3 rotKirby[NUM_PIEZAS_KIRBY];

//el headcrab mide como 0.3 de alto y 0.6 de largo contando las patas, y se hizo en Blender
const int NUM_PIEZAS_HEADCRAB = 10;
Model Headcrab_M[NUM_PIEZAS_HEADCRAB];
const char* piezasHeadcrab[NUM_PIEZAS_HEADCRAB] = {
	"cuerpo", "boca",
	"muslo_del_der", "garra_del_der", "muslo_del_izq", "garra_del_izq",
	"muslo_tras_der", "garra_tras_der", "muslo_tras_izq", "garra_tras_izq" };
int padresHeadcrab[NUM_PIEZAS_HEADCRAB] = {
	-1, 0,
	0, 2, 0, 4,
	0, 6, 0, 8 };
glm::vec3 pivotesHeadcrab[NUM_PIEZAS_HEADCRAB] = {
	glm::vec3(0.0f, 0.15f, 0.0f),         //cuerpo, gira en su centro
	glm::vec3(0.0f, 0.10f, 0.10f),        //boca, se abre desde la bisagra de abajo
	glm::vec3(-0.095f, 0.135f, 0.085f),   //muslo_del_der, gira en la cadera
	glm::vec3(-0.175f, 0.205f, 0.205f),   //garra_del_der, gira en la rodilla
	glm::vec3(0.095f, 0.135f, 0.085f),    //muslo_del_izq
	glm::vec3(0.175f, 0.205f, 0.205f),    //garra_del_izq
	glm::vec3(-0.10f, 0.12f, -0.06f),     //muslo_tras_der
	glm::vec3(-0.20f, 0.165f, -0.12f),    //garra_tras_der
	glm::vec3(0.10f, 0.12f, -0.06f),      //muslo_tras_izq
	glm::vec3(0.20f, 0.165f, -0.12f)      //garra_tras_izq
};
glm::vec3 rotHeadcrab[NUM_PIEZAS_HEADCRAB];

//variables de la caminata del headcrab, que da vueltas en circulo en la superficie de Xen y empieza o se detiene con H
glm::vec3 centroHeadcrab = glm::vec3(0.0f, 0.0f, 2.0f); //el centro del circulo se mide desde posXen
float radioHeadcrab = 2.5f;
float anguloHeadcrab = 0.0f;
float anguloHeadcrabOffset = 0.4f;
//las patas van y vienen entre -limitePaso y limitePaso, como los paneles del satelite de la practica 9
float pasoHeadcrab = 0.0f;
float pasoHeadcrabOffset = 2.5f;
float limitePaso = 20.0f;
bool pasoAdelante = true;

//--- planetas: Xen y planeta 2 ---
//la superficie de cada planeta es un piso aparte y muy lejos del espacio, como el plano lejano
//de la perspectiva esta en 1000 nunca se ven juntos
glm::vec3 posXen = glm::vec3(3000.0f, 0.0f, 0.0f);
glm::vec3 posPlaneta2 = glm::vec3(-3000.0f, 0.0f, 0.0f);
//zona donde esta el usuario, 0 en el espacio, 1 en Xen y 2 en el planeta 2; se compara con la de Window para saber cuando cambio
int zonaActual = 0;
Skybox skyboxXen;
Skybox skyboxPlaneta2;
Texture planeta2SueloTexture;
Texture xenSueloTexture;

//el planeta Xen que se ve desde el espacio es una esfera que gira sobre su eje
Model XenPlaneta_M;
glm::vec3 posPlanetaXen = glm::vec3(-300.0f, 80.0f, -500.0f);
float giroPlanetaXen = 0.0f;
float giroPlanetaXenOffset = 0.05f;

//el planeta 2 es Pop Star, el planeta de Kirby, que tiene forma de estrella; se ve de frente y gira como rehilete
Model Planeta2Espacio_M;
glm::vec3 posPlaneta2Espacio = glm::vec3(-500.0f, 120.0f, -100.0f);
float giroPlaneta2 = 0.0f;
float giroPlaneta2Offset = 0.04f;

//objetos de la superficie del planeta 2, hechos en Blender
Model Planeta2Arbol_M;
Model Planeta2Nube1_M;
Model Planeta2Nube2_M;
Model WarpStar_M;
Model Planeta2Colina_M;

//las colinas van en las orillas del mapa como paisaje, porque la camara no choca con nada y las atravesaria
//el modelo mide 1 de radio y 0.4 de alto, y se baja para que la orilla, donde la textura se estira, quede enterrada
const int NUM_COLINAS = 5;
glm::vec3 posColinas[NUM_COLINAS] = {
	glm::vec3(-35.0f, -1.0f, -72.0f),
	glm::vec3(30.0f, -1.0f, -68.0f),
	glm::vec3(-72.0f, -1.0f, 0.0f),
	glm::vec3(70.0f, -1.0f, 15.0f),
	glm::vec3(10.0f, -1.0f, 68.0f) };
float giroColinas[NUM_COLINAS] = { 0.0f, 70.0f, 140.0f, 210.0f, 290.0f };
float escalaColinas[NUM_COLINAS] = { 24.0f, 30.0f, 26.0f, 28.0f, 30.0f };

//el mismo arbol se repite con distinto giro y tamano para que no se vean todos iguales
const int NUM_ARBOLES = 6;
glm::vec3 posArboles[NUM_ARBOLES] = {
	glm::vec3(-15.0f, 0.0f, -20.0f),
	glm::vec3(18.0f, 0.0f, -25.0f),
	glm::vec3(-30.0f, 0.0f, 5.0f),
	glm::vec3(28.0f, 0.0f, 10.0f),
	glm::vec3(-8.0f, 0.0f, -40.0f),
	glm::vec3(40.0f, 0.0f, -35.0f) };
float giroArboles[NUM_ARBOLES] = { 0.0f, 60.0f, 130.0f, 200.0f, 250.0f, 310.0f };
float escalaArboles[NUM_ARBOLES] = { 1.2f, 1.4f, 1.1f, 1.3f, 1.5f, 1.2f };

//hay dos modelos de nube; tipoNubes dice cual se dibuja en cada lugar
const int NUM_NUBES = 5;
glm::vec3 posNubes[NUM_NUBES] = {
	glm::vec3(-20.0f, 28.0f, -50.0f),
	glm::vec3(25.0f, 32.0f, -60.0f),
	glm::vec3(0.0f, 40.0f, -90.0f),
	glm::vec3(-45.0f, 30.0f, -20.0f),
	glm::vec3(50.0f, 26.0f, -10.0f) };
int tipoNubes[NUM_NUBES] = { 2, 1, 2, 1, 1 };
float escalaNubes[NUM_NUBES] = { 3.0f, 3.0f, 4.0f, 3.0f, 2.5f };

//la warp star flota un poco arriba del piso, enfrente de donde llega la camara
glm::vec3 posWarpStar = glm::vec3(4.0f, 0.8f, -4.0f);

//objetos de la superficie de Xen, hechos en Blender
Model XenCristal_M;
Model XenIsla_M;
Model XenTallo_M;
Model XenFoco_M;

//criaturas de Xen acomodadas de la mas simple a la mas evolucionada; son descargadas y los creditos
//estan en el LICENCIA.txt de cada una
Model Houndeye_M;
Model Bullsquid_M;
Model Vortigaunt_M;
Model AlienGrunt_M;

//las islas flotantes suben y bajan con el seno de un angulo que avanza cada frame
glm::vec3 posIslas[3] = {
	glm::vec3(-20.0f, 12.0f, -30.0f),
	glm::vec3(25.0f, 18.0f, -20.0f),
	glm::vec3(5.0f, 25.0f, -45.0f) };
float escalaIslas[3] = { 3.0f, 4.0f, 6.0f };
float anguloIslas = 0.0f;
float anguloIslasOffset = 0.6f;

//las plantas de luz se esconden en el piso si la camara se acerca, como en Half-Life
//alturaPlantas escala el tallo en Y, vale 1 cuando la planta esta parada y 0.1 cuando esta escondida
const int NUM_PLANTAS = 3;
glm::vec3 posPlantas[NUM_PLANTAS] = {
	glm::vec3(-7.0f, 0.0f, 2.0f),
	glm::vec3(8.5f, 0.0f, 0.0f),
	glm::vec3(2.0f, 0.0f, -15.0f) };
float alturaPlantas[NUM_PLANTAS] = { 1.0f, 1.0f, 1.0f };
float altoTallo = 1.62f; //altura de la punta del tallo, ahi va el foco

//--- pista de carreras y naves ---
Model pistaM;
Model nave1Race;

//--- satelites y cuerpos espaciales ---
Model sateliteRace;

//--- plataforma de lanzamiento y cohete ---

//--- estacion espacial y naves ---

//--- avatar ---

//--- camaras y recorridos ---

//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---


// ================== 2. GEOMETRIA HECHA A MANO ==================

//calcula las normales promediando las de los triangulos que comparten cada vertice
//se llama antes de CreateMeshModel porque ahi es donde se copian los vertices a la GPU
//en este proyecto las normales apuntan hacia adentro del objeto, asi que los triangulos
//tienen que ir en sentido horario vistos desde afuera
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


//cada malla que se crea aqui se mete a meshListModel y luego se dibuja con su indice
void CreateObjects()
{
	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	//las normales van hacia abajo, o sea hacia adentro del piso, por como el shader calcula la luz
	//las UV llegan a 10 para que la textura se repita 10 veces
	GLfloat floorVertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	//dos planos cruzados, sirve para plantas o cosas con transparencia
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4, 5, 6,
	   4, 6, 7
	};

	//mismas normales que el piso para que se ilumine igual de los dos lados
	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	};

	//CreateMeshModel(vertices, indices, cantidad de floats del arreglo, cantidad de indices)
	MeshModel* piso = new MeshModel();
	piso->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshListModel.push_back(piso); //meshListModel[0]

	MeshModel* vegetacion = new MeshModel();
	vegetacion->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshListModel.push_back(vegetacion); //meshListModel[1]
}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1); //shaderList[0]
}


//cubo de lado 1 centrado en el origen, con la imagen completa en cada cara
void CrearCubo()
{
	//cada normal apunta hacia adentro del cubo, al reves de su cara, porque el shader calcula la luz con
	//FragPos menos la posicion de la luz, y si apuntaran hacia afuera esa cara se veria oscura
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,
		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
	};

	GLfloat cubo_vertices[] = {
		//  x      y      z      S      T      NX     NY     NZ
		// front
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,

		// right
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,

		 // back, la S va al reves para que la imagen no salga en espejo
		 -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
		  0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
		  0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		 -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,

		 // left
		 -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		 -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		 -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,

		 // bottom
		 -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
		  0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
		  0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		 -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,

		 // top
		 -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
		 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
	};

	MeshModel* cubo = new MeshModel();
	cubo->CreateMeshModel(cubo_vertices, cubo_indices, 192, 36);
	meshListModel.push_back(cubo); //meshListModel[2]
}


//dibuja un modelo con jerarquia, donde la matriz de cada pieza es la de su padre mas su propia rotacion
//para girar en la articulacion se lleva el pivote al origen, se rota y se regresa
//raiz es la matriz que coloca todo el modelo en la escena
void DibujaJerarquia(Model piezas[], int padres[], glm::vec3 pivotes[], glm::vec3 rotaciones[], int numPiezas,
	glm::mat4 raiz, GLuint uniformModel)
{
	glm::mat4 matrices[20]; //una por pieza, para que los hijos usen la de su padre
	for (int i = 0; i < numPiezas; i++)
	{
		glm::mat4 model;
		if (padres[i] == -1)
			model = raiz;
		else
			model = matrices[padres[i]];

		model = glm::translate(model, pivotes[i]);
		model = glm::rotate(model, rotaciones[i].x * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotaciones[i].y * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotaciones[i].z * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, -pivotes[i]);
		matrices[i] = model;

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		piezas[i].RenderModel();
	}
}


//funciones con geometria de cada actividad; se llaman en la carga de recursos, dentro de su seccion
//el indice en meshListModel depende del orden en que se agregan, por eso cada quien lo guarda en una
//variable global antes del push_back, por ejemplo idxCohete = meshListModel.size();, y dibuja con meshListModel[idxCohete]

//--- personajes ---

//--- planetas: Xen y planeta 2 ---

//--- pista de carreras y naves ---

//--- satelites y cuerpos espaciales ---

//--- plataforma de lanzamiento y cohete ---

//--- estacion espacial y naves ---

//--- avatar ---

//--- camaras y recorridos ---

//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---


int main()
{
	mainWindow = Window(1366, 768); //tambien jala con 1280x1024 o 1024x768
	mainWindow.Initialise();
	CreateObjects();
	CrearCubo();
	CreateShaders();

	//Camera(posicion inicial, vector up, yaw, pitch, velocidad de movimiento, velocidad de giro)
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	// ================== 3. CARGA DE RECURSOS ==================

	//en las texturas primero se asigna la ruta y luego se carga
	//LoadTextureA es para imagenes con canal alfa, como png o tga, y LoadTexture para las que no tienen, como jpg
	pruebaTexture = Texture("Textures/prueba.png");
	pruebaTexture.LoadTextureA();
	vegetacionTexture = Texture("Textures/prueba_alpha.png");
	vegetacionTexture.LoadTextureA();

	//modelos
	Prueba_M = Model();
	Prueba_M.LoadModel("Models/prueba.obj");

	//las 6 caras del skybox van en este orden: rt, lf, dn, up, bk, ft
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/sBRight.png");//derecha, hacia X positivo
	skyboxFaces.push_back("Textures/Skybox/Left.png");//izquierda, hacia X negativo
	skyboxFaces.push_back("Textures/Skybox/DownF.png");//abajo, hacia Y negativo
	skyboxFaces.push_back("Textures/Skybox/Top.png");//arriba, hacia Y positivo
	skyboxFaces.push_back("Textures/Skybox/FrontF.png");//atras, hacia Z positivo
	skyboxFaces.push_back("Textures/Skybox/BackF.png");//enfrente, hacia Z negativo

	skybox = Skybox(skyboxFaces);

	//los materiales se crean con Material(intensidad especular, shininess)
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//--- personajes ---
	capitanFalcon = Model();
	capitanFalcon.LoadModel("Models/capitanFalcon/capitanFalcon.obj");

	//las rutas se arman con el nombre de cada pieza
	for (int i = 0; i < NUM_PIEZAS_CARNIVORA; i++)
	{
		std::string ruta = std::string("Models/Carnivora/carnivora_") + piezasCarnivora[i] + ".obj";
		Carnivora_M[i] = Model();
		Carnivora_M[i].LoadModel(ruta);
	}

	for (int i = 0; i < NUM_PIEZAS_GORDON; i++)
	{
		std::string ruta = std::string("Models/GordonParts/gordon_") + piezasGordon[i] + ".obj";
		Gordon_M[i] = Model();
		Gordon_M[i].LoadModel(ruta);
	}

	for (int i = 0; i < NUM_PIEZAS_KIRBY; i++)
	{
		std::string ruta = std::string("Models/Kirby/kirby_") + piezasKirby[i] + ".obj";
		Kirby_M[i] = Model();
		Kirby_M[i].LoadModel(ruta);
	}

	for (int i = 0; i < NUM_PIEZAS_HEADCRAB; i++)
	{
		std::string ruta = std::string("Models/Headcrab/headcrab_") + piezasHeadcrab[i] + ".obj";
		Headcrab_M[i] = Model();
		Headcrab_M[i].LoadModel(ruta);
	}

	//--- planetas: Xen y planeta 2 ---
	//skybox de Xen, hecho en Blender; las caras van en el mismo orden que en el otro skybox: rt, lf, dn, up, bk, ft
	std::vector<std::string> skyboxXenFaces;
	skyboxXenFaces.push_back("Textures/Skybox/xen_rt.png");
	skyboxXenFaces.push_back("Textures/Skybox/xen_lf.png");
	skyboxXenFaces.push_back("Textures/Skybox/xen_dn.png");
	skyboxXenFaces.push_back("Textures/Skybox/xen_up.png");
	skyboxXenFaces.push_back("Textures/Skybox/xen_bk.png");
	skyboxXenFaces.push_back("Textures/Skybox/xen_ft.png");
	skyboxXen = Skybox(skyboxXenFaces);

	xenSueloTexture = Texture("Textures/xen_suelo.png");
	xenSueloTexture.LoadTextureA();

	XenPlaneta_M = Model();
	XenPlaneta_M.LoadModel("Models/Xen/xen_planeta.obj");
	XenCristal_M = Model();
	XenCristal_M.LoadModel("Models/Xen/xen_cristal.obj");
	XenIsla_M = Model();
	XenIsla_M.LoadModel("Models/Xen/xen_isla.obj");
	XenTallo_M = Model();
	XenTallo_M.LoadModel("Models/Xen/xen_planta_tallo.obj");
	XenFoco_M = Model();
	XenFoco_M.LoadModel("Models/Xen/xen_planta_foco.obj");

	Houndeye_M = Model();
	Houndeye_M.LoadModel("Models/Houndeye/houndeye.obj");
	Bullsquid_M = Model();
	Bullsquid_M.LoadModel("Models/Bullsquid/bullsquid.obj");
	Vortigaunt_M = Model();
	Vortigaunt_M.LoadModel("Models/Vortigaunt/vortigaunt.obj");
	AlienGrunt_M = Model();
	AlienGrunt_M.LoadModel("Models/AlienGrunt/aliengrunt.obj");

	//skybox, piso y estrella del planeta 2, hechos en Blender
	std::vector<std::string> skyboxPlaneta2Faces;
	skyboxPlaneta2Faces.push_back("Textures/Skybox/planeta2_rt.png");
	skyboxPlaneta2Faces.push_back("Textures/Skybox/planeta2_lf.png");
	skyboxPlaneta2Faces.push_back("Textures/Skybox/planeta2_dn.png");
	skyboxPlaneta2Faces.push_back("Textures/Skybox/planeta2_up.png");
	skyboxPlaneta2Faces.push_back("Textures/Skybox/planeta2_bk.png");
	skyboxPlaneta2Faces.push_back("Textures/Skybox/planeta2_ft.png");
	skyboxPlaneta2 = Skybox(skyboxPlaneta2Faces);

	planeta2SueloTexture = Texture("Textures/planeta2_suelo.png");
	planeta2SueloTexture.LoadTextureA();

	Planeta2Espacio_M = Model();
	Planeta2Espacio_M.LoadModel("Models/Planeta2/planeta2_planeta.obj");
	Planeta2Arbol_M = Model();
	Planeta2Arbol_M.LoadModel("Models/Planeta2/planeta2_arbol.obj");
	Planeta2Nube1_M = Model();
	Planeta2Nube1_M.LoadModel("Models/Planeta2/planeta2_nube1.obj");
	Planeta2Nube2_M = Model();
	Planeta2Nube2_M.LoadModel("Models/Planeta2/planeta2_nube2.obj");
	WarpStar_M = Model();
	WarpStar_M.LoadModel("Models/Planeta2/planeta2_warpstar.obj");
	Planeta2Colina_M = Model();
	Planeta2Colina_M.LoadModel("Models/Planeta2/planeta2_colina.obj");

	//--- pista de carreras y naves ---
	pistaM = Model();
	pistaM.LoadModel("Models/pista/pista.obj");
	nave1Race = Model();
	nave1Race.LoadModel("Models/nave1Race/nave1Race.obj");

	//--- satelites y cuerpos espaciales ---
	sateliteRace = Model();
	sateliteRace.LoadModel("Models/sateliteRace/sateliteRace.obj");

	//--- plataforma de lanzamiento y cohete ---

	//--- estacion espacial y naves ---

	//--- avatar ---

	//--- camaras y recorridos ---


	// ---------- luces ----------
	//los parametros van en este orden:
	//DirectionalLight(r, g, b, intensidad ambiental, intensidad difusa, direccion x, y, z)
	//PointLight(r, g, b, ambiental, difusa, posicion x, y, z, constante, lineal, exponente)
	//SpotLight(r, g, b, ambiental, difusa, posicion x, y, z, direccion x, y, z, constante, lineal, exponente, angulo del cono)

	//solo hay una luz direccional y siempre tiene que existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.2f, 0.4f,
		0.0f, -1.0f, -1.0f);

	//contador de luces puntuales, no puede pasar de MAX_POINT_LIGHTS
	unsigned int pointLightCount = 0;
	//luz puntual blanca
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,
		0.0f, 0.6f,
		2.0f, 1.5f, -3.0f,
		1.0f, 0.05f, 0.005f);
	pointLightCount++;

	//contador de spotlights, no puede pasar de MAX_SPOT_LIGHTS
	unsigned int spotLightCount = 0;
	//la posicion y la direccion de la linterna se actualizan en el while con la camara
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;

	//spotlight azul fija apuntando hacia abajo
	spotLights[1] = SpotLight(0.2f, 0.4f, 1.0f,
		0.0f, 2.0f,
		3.0f, 4.0f, -7.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.05f, 0.005f,
		20.0f);
	spotLightCount++;
	//para agregar otra luz se pone en el siguiente lugar libre del arreglo y se le suma 1 al contador

	//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---
	//las luces de la escena van aqui; si otra actividad necesita una luz, se pide a quien lleva esta seccion

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	//glm::perspective usa radianes, asi que el 45.0f no son 45 grados, pero asi venia del lab y se ve bien
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	// ================== 4. CICLO PRINCIPAL ==================
	//todo lo que esta dentro del while se repite cada frame hasta que se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//eventos de teclado y mouse
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//matriz de vista de la camara libre; con otras camaras se cambia en su seccion
		glm::mat4 view = camera.calculateViewMatrix();

		//--- camaras y recorridos ---

		//limpiar la pantalla
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//el skybox va primero, antes que todo lo demas, y cada planeta tiene el suyo segun zonaActual, que cambia con Z, X y C
		if (zonaActual == 1)
			skyboxXen.DrawSkybox(view, projection, deltaTime);
		else if (zonaActual == 2)
			skyboxPlaneta2.DrawSkybox(view, projection, deltaTime);
		else
			skybox.DrawSkybox(view, projection, deltaTime);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//intensidad especular y brillo del material
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		//linterna pegada a la camara, un poco abajo de los ojos
		//aqui en el while es donde se pueden cambiar cosas de las luces mientras corre el programa
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---
		//aqui se cambian las luces cada frame, antes de mandarlas al shader

		//mandar todas las luces al shader
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		//para dibujar cada objeto siempre se hace lo mismo, se pone la matriz model, el color, la textura y el material, y luego se hace el render
		glm::mat4 model(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f); //con blanco la textura se ve con sus colores normales

		// ---------- objetos opacos ----------

		//piso
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pruebaTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[0]->RenderMeshModel();

		//el cubo se mueve en X con las teclas Y y U, que estan en Window.cpp
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(mainWindow.getmuevex(), -0.5f, -4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pruebaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[2]->RenderMeshModel();

		//el modelo cargado de archivo trae sus propias texturas, por eso aqui no va UseTexture
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(4.0f, -1.0f, -3.0f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Prueba_M.RenderModel();

		//--- personajes ---
		model = glm::mat4(1.0f);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(240.0f, 15.0f, -130.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		capitanFalcon.RenderModel();

		//los pies de los modelos con jerarquia estan en y = 0, por eso se bajan a -1, que es la altura del piso
		//para articularlos se cambian las rotaciones; por ejemplo, rotGordon[2].x = -30.0f; mueve el brazo derecho en el hombro
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		//planta carnivora
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-4.0f, -1.0f, -3.0f));
		DibujaJerarquia(Carnivora_M, padresCarnivora, pivotesCarnivora, rotCarnivora, NUM_PIEZAS_CARNIVORA, model, uniformModel);

		//Gordon
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.0f, -1.0f, -3.0f));
		DibujaJerarquia(Gordon_M, padresGordon, pivotesGordon, rotGordon, NUM_PIEZAS_GORDON, model, uniformModel);

		//Kirby
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.5f, -1.0f, -3.0f));
		DibujaJerarquia(Kirby_M, padresKirby, pivotesKirby, rotKirby, NUM_PIEZAS_KIRBY, model, uniformModel);

		//el headcrab camina en circulo en la superficie de Xen mientras su caminata este prendida con la tecla H
		if (mainWindow.getcaminaHeadcrab())
		{
			anguloHeadcrab += anguloHeadcrabOffset * deltaTime;
			if (anguloHeadcrab > 360.0f)
				anguloHeadcrab -= 360.0f;
			if (pasoAdelante)
			{
				pasoHeadcrab += pasoHeadcrabOffset * deltaTime;
				if (pasoHeadcrab > limitePaso)
					pasoAdelante = false;
			}
			else
			{
				pasoHeadcrab -= pasoHeadcrabOffset * deltaTime;
				if (pasoHeadcrab < -limitePaso)
					pasoAdelante = true;
			}
		}
		//las patas se mueven en diagonal, la delantera derecha junto con la trasera izquierda y al reves,
		//y giran en Y sobre la cadera para ir hacia adelante y hacia atras
		rotHeadcrab[2].y = pasoHeadcrab;
		rotHeadcrab[8].y = pasoHeadcrab;
		rotHeadcrab[4].y = -pasoHeadcrab;
		rotHeadcrab[6].y = -pasoHeadcrab;
		//la garra se levanta un poco mientras su pata va hacia adelante
		rotHeadcrab[3].x = -pasoHeadcrab * 0.5f;
		rotHeadcrab[9].x = -pasoHeadcrab * 0.5f;
		rotHeadcrab[5].x = pasoHeadcrab * 0.5f;
		rotHeadcrab[7].x = pasoHeadcrab * 0.5f;
		//la posicion en el circulo sale del seno y el coseno, y se gira 90 grados mas para que mire hacia donde avanza
		model = glm::mat4(1.0);
		model = glm::translate(model, posXen + centroHeadcrab +
			glm::vec3(radioHeadcrab * sin(anguloHeadcrab * toRadians), 0.0f, radioHeadcrab * cos(anguloHeadcrab * toRadians)));
		model = glm::rotate(model, (anguloHeadcrab + 90.0f) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(Headcrab_M, padresHeadcrab, pivotesHeadcrab, rotHeadcrab, NUM_PIEZAS_HEADCRAB, model, uniformModel);

		//--- planetas: Xen y planeta 2 ---
		//cuando una tecla cambia zonaPlaneta en Window, la camara se vuelve a crear en la otra zona
		if (mainWindow.getzonaPlaneta() != zonaActual)
		{
			zonaActual = mainWindow.getzonaPlaneta();
			if (zonaActual == 1)
				camera = Camera(posXen + glm::vec3(0.0f, 1.5f, 8.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.3f, 0.5f);
			else if (zonaActual == 2)
				camera = Camera(posPlaneta2 + glm::vec3(0.0f, 1.5f, 8.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.3f, 0.5f);
			else
				camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);
		}

		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		//el planeta Xen en el espacio tiene radio 60 y gira despacio sobre Y
		giroPlanetaXen += giroPlanetaXenOffset * deltaTime;
		if (giroPlanetaXen > 360.0f)
			giroPlanetaXen -= 360.0f;
		model = glm::mat4(1.0);
		model = glm::translate(model, posPlanetaXen);
		model = glm::rotate(model, giroPlanetaXen * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(60.0f, 60.0f, 60.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		XenPlaneta_M.RenderModel();

		//el piso base mide 20 por 20 y su textura se repite 10 veces, asi que con escala 10 el piso de Xen queda de 200 por 200
		model = glm::mat4(1.0);
		model = glm::translate(model, posXen);
		model = glm::scale(model, glm::vec3(10.0f, 1.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		xenSueloTexture.UseTexture();
		meshListModel[0]->RenderMeshModel();

		//criaturas de la mas simple a la mas evolucionada, viendo hacia donde llega la camara
		model = glm::mat4(1.0);
		model = glm::translate(model, posXen + glm::vec3(-5.0f, 0.0f, -3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Houndeye_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, posXen + glm::vec3(-1.5f, 0.0f, -4.5f));
		model = glm::rotate(model, 20.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Bullsquid_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, posXen + glm::vec3(2.5f, 0.0f, -4.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Vortigaunt_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, posXen + glm::vec3(6.0f, 0.0f, -3.5f));
		model = glm::rotate(model, -20.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AlienGrunt_M.RenderModel();

		//cristales
		model = glm::mat4(1.0);
		model = glm::translate(model, posXen + glm::vec3(-10.0f, 0.0f, -3.0f));
		model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		XenCristal_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, posXen + glm::vec3(11.0f, 0.0f, 3.0f));
		model = glm::rotate(model, 70.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		XenCristal_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, posXen + glm::vec3(-4.0f, 0.0f, 6.0f));
		model = glm::rotate(model, 150.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		XenCristal_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, posXen + glm::vec3(12.0f, 0.0f, -12.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		XenCristal_M.RenderModel();

		//en las plantas de luz el foco cuelga del tallo, por eso modelaux guarda la matriz antes de escalar el tallo
		for (int i = 0; i < NUM_PLANTAS; i++)
		{
			glm::vec3 pos = posXen + posPlantas[i];
			if (glm::distance(camera.getCameraPosition(), pos) < 4.0f)
			{
				if (alturaPlantas[i] > 0.1f)
					alturaPlantas[i] -= 0.05f * deltaTime;
			}
			else if (alturaPlantas[i] < 1.0f)
			{
				alturaPlantas[i] += 0.01f * deltaTime;
			}
			model = glm::mat4(1.0);
			model = glm::translate(model, pos);
			glm::mat4 modelaux = model;
			model = glm::scale(model, glm::vec3(1.0f, alturaPlantas[i], 1.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			XenTallo_M.RenderModel();
			//el foco baja lo mismo que la punta del tallo
			model = modelaux;
			model = glm::translate(model, glm::vec3(0.0f, altoTallo * (alturaPlantas[i] - 1.0f), 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			XenFoco_M.RenderModel();
		}

		//cada isla flotante sube y baja desfasada 120 grados de la anterior
		anguloIslas += anguloIslasOffset * deltaTime;
		if (anguloIslas > 360.0f)
			anguloIslas -= 360.0f;
		for (int i = 0; i < 3; i++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, posXen + posIslas[i] +
				glm::vec3(0.0f, 0.8f * sin((anguloIslas + i * 120.0f) * toRadians), 0.0f));
			model = glm::rotate(model, i * 50.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(escalaIslas[i], escalaIslas[i], escalaIslas[i]));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			XenIsla_M.RenderModel();
		}

		//Pop Star mide como 1.8 de la punta al centro, asi que con escala 40 cada punta sale unas 70 unidades
		//gira sobre Z porque la estrella esta parada viendo hacia Z positivo
		giroPlaneta2 += giroPlaneta2Offset * deltaTime;
		if (giroPlaneta2 > 360.0f)
			giroPlaneta2 -= 360.0f;
		model = glm::mat4(1.0);
		model = glm::translate(model, posPlaneta2Espacio);
		model = glm::rotate(model, giroPlaneta2 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(40.0f, 40.0f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Planeta2Espacio_M.RenderModel();

		//el piso del planeta 2 mide lo mismo que el de Xen
		model = glm::mat4(1.0);
		model = glm::translate(model, posPlaneta2);
		model = glm::scale(model, glm::vec3(10.0f, 1.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		planeta2SueloTexture.UseTexture();
		meshListModel[0]->RenderMeshModel();

		for (int i = 0; i < NUM_COLINAS; i++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, posPlaneta2 + posColinas[i]);
			model = glm::rotate(model, giroColinas[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(escalaColinas[i], escalaColinas[i], escalaColinas[i]));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Planeta2Colina_M.RenderModel();
		}

		for (int i = 0; i < NUM_ARBOLES; i++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, posPlaneta2 + posArboles[i]);
			model = glm::rotate(model, giroArboles[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(escalaArboles[i], escalaArboles[i], escalaArboles[i]));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Planeta2Arbol_M.RenderModel();
		}

		for (int i = 0; i < NUM_NUBES; i++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, posPlaneta2 + posNubes[i]);
			model = glm::scale(model, glm::vec3(escalaNubes[i], escalaNubes[i], escalaNubes[i]));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			if (tipoNubes[i] == 1)
				Planeta2Nube1_M.RenderModel();
			else
				Planeta2Nube2_M.RenderModel();
		}

		//la punta de la warp star ve hacia Z positivo, se gira un poco para que se vean los propulsores
		model = glm::mat4(1.0);
		model = glm::translate(model, posPlaneta2 + posWarpStar);
		model = glm::rotate(model, -20.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WarpStar_M.RenderModel();

		//--- pista de carreras y naves ---
		//pista de carreras
		model = glm::mat4(1.0f);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(240.0f, 10.0f, -130.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pistaM.RenderModel();

		//nave 1 de la pista
		model = glm::mat4(1.0f);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(270.0f, 40.0f, -130.0f));
		model = glm::rotate(model, 30.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		nave1Race.RenderModel();

		//--- satelites y cuerpos espaciales ---
		//satelite
		model = glm::mat4(1.0f);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(340.0f, 50.0f, -530.0f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sateliteRace.RenderModel();

		//--- plataforma de lanzamiento y cohete ---

		//--- estacion espacial y naves ---

		//--- avatar ---

		//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---
		//postes o lamparas de las luminarias

		// ---------- objetos con transparencia ----------
		//van al final, despues de todos los opacos, si no se ven mal
		//cada actividad dibuja aqui sus transparentes, entre glEnable(GL_BLEND) y glDisable(GL_BLEND)

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.5f, 0.0f, -6.5f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		//el blending sirve para la transparencia o la traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		vegetacionTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[1]->RenderMeshModel();
		glDisable(GL_BLEND);

		//--- personajes ---

		//--- planetas: Xen y planeta 2 ---

		//--- pista de carreras y naves ---

		//--- satelites y cuerpos espaciales ---

		//--- plataforma de lanzamiento y cohete ---

		//--- estacion espacial y naves ---

		//--- avatar ---

		//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
