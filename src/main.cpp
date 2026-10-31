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
//zona donde esta el usuario, 0 en el espacio, 1 en Xen, 2 en el planeta 2 y 3 en Verdania; se compara con la de Window para saber cuando cambio
int zonaActual = 0;
Skybox skyboxXen;
Skybox skyboxPlaneta2;
Texture planeta2SueloTexture;
Texture xenSueloTexture;

//en el espacio los tres planetas estan en un circulo de radio 700 alrededor de la nave nodriza, separados 120 grados
//Xen va hacia Z negativo, el planeta 2 abajo a la izquierda y Verdania abajo a la derecha, vistos desde arriba
//las X salen de 700 por el seno de 120 grados, que da 606, y las Z de 700 por el coseno, que da 350
//el planeta Xen que se ve desde el espacio es una esfera que gira sobre su eje
Model XenPlaneta_M;
glm::vec3 posPlanetaXen = glm::vec3(0.0f, 80.0f, -700.0f);
float giroPlanetaXen = 0.0f;
float giroPlanetaXenOffset = 0.05f;

//el planeta 2 es Pop Star, el planeta de Kirby, que tiene forma de estrella; se ve de frente y gira como rehilete
Model Planeta2Espacio_M;
glm::vec3 posPlaneta2Espacio = glm::vec3(-606.0f, 80.0f, 350.0f);
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

//--- nave de Among Us y crewmates ---
//nave de Among Us con la puerta abierta y 6 crewmates que se salieron y van flotando en el espacio
//la nave se hizo en Blender, mide como 12 de largo y tiene el frente hacia Z positivo y la puerta atras, hacia Z negativo
//el crewmate es descargado y sus creditos estan en el LICENCIA.txt de su carpeta; mide 1 de alto y esta separado
//en cuerpo y visor, el cuerpo tiene la textura en gris para pintarlo de cualquier color con el uniform color
//y el visor se dibuja con blanco para que no se pinte
Model AmongUsNave_M;
Model CrewmateCuerpo_M;
Model CrewmateVisor_M;

//el grupo esta enfrente de donde empieza la camara y se gira para que la puerta quede viendo hacia ella
glm::vec3 posNaveAmongUs = glm::vec3(10.0f, -2.0f, -48.0f);
float giroNaveAmongUs = 160.0f;

//la nave y los crewmates van hacia adelante y hacia atras con el seno de anguloAmongUs, que avanza cada frame
//como el de las islas de Xen; cada crewmate va desfasado 60 grados del anterior para que no se muevan todos juntos
float anguloAmongUs = 0.0f;
float anguloAmongUsOffset = 0.4f;
float avanceNave = 0.5f;      //lo mas que se mueve la nave hacia adelante o hacia atras
float avanceCrewmates = 0.4f; //lo mismo para los crewmates, que se acercan y se alejan de la puerta

//el lugar de cada crewmate se mide desde el centro de la nave; el primero apenas va saliendo por la puerta y los
//demas estan cada vez mas lejos, girados en Y hacia distintos lados y ladeados en Z como si dieran vueltas
//la puerta esta en z = -4.5 y entre los motores, que estan en x = 2.3 y x = -2.3, por eso los crewmates cercanos
//van mas al centro en X que los que ya pasaron la punta del fuego
const int NUM_CREWMATES_NAVE = 6;
glm::vec3 posCrewmates[NUM_CREWMATES_NAVE] = {
	glm::vec3(0.0f, -0.9f, -5.4f),
	glm::vec3(-0.8f, 0.3f, -6.6f),
	glm::vec3(0.9f, -1.3f, -7.6f),
	glm::vec3(-0.4f, 0.9f, -8.6f),
	glm::vec3(-1.9f, -0.4f, -9.7f),
	glm::vec3(1.1f, 0.7f, -10.6f) };
float giroYCrewmates[NUM_CREWMATES_NAVE] = { 180.0f, 150.0f, 220.0f, 110.0f, 190.0f, 250.0f };
float ladeoCrewmates[NUM_CREWMATES_NAVE] = { 0.0f, 25.0f, -40.0f, 150.0f, 70.0f, -110.0f };
//rojo, azul, verde, rosa, naranja y amarillo, como en el juego
glm::vec3 colorCrewmates[NUM_CREWMATES_NAVE] = {
	glm::vec3(0.77f, 0.07f, 0.07f),
	glm::vec3(0.07f, 0.18f, 0.82f),
	glm::vec3(0.07f, 0.50f, 0.18f),
	glm::vec3(0.93f, 0.33f, 0.73f),
	glm::vec3(0.94f, 0.49f, 0.05f),
	glm::vec3(0.96f, 0.96f, 0.34f) };

//hay un crewmate escondido en cada planeta, en Xen arriba de la isla flotante mas baja, en el planeta 2 atras de la
//colina del fondo a la izquierda y en Verdania atras de la casa de crazy dave
//su lugar se mide desde el centro de la superficie de su planeta; el de Xen esta en la altura de la isla mas lo que
//sale la isla hacia arriba, que con su escala de 3 es 0.6
const int NUM_CREWMATES_ESCONDIDOS = 3;
glm::vec3 posEscondidos[NUM_CREWMATES_ESCONDIDOS] = {
	glm::vec3(-20.0f, 12.6f, -30.0f),
	glm::vec3(-35.0f, 0.0f, -97.0f),
	glm::vec3(2.5f, 0.0f, -39.5f) };
float giroEscondidos[NUM_CREWMATES_ESCONDIDOS] = { 30.0f, 0.0f, 200.0f };
//blanco, morado y cian
glm::vec3 colorEscondidos[NUM_CREWMATES_ESCONDIDOS] = {
	glm::vec3(0.84f, 0.88f, 0.94f),
	glm::vec3(0.42f, 0.18f, 0.73f),
	glm::vec3(0.22f, 0.95f, 0.86f) };

//--- planeta 3: Verdania ---
//la superficie de Verdania es otro piso aparte y lejos, igual que Xen y el planeta 2, pero hacia Z positivo
glm::vec3 posVerdania = glm::vec3(0.0f, 0.0f, 3000.0f);
Skybox skyboxVerdania;
Texture verdaniaPastoTexture;

//todas las plantas de Verdania son modelos descargados y separados en piezas por sus huesos
//los creditos estan en el LICENCIA.txt de cada una y los pivotes salen del piezas.txt de cada carpeta
//cada planta por piezas tiene los mismos arreglos que la carnivora, uno con el nombre del .obj de cada pieza,
//otro con el indice de la pieza de la que cuelga cada una (la raiz lleva -1 y el padre siempre va antes que sus hijos),
//otro con el pivote, que es el punto de la articulacion donde gira la pieza, y otro con los grados que gira
//en x, y, z, que se cambian en el while para animarlas
//anguloVerdania marca el tiempo de todas las animaciones de Verdania y avanza cada frame mientras la tecla V
//las tenga prendidas, igual que el angulo de las islas de Xen; cada planta usa el seno de ese angulo
float anguloVerdania = 0.0f;
float anguloVerdaniaOffset = 2.0f;

//el peashooter mide como 1.5 de alto
const int NUM_PIEZAS_PEASHOOTER = 12;
Model Peashooter_M[NUM_PIEZAS_PEASHOOTER];
const char* piezasPeashooter[NUM_PIEZAS_PEASHOOTER] = {
	"base", "tallo1", "tallo2", "cabeza", "pelo_izq", "pelo_der", "brazo_izq", "brazo_der", "hoja_del_izq", "hoja_del_der", "hoja_tras_izq", "hoja_tras_der" };
int padresPeashooter[NUM_PIEZAS_PEASHOOTER] = {
	-1, 0, 1, 2, 3, 3, 2, 2, 0, 0, 0, 0 };
glm::vec3 pivotesPeashooter[NUM_PIEZAS_PEASHOOTER] = {
	glm::vec3(0.0000f, 0.0000f, 0.0000f),       //base
	glm::vec3(0.0000f, 0.2630f, 0.2242f),       //tallo1
	glm::vec3(0.0000f, 0.6249f, 0.1036f),       //tallo2
	glm::vec3(0.0000f, 0.8897f, 0.0169f),       //cabeza
	glm::vec3(0.0581f, 1.3543f, -0.2124f),      //pelo_izq
	glm::vec3(-0.0751f, 1.3665f, -0.1937f),     //pelo_der
	glm::vec3(0.0583f, 0.6875f, 0.0730f),       //brazo_izq
	glm::vec3(-0.0583f, 0.6875f, 0.0730f),      //brazo_der
	glm::vec3(0.0622f, 0.2206f, 0.2834f),       //hoja_del_izq
	glm::vec3(-0.0622f, 0.2206f, 0.2834f),      //hoja_del_der
	glm::vec3(0.0622f, 0.2206f, 0.1582f),       //hoja_tras_izq
	glm::vec3(-0.0622f, 0.2206f, 0.1582f)       //hoja_tras_der
};
glm::vec3 rotPeashooter[NUM_PIEZAS_PEASHOOTER];
//el peashooter camina en circulo como el headcrab; se prende y se apaga con V junto con las demas plantas
glm::vec3 centroPeashooter = glm::vec3(0.0f, 0.0f, -2.0f); //el centro del circulo se mide desde posVerdania
float radioPeashooter = 2.5f;
float anguloPeashooter = 0.0f;
float anguloPeashooterOffset = 0.4f;

//crazy dave mide como 2.3 de alto contando la olla
const int NUM_PIEZAS_CRAZYDAVE = 17;
Model CrazyDave_M[NUM_PIEZAS_CRAZYDAVE];
const char* piezasCrazyDave[NUM_PIEZAS_CRAZYDAVE] = {
	"cadera", "torso", "cabeza", "mandibula", "olla", "brazo_der", "antebrazo_der", "mano_der", "brazo_izq", "antebrazo_izq", "mano_izq", "muslo_der", "pierna_der", "pie_der", "muslo_izq", "pierna_izq", "pie_izq" };
int padresCrazyDave[NUM_PIEZAS_CRAZYDAVE] = {
	-1, 0, 1, 2, 2, 1, 5, 6, 1, 8, 9, 0, 11, 12, 0, 14, 15 };
glm::vec3 pivotesCrazyDave[NUM_PIEZAS_CRAZYDAVE] = {
	glm::vec3(0.0000f, 0.0000f, 0.0000f),       //cadera
	glm::vec3(0.0000f, 1.1078f, -0.0846f),      //torso
	glm::vec3(-0.0001f, 1.7563f, -0.0820f),     //cabeza
	glm::vec3(-0.0001f, 1.9597f, -0.0165f),     //mandibula
	glm::vec3(-0.0001f, 2.0927f, -0.0618f),     //olla
	glm::vec3(-0.3109f, 1.6046f, -0.1178f),     //brazo_der
	glm::vec3(-0.6130f, 1.3205f, -0.1140f),     //antebrazo_der
	glm::vec3(-0.8265f, 1.1239f, 0.0458f),      //mano_der
	glm::vec3(0.3109f, 1.6046f, -0.1178f),      //brazo_izq
	glm::vec3(0.6130f, 1.3205f, -0.1140f),      //antebrazo_izq
	glm::vec3(0.8265f, 1.1239f, 0.0458f),       //mano_izq
	glm::vec3(-0.1930f, 0.8930f, -0.0748f),     //muslo_der
	glm::vec3(-0.1930f, 0.5450f, -0.0488f),     //pierna_der
	glm::vec3(-0.1927f, 0.1641f, -0.0969f),     //pie_der
	glm::vec3(0.1930f, 0.8930f, -0.0748f),      //muslo_izq
	glm::vec3(0.1930f, 0.5450f, -0.0488f),      //pierna_izq
	glm::vec3(0.1927f, 0.1641f, -0.0969f)       //pie_izq
};
glm::vec3 rotCrazyDave[NUM_PIEZAS_CRAZYDAVE];
//crazy dave va y viene en linea recta en X entre -limiteDave y limiteDave, como los paneles del satelite de la practica 9
float posDave = 0.0f;
float posDaveOffset = 0.02f;
float limiteDave = 4.0f;
bool daveAdelante = true;
//hacia donde mira crazy dave, en grados alrededor de Y, vale 90 cuando camina hacia X positivo y -90 hacia X negativo
//al cambiar de direccion se va girando de rumboDaveOffset en rumboDaveOffset para que la vuelta no sea de golpe
float rumboDave = 90.0f;
float rumboDaveOffset = 6.0f;

//desde el espacio Verdania se ve como la cabeza del peashooter en grande, lisa y con textura de planeta
//peashooter_planeta.obj es la cabeza sin cuello, sin cejas y redonda por abajo; su textura es la de Verdania
Model PlanetaVerdania_M;
glm::vec3 posPlanetaVerdania = glm::vec3(606.0f, 80.0f, 350.0f); //en el circulo de los planetas, ver posPlanetaXen
//centro de la cabeza en las coordenadas del modelo; ahi se gira y se escala para que no gire alrededor de los pies
glm::vec3 centroCabezaPea = glm::vec3(0.0f, 1.15f, 0.05f);
float giroPlanetaVerdania = 0.0f;
float giroPlanetaVerdaniaOffset = 0.05f;

//NPCs de Verdania, los tres peas en maceta miden como 1.4 de alto y el cactus como 1.45
//ice pea, un pea de hielo en maceta
const int NUM_PIEZAS_ICEPEA = 8;
Model IcePea_M[NUM_PIEZAS_ICEPEA];
const char* piezasIcePea[NUM_PIEZAS_ICEPEA] = {
	"base", "tallo", "cabeza", "boca", "hoja_izq", "hoja_frente", "hoja_der", "hoja_atras" };
int padresIcePea[NUM_PIEZAS_ICEPEA] = {
	-1, 0, 1, 2, 0, 0, 0, 0 };
glm::vec3 pivotesIcePea[NUM_PIEZAS_ICEPEA] = {
	glm::vec3(0.0000f, 0.0000f, 0.0000f),       //base
	glm::vec3(0.0000f, 0.5482f, 0.0449f),       //tallo
	glm::vec3(0.0000f, 0.9346f, -0.0249f),      //cabeza
	glm::vec3(0.0000f, 1.1496f, 0.3915f),       //boca
	glm::vec3(0.0361f, 0.5212f, 0.0572f),       //hoja_izq
	glm::vec3(0.0005f, 0.5212f, 0.0882f),       //hoja_frente
	glm::vec3(-0.0299f, 0.5212f, 0.0563f),      //hoja_der
	glm::vec3(0.0000f, 0.5212f, 0.0221f)        //hoja_atras
};
glm::vec3 rotIcePea[NUM_PIEZAS_ICEPEA];

//gatling pea, un pea con casco y canones de metralleta; los canones son una pieza aparte para que puedan girar
const int NUM_PIEZAS_GATLINGPEA = 9;
Model GatlingPea_M[NUM_PIEZAS_GATLINGPEA];
const char* piezasGatlingPea[NUM_PIEZAS_GATLINGPEA] = {
	"base", "tallo", "cabeza", "boca", "canones", "hoja_izq", "hoja_frente", "hoja_der", "hoja_atras" };
int padresGatlingPea[NUM_PIEZAS_GATLINGPEA] = {
	-1, 0, 1, 2, 3, 0, 0, 0, 0 };
glm::vec3 pivotesGatlingPea[NUM_PIEZAS_GATLINGPEA] = {
	glm::vec3(0.0000f, 0.0000f, 0.0000f),       //base
	glm::vec3(0.0000f, 0.5482f, -0.0555f),      //tallo
	glm::vec3(0.0000f, 0.9346f, -0.1253f),      //cabeza
	glm::vec3(0.0000f, 1.1496f, 0.2911f),       //boca
	glm::vec3(0.0000f, 1.1496f, 0.2911f),       //canones
	glm::vec3(0.0361f, 0.5212f, -0.0433f),      //hoja_izq
	glm::vec3(0.0005f, 0.5212f, -0.0123f),      //hoja_frente
	glm::vec3(-0.0299f, 0.5212f, -0.0441f),     //hoja_der
	glm::vec3(0.0000f, 0.5212f, -0.0783f)       //hoja_atras
};
glm::vec3 rotGatlingPea[NUM_PIEZAS_GATLINGPEA];
//angulo de los canones de la gatling pea; avanza mientras V este prendida
float giroCanones = 0.0f;
float giroCanonesOffset = 8.0f;

//fire pea, un pea de fuego en maceta
const int NUM_PIEZAS_FIREPEA = 8;
Model FirePea_M[NUM_PIEZAS_FIREPEA];
const char* piezasFirePea[NUM_PIEZAS_FIREPEA] = {
	"base", "tallo", "cabeza", "boca", "hoja_izq", "hoja_frente", "hoja_der", "hoja_atras" };
int padresFirePea[NUM_PIEZAS_FIREPEA] = {
	-1, 0, 1, 2, 0, 0, 0, 0 };
glm::vec3 pivotesFirePea[NUM_PIEZAS_FIREPEA] = {
	glm::vec3(0.0000f, 0.0000f, 0.0000f),       //base
	glm::vec3(0.0000f, 0.5357f, -0.0244f),      //tallo
	glm::vec3(0.0000f, 0.9221f, -0.0942f),      //cabeza
	glm::vec3(0.0000f, 1.1371f, 0.3222f),       //boca
	glm::vec3(0.0361f, 0.5087f, -0.0121f),      //hoja_izq
	glm::vec3(0.0005f, 0.5087f, 0.0188f),       //hoja_frente
	glm::vec3(-0.0299f, 0.5087f, -0.0130f),     //hoja_der
	glm::vec3(0.0000f, 0.5087f, -0.0472f)       //hoja_atras
};
glm::vec3 rotFirePea[NUM_PIEZAS_FIREPEA];

//cactus, con un tallo de dos partes y un brazo de cada lado
const int NUM_PIEZAS_CACTUS = 6;
Model Cactus_M[NUM_PIEZAS_CACTUS];
const char* piezasCactus[NUM_PIEZAS_CACTUS] = {
	"base", "tallo1", "tallo2", "cabeza", "brazo_izq", "brazo_der" };
int padresCactus[NUM_PIEZAS_CACTUS] = {
	-1, 0, 1, 2, 2, 2 };
glm::vec3 pivotesCactus[NUM_PIEZAS_CACTUS] = {
	glm::vec3(0.0000f, 0.0000f, 0.0000f),       //base
	glm::vec3(-0.0128f, 0.2208f, -0.0435f),     //tallo1
	glm::vec3(-0.0128f, 0.5095f, -0.0425f),     //tallo2
	glm::vec3(-0.0128f, 0.9318f, -0.0415f),     //cabeza
	glm::vec3(0.2003f, 0.6514f, -0.0490f),      //brazo_izq
	glm::vec3(-0.2346f, 0.5808f, -0.0527f)      //brazo_der
};
glm::vec3 rotCactus[NUM_PIEZAS_CACTUS];

//plantas de adorno de Verdania, cada una con una animacion sencilla
//girasol de Garden Warfare 1 (mide como 1.1 de alto)
const int NUM_PIEZAS_SUNFLOWER = 12;
Model Sunflower_M[NUM_PIEZAS_SUNFLOWER];
const char* piezasSunflower[NUM_PIEZAS_SUNFLOWER] = {
	"base", "tallo1", "tallo2", "cabeza", "ala_izq", "ala_der", "brazo_izq", "brazo_der", "hoja_del_izq", "hoja_del_der", "hoja_tras_izq", "hoja_tras_der" };
int padresSunflower[NUM_PIEZAS_SUNFLOWER] = {
	-1, 0, 1, 2, 2, 2, 2, 2, 0, 0, 0, 0 };
glm::vec3 pivotesSunflower[NUM_PIEZAS_SUNFLOWER] = {
	glm::vec3(0.0000f, 0.0000f, 0.0000f),       //base
	glm::vec3(-0.0018f, 0.1895f, 0.0845f),      //tallo1
	glm::vec3(-0.0018f, 0.4573f, -0.0415f),     //tallo2
	glm::vec3(-0.0008f, 0.7407f, 0.0224f),      //cabeza
	glm::vec3(0.0328f, 0.5654f, -0.0662f),      //ala_izq
	glm::vec3(-0.0364f, 0.5654f, -0.0662f),     //ala_der
	glm::vec3(0.0332f, 0.4957f, -0.0525f),      //brazo_izq
	glm::vec3(-0.0367f, 0.4957f, -0.0525f),     //brazo_der
	glm::vec3(0.0355f, 0.1462f, 0.1199f),       //hoja_del_izq
	glm::vec3(-0.0390f, 0.1462f, 0.1199f),      //hoja_del_der
	glm::vec3(0.0355f, 0.1462f, 0.0450f),       //hoja_tras_izq
	glm::vec3(-0.0390f, 0.1462f, 0.0450f)       //hoja_tras_der
};
glm::vec3 rotSunflower[NUM_PIEZAS_SUNFLOWER];

//girasol de Garden Warfare 2 (mide como 1.9 de alto)
const int NUM_PIEZAS_SUNFLOWER2 = 12;
Model Sunflower2_M[NUM_PIEZAS_SUNFLOWER2];
const char* piezasSunflower2[NUM_PIEZAS_SUNFLOWER2] = {
	"base", "tallo1", "tallo2", "cabeza", "ala_izq", "ala_der", "brazo_izq", "brazo_der", "hoja_del_izq", "hoja_del_der", "hoja_tras_izq", "hoja_tras_der" };
int padresSunflower2[NUM_PIEZAS_SUNFLOWER2] = {
	-1, 0, 1, 2, 2, 2, 2, 2, 0, 0, 0, 0 };
glm::vec3 pivotesSunflower2[NUM_PIEZAS_SUNFLOWER2] = {
	glm::vec3(0.0000f, 0.0000f, 0.0000f),       //base
	glm::vec3(-0.0029f, 0.3168f, 0.1419f),      //tallo1
	glm::vec3(-0.0029f, 0.7646f, -0.0687f),     //tallo2
	glm::vec3(-0.0014f, 1.2384f, 0.0380f),      //cabeza
	glm::vec3(0.0549f, 0.9454f, -0.1100f),      //ala_izq
	glm::vec3(-0.0608f, 0.9454f, -0.1100f),     //ala_der
	glm::vec3(0.0556f, 0.8288f, -0.0871f),      //brazo_izq
	glm::vec3(-0.0614f, 0.8288f, -0.0871f),     //brazo_der
	glm::vec3(0.0593f, 0.2445f, 0.2011f),       //hoja_del_izq
	glm::vec3(-0.0652f, 0.2445f, 0.2011f),      //hoja_del_der
	glm::vec3(0.0593f, 0.2445f, 0.0759f),       //hoja_tras_izq
	glm::vec3(-0.0652f, 0.2445f, 0.0759f)       //hoja_tras_der
};
glm::vec3 rotSunflower2[NUM_PIEZAS_SUNFLOWER2];

//girasol de peluche (stuffy flower) de Garden Warfare 2, tiene las mismas piezas que el girasol
const int NUM_PIEZAS_STUFFYFLOWER = 12;
Model StuffyFlower_M[NUM_PIEZAS_STUFFYFLOWER];
const char* piezasStuffyFlower[NUM_PIEZAS_STUFFYFLOWER] = {
	"base", "tallo1", "tallo2", "cabeza", "ala_izq", "ala_der", "brazo_izq", "brazo_der", "hoja_del_izq", "hoja_del_der", "hoja_tras_izq", "hoja_tras_der" };
int padresStuffyFlower[NUM_PIEZAS_STUFFYFLOWER] = {
	-1, 0, 1, 2, 2, 2, 2, 2, 0, 0, 0, 0 };
glm::vec3 pivotesStuffyFlower[NUM_PIEZAS_STUFFYFLOWER] = {
	glm::vec3(0.0000f, 0.0000f, 0.0000f),       //base
	glm::vec3(-0.0037f, 0.3196f, 0.1353f),      //tallo1
	glm::vec3(-0.0037f, 0.7674f, -0.0753f),     //tallo2
	glm::vec3(-0.0021f, 1.2413f, 0.0315f),      //cabeza
	glm::vec3(0.0542f, 0.9482f, -0.1166f),      //ala_izq
	glm::vec3(-0.0615f, 0.9482f, -0.1166f),     //ala_der
	glm::vec3(0.0548f, 0.8316f, -0.0937f),      //brazo_izq
	glm::vec3(-0.0622f, 0.8316f, -0.0937f),     //brazo_der
	glm::vec3(0.0586f, 0.2473f, 0.1945f),       //hoja_del_izq
	glm::vec3(-0.0659f, 0.2473f, 0.1945f),      //hoja_del_der
	glm::vec3(0.0586f, 0.2473f, 0.0693f),       //hoja_tras_izq
	glm::vec3(-0.0659f, 0.2473f, 0.0693f)       //hoja_tras_der
};
glm::vec3 rotStuffyFlower[NUM_PIEZAS_STUFFYFLOWER];

//hypno-shroom, un hongo con tallo, sombrero y boca
const int NUM_PIEZAS_HYPNOSHROOM = 4;
Model HypnoShroom_M[NUM_PIEZAS_HYPNOSHROOM];
const char* piezasHypnoShroom[NUM_PIEZAS_HYPNOSHROOM] = {
	"base", "tallo", "sombrero", "boca" };
int padresHypnoShroom[NUM_PIEZAS_HYPNOSHROOM] = {
	-1, 0, 1, 1 };
glm::vec3 pivotesHypnoShroom[NUM_PIEZAS_HYPNOSHROOM] = {
	glm::vec3(0.0000f, 0.0000f, 0.0000f),       //base
	glm::vec3(-0.0064f, 0.1996f, 0.0044f),      //tallo
	glm::vec3(-0.0157f, 0.5987f, 0.0004f),      //sombrero
	glm::vec3(-0.0024f, 0.1765f, 0.1307f)       //boca
};
glm::vec3 rotHypnoShroom[NUM_PIEZAS_HYPNOSHROOM];

//laser bean, un frijol en maceta; la maceta es la raiz y el cuerpo cuelga de ella
const int NUM_PIEZAS_LASERBEAN = 3;
Model LaserBean_M[NUM_PIEZAS_LASERBEAN];
const char* piezasLaserBean[NUM_PIEZAS_LASERBEAN] = {
	"maceta", "cuerpo", "pelo" };
int padresLaserBean[NUM_PIEZAS_LASERBEAN] = {
	-1, 0, 1 };
glm::vec3 pivotesLaserBean[NUM_PIEZAS_LASERBEAN] = {
	glm::vec3(0.0000f, 0.3107f, 0.0000f),       //maceta
	glm::vec3(0.0000f, 0.5511f, 0.2324f),       //cuerpo
	glm::vec3(0.0000f, 1.1489f, -0.1837f)       //pelo
};
glm::vec3 rotLaserBean[NUM_PIEZAS_LASERBEAN];

//potato mine, una papa enterrada; la tierra es la raiz, el cuerpo cuelga de ella y la antena del cuerpo
const int NUM_PIEZAS_POTATOMINE = 3;
Model PotatoMine_M[NUM_PIEZAS_POTATOMINE];
const char* piezasPotatoMine[NUM_PIEZAS_POTATOMINE] = {
	"tierra", "cuerpo", "antena" };
int padresPotatoMine[NUM_PIEZAS_POTATOMINE] = {
	-1, 0, 1 };
glm::vec3 pivotesPotatoMine[NUM_PIEZAS_POTATOMINE] = {
	glm::vec3(-0.0042f, 0.1899f, 0.0095f),      //tierra
	glm::vec3(-0.0042f, 0.1899f, 0.0095f),      //cuerpo
	glm::vec3(-0.0042f, 0.4880f, 0.0104f)       //antena
};
glm::vec3 rotPotatoMine[NUM_PIEZAS_POTATOMINE];

//torchwood, un tronco con cabeza, boca y brazos que mide como 2.7 de alto
const int NUM_PIEZAS_TORCHWOOD = 9;
Model Torchwood_M[NUM_PIEZAS_TORCHWOOD];
const char* piezasTorchwood[NUM_PIEZAS_TORCHWOOD] = {
	"base", "tronco", "cabeza", "fuego", "mandibula", "brazo_izq", "antebrazo_izq", "brazo_der", "antebrazo_der" };
int padresTorchwood[NUM_PIEZAS_TORCHWOOD] = {
	-1, 0, 1, 2, 2, 2, 5, 2, 7 };
glm::vec3 pivotesTorchwood[NUM_PIEZAS_TORCHWOOD] = {
	glm::vec3(0.0000f, 0.0000f, 0.0000f),       //base
	glm::vec3(0.0202f, 0.8698f, -0.1053f),      //tronco
	glm::vec3(0.0185f, 1.2176f, -0.1224f),      //cabeza
	glm::vec3(0.0133f, 2.1441f, -0.1968f),      //fuego
	glm::vec3(0.0121f, 1.4448f, 0.2648f),       //mandibula
	glm::vec3(0.5861f, 1.6801f, -0.2093f),      //brazo_izq
	glm::vec3(0.9585f, 1.4009f, -0.2865f),      //antebrazo_izq
	glm::vec3(-0.5941f, 1.7198f, -0.2244f),     //brazo_der
	glm::vec3(-0.9764f, 1.2656f, -0.2501f)      //antebrazo_der
};
glm::vec3 rotTorchwood[NUM_PIEZAS_TORCHWOOD];

//la wall-nut es una sola pieza, por eso es un Model normal
Model WallNut_M;

//jardin de crazy dave con su casa, la calle, cercas, un farol, arboles, arbustos, flores y piedras, son modelos CC0 de Kenney y Quaternius
//es como si crazy dave hubiera llegado a Verdania y armado su jardin donde ya vivian las plantas
//todos estan en Models/JardinDave y los creditos en su LICENCIA.txt
Model JardinCasa_M;
Model JardinCalle_M;
Model JardinCalleFin_M;
Model JardinCerca_M;
Model JardinCercaPuerta_M;
Model JardinFarol_M;
Model JardinArbol1_M;
Model JardinArbol2_M;
Model JardinArbusto_M;
Model JardinArbustoFlores_M;
Model JardinFlores1_M;
Model JardinFlores2_M;
Model JardinRoca_M;
Model JardinCaminoPiedra_M;
Model JardinHongo_M;
Model JardinTrebol_M;
Model JardinHelecho_M;
//los modelos de Kenney, que son la casa, la calle, las cercas y el farol, son muy chicos; una casa mide 1.2 de alto y con escala 6.5 queda de casi 8
float escalaKenney = 6.5f;

//la calle va en linea recta sobre X; cada tramo mide 1 por 1 (6.5 ya escalado) y el ultimo es el final de la calle
const int NUM_TRAMOS_CALLE = 4;
float posCalleX[NUM_TRAMOS_CALLE] = { 19.5f, 26.0f, 32.5f, 39.0f };
float posCalleZ = -27.0f;

//el camino de piedra sigue donde termina la calle, pasa por la puerta de la cerca y entra al jardin
const int NUM_PIEDRAS_CAMINO = 9;
glm::vec3 posCamino[NUM_PIEDRAS_CAMINO] = {
	glm::vec3(12.0f, 0.0f, -26.5f), glm::vec3(10.0f, 0.0f, -25.8f), glm::vec3(8.0f, 0.0f, -24.8f),
	glm::vec3(6.5f, 0.0f, -23.4f), glm::vec3(5.5f, 0.0f, -21.8f), glm::vec3(5.2f, 0.0f, -20.0f),
	glm::vec3(5.4f, 0.0f, -18.2f), glm::vec3(5.9f, 0.0f, -16.4f), glm::vec3(6.6f, 0.0f, -14.7f) };

//la cerca separa el jardin de la casa; la pieza de en medio-derecha es la puerta, por donde pasa el camino
const int NUM_CERCAS_JARDIN = 4;
float posCercaX[NUM_CERCAS_JARDIN] = { -15.6f, -5.2f, 5.2f, 15.6f };
float posCercaZ = -22.0f;
int indicePuerta = 2;
//la puerta mide 1.2 de ancho y las otras cercas 1.6, por eso la puerta se estira en X para que no queden huecos
float estiraPuerta = 1.33f;

//arboles, arbustos, flores y piedras: posicion, tamano y giro de cada uno; el mismo modelo se repite cambiando el giro
//y el tamano para que no se vean todos iguales, como los arboles del planeta 2
const int NUM_ARBOLES_JARDIN = 4;
glm::vec3 posArbolesJardin[NUM_ARBOLES_JARDIN] = {
	glm::vec3(-14.0f, 0.0f, -27.0f), glm::vec3(18.0f, 0.0f, -14.0f), glm::vec3(-20.0f, 0.0f, -4.0f), glm::vec3(16.0f, 0.0f, 7.0f) };
float escalaArbolesJardin[NUM_ARBOLES_JARDIN] = { 1.0f, 0.8f, 0.9f, 0.7f };
float giroArbolesJardin[NUM_ARBOLES_JARDIN] = { 0.0f, 80.0f, 150.0f, 230.0f };

const int NUM_ARBUSTOS_JARDIN = 6;
glm::vec3 posArbustosJardin[NUM_ARBUSTOS_JARDIN] = {
	glm::vec3(-9.0f, 0.0f, -20.0f), glm::vec3(-12.0f, 0.0f, -10.0f), glm::vec3(11.0f, 0.0f, -19.5f),
	glm::vec3(12.0f, 0.0f, -3.0f), glm::vec3(-10.0f, 0.0f, 8.0f), glm::vec3(-16.0f, 0.0f, -18.0f) };
float escalaArbustosJardin[NUM_ARBUSTOS_JARDIN] = { 1.2f, 1.0f, 1.1f, 1.3f, 0.9f, 1.0f };
float giroArbustosJardin[NUM_ARBUSTOS_JARDIN] = { 10.0f, 70.0f, 130.0f, 200.0f, 260.0f, 320.0f };

const int NUM_FLORES_JARDIN = 8;
glm::vec3 posFloresJardin[NUM_FLORES_JARDIN] = {
	glm::vec3(-2.0f, 0.0f, -6.0f), glm::vec3(3.0f, 0.0f, -5.0f), glm::vec3(-8.5f, 0.0f, -3.0f), glm::vec3(9.0f, 0.0f, 1.0f),
	glm::vec3(2.0f, 0.0f, 5.0f), glm::vec3(-5.0f, 0.0f, -15.0f), glm::vec3(8.0f, 0.0f, -13.0f), glm::vec3(-3.0f, 0.0f, 6.0f) };
float escalaFloresJardin[NUM_FLORES_JARDIN] = { 0.5f, 0.45f, 0.55f, 0.5f, 0.4f, 0.6f, 0.5f, 0.45f };

const int NUM_DETALLES_JARDIN = 8;
glm::vec3 posDetallesJardin[NUM_DETALLES_JARDIN] = {
	glm::vec3(-4.0f, 0.0f, 2.0f), glm::vec3(4.0f, 0.0f, 1.0f), glm::vec3(-7.0f, 0.0f, -9.0f), glm::vec3(10.0f, 0.0f, -12.0f),
	glm::vec3(1.0f, 0.0f, -10.0f), glm::vec3(-12.0f, 0.0f, 2.0f), glm::vec3(13.0f, 0.0f, -8.0f), glm::vec3(-6.0f, 0.0f, 11.0f) };
//cada tipo de detalle viene de distinto tamano, el trebol mide 1.1, el helecho 9 de ancho y el hongo 0.45, por eso cada uno lleva su escala
float escalaDetallesJardin[3] = { 0.6f, 0.25f, 2.0f }; //trebol, helecho, hongo

//--- pista de carreras y naves ---

//puntos del centro de la pista, sacados de pista.obj con un script de Blender y en el orden en que avanzan las naves
//cada punto tiene la normal del piso en ese lugar, que dice hacia donde esta arriba para la nave
const int NUM_PUNTOS_PISTA = 148;
glm::vec3 posRutaPista[NUM_PUNTOS_PISTA] = {
	glm::vec3(27.000f, 0.000f, -0.542f),
	glm::vec3(27.101f, 0.053f, -68.762f),
	glm::vec3(27.387f, 0.203f, -137.512f),
	glm::vec3(27.831f, 0.437f, -206.262f),
	glm::vec3(28.406f, 0.739f, -275.012f),
	glm::vec3(29.087f, 1.097f, -343.762f),
	glm::vec3(29.848f, 1.496f, -412.512f),
	glm::vec3(30.661f, 1.923f, -481.262f),
	glm::vec3(31.500f, 2.364f, -550.012f),
	glm::vec3(32.339f, 2.805f, -618.762f),
	glm::vec3(33.152f, 3.232f, -687.512f),
	glm::vec3(33.913f, 3.632f, -756.262f),
	glm::vec3(34.594f, 3.990f, -825.012f),
	glm::vec3(35.170f, 4.292f, -893.762f),
	glm::vec3(35.613f, 4.525f, -962.512f),
	glm::vec3(35.899f, 4.675f, -1031.262f),
	glm::vec3(36.000f, 4.728f, -1100.012f),
	glm::vec3(33.466f, 6.016f, -1151.844f),
	glm::vec3(25.938f, 9.851f, -1202.957f),
	glm::vec3(13.546f, 16.182f, -1252.819f),
	glm::vec3(-3.476f, 24.894f, -1300.717f),
	glm::vec3(-24.894f, 35.876f, -1346.189f),
	glm::vec3(-50.345f, 48.942f, -1388.620f),
	glm::vec3(-79.517f, 63.930f, -1427.642f),
	glm::vec3(-111.948f, 80.600f, -1462.767f),
	glm::vec3(-147.271f, 98.760f, -1493.735f),
	glm::vec3(-184.958f, 118.129f, -1520.205f),
	glm::vec3(-224.617f, 138.501f, -1542.027f),
	glm::vec3(-265.689f, 159.579f, -1559.014f),
	glm::vec3(-307.782f, 181.155f, -1571.128f),
	glm::vec3(-350.337f, 202.932f, -1578.328f),
	glm::vec3(-392.987f, 224.717f, -1580.677f),
	glm::vec3(-435.202f, 246.232f, -1578.269f),
	glm::vec3(-476.658f, 267.305f, -1571.250f),
	glm::vec3(-516.881f, 287.692f, -1559.830f),
	glm::vec3(-555.600f, 307.252f, -1544.218f),
	glm::vec3(-592.412f, 325.781f, -1524.711f),
	glm::vec3(-627.108f, 343.176f, -1501.561f),
	glm::vec3(-659.366f, 359.277f, -1475.127f),
	glm::vec3(-689.041f, 374.018f, -1445.678f),
	glm::vec3(-715.891f, 387.288f, -1413.607f),
	glm::vec3(-739.829f, 399.053f, -1379.180f),
	glm::vec3(-760.693f, 409.247f, -1342.804f),
	glm::vec3(-778.442f, 417.865f, -1304.723f),
	glm::vec3(-792.984f, 424.877f, -1265.334f),
	glm::vec3(-804.312f, 430.303f, -1224.854f),
	glm::vec3(-812.389f, 434.142f, -1183.654f),
	glm::vec3(-817.230f, 436.426f, -1141.918f),
	glm::vec3(-818.837f, 437.179f, -1099.988f),
	glm::vec3(-818.918f, 436.991f, -1037.455f),
	glm::vec3(-819.155f, 436.323f, -974.988f),
	glm::vec3(-819.559f, 435.022f, -912.455f),
	glm::vec3(-820.148f, 432.938f, -849.988f),
	glm::vec3(-820.927f, 429.914f, -787.455f),
	glm::vec3(-821.871f, 425.796f, -724.988f),
	glm::vec3(-822.904f, 420.406f, -662.455f),
	glm::vec3(-823.878f, 413.575f, -599.988f),
	glm::vec3(-824.620f, 405.343f, -537.455f),
	glm::vec3(-824.992f, 396.090f, -474.988f),
	glm::vec3(-824.858f, 386.083f, -412.455f),
	glm::vec3(-824.107f, 375.642f, -349.988f),
	glm::vec3(-822.665f, 365.053f, -287.455f),
	glm::vec3(-820.506f, 354.657f, -224.988f),
	glm::vec3(-817.652f, 344.754f, -162.455f),
	glm::vec3(-814.184f, 335.684f, -99.988f),
	glm::vec3(-809.499f, 327.338f, -37.455f),
	glm::vec3(-803.165f, 319.591f, 25.012f),
	glm::vec3(-795.534f, 312.618f, 87.545f),
	glm::vec3(-787.005f, 306.571f, 150.012f),
	glm::vec3(-777.957f, 301.519f, 212.545f),
	glm::vec3(-768.809f, 297.493f, 275.012f),
	glm::vec3(-759.936f, 294.460f, 337.545f),
	glm::vec3(-751.741f, 292.362f, 400.012f),
	glm::vec3(-745.241f, 291.467f, 462.545f),
	glm::vec3(-740.971f, 291.708f, 525.012f),
	glm::vec3(-738.496f, 292.623f, 587.545f),
	glm::vec3(-737.389f, 293.880f, 650.012f),
	glm::vec3(-737.222f, 295.233f, 712.545f),
	glm::vec3(-737.586f, 296.470f, 775.012f),
	glm::vec3(-738.086f, 297.386f, 837.545f),
	glm::vec3(-738.337f, 297.749f, 900.012f),
	glm::vec3(-736.439f, 297.354f, 940.578f),
	glm::vec3(-730.578f, 296.065f, 981.918f),
	glm::vec3(-721.243f, 293.890f, 1021.170f),
	glm::vec3(-708.438f, 290.739f, 1059.367f),
	glm::vec3(-692.304f, 286.552f, 1096.245f),
	glm::vec3(-672.315f, 281.089f, 1132.650f),
	glm::vec3(-649.890f, 274.645f, 1166.037f),
	glm::vec3(-624.630f, 267.030f, 1197.310f),
	glm::vec3(-596.731f, 258.217f, 1226.208f),
	glm::vec3(-565.386f, 247.845f, 1253.276f),
	glm::vec3(-532.793f, 236.557f, 1276.560f),
	glm::vec3(-498.263f, 224.055f, 1296.705f),
	glm::vec3(-462.072f, 210.360f, 1313.469f),
	glm::vec3(-423.289f, 195.007f, 1326.990f),
	glm::vec3(-384.671f, 179.005f, 1336.203f),
	glm::vec3(-345.377f, 161.957f, 1341.400f),
	glm::vec3(-305.516f, 144.427f, 1342.414f),
	glm::vec3(-263.936f, 126.471f, 1338.938f),
	glm::vec3(-223.700f, 109.378f, 1331.116f),
	glm::vec3(-183.975f, 92.752f, 1318.896f),
	glm::vec3(-145.228f, 76.764f, 1302.326f),
	glm::vec3(-106.751f, 61.108f, 1280.775f),
	glm::vec3(-71.415f, 46.933f, 1255.765f),
	glm::vec3(-38.432f, 33.887f, 1226.924f),
	glm::vec3(-8.214f, 22.112f, 1194.541f),
	glm::vec3(19.672f, 11.420f, 1157.762f),
	glm::vec3(43.126f, 2.581f, 1119.292f),
	glm::vec3(62.901f, -4.552f, 1078.060f),
	glm::vec3(78.309f, -9.834f, 1035.744f),
	glm::vec3(89.868f, -13.952f, 990.206f),
	glm::vec3(96.634f, -16.287f, 945.240f),
	glm::vec3(99.110f, -17.024f, 899.997f),
	glm::vec3(98.456f, -16.834f, 875.974f),
	glm::vec3(96.724f, -16.116f, 847.974f),
	glm::vec3(94.416f, -15.150f, 823.973f),
	glm::vec3(91.507f, -14.298f, 800.134f),
	glm::vec3(88.022f, -12.965f, 775.987f),
	glm::vec3(83.475f, -11.070f, 747.987f),
	glm::vec3(79.300f, -9.331f, 723.987f),
	glm::vec3(75.001f, -7.540f, 699.987f),
	glm::vec3(70.701f, -5.748f, 675.987f),
	glm::vec3(65.852f, -3.727f, 647.987f),
	glm::vec3(61.979f, -2.114f, 623.987f),
	glm::vec3(58.501f, -0.665f, 599.987f),
	glm::vec3(55.541f, 0.569f, 575.987f),
	glm::vec3(52.908f, 1.666f, 547.987f),
	glm::vec3(51.498f, 2.253f, 523.987f),
	glm::vec3(51.001f, 2.460f, 499.987f),
	glm::vec3(50.728f, 2.086f, 474.787f),
	glm::vec3(49.971f, 1.278f, 449.987f),
	glm::vec3(48.778f, 0.460f, 424.787f),
	glm::vec3(47.255f, 0.096f, 399.987f),
	glm::vec3(45.424f, 0.096f, 374.787f),
	glm::vec3(43.410f, 0.096f, 349.987f),
	glm::vec3(41.257f, 0.096f, 325.187f),
	glm::vec3(38.282f, 0.096f, 299.987f),
	glm::vec3(34.448f, 0.096f, 274.787f),
	glm::vec3(30.733f, 0.096f, 249.987f),
	glm::vec3(27.929f, 0.096f, 224.787f),
	glm::vec3(27.001f, 0.096f, 202.787f),
	glm::vec3(27.001f, 0.096f, 174.787f),
	glm::vec3(27.001f, 0.096f, 149.987f),
	glm::vec3(27.001f, 0.096f, 124.787f),
	glm::vec3(27.000f, 0.048f, 100.857f),
	glm::vec3(27.000f, 0.096f, 73.728f),
	glm::vec3(27.000f, 0.096f, 48.928f),
	glm::vec3(27.000f, 0.096f, 23.728f),
};
glm::vec3 normalRutaPista[NUM_PUNTOS_PISTA] = {
	glm::vec3(-0.000f, 1.000f, -0.002f),
	glm::vec3(-0.004f, 1.000f, 0.001f),
	glm::vec3(-0.014f, 1.000f, 0.002f),
	glm::vec3(-0.030f, 1.000f, 0.003f),
	glm::vec3(-0.051f, 0.999f, 0.003f),
	glm::vec3(-0.074f, 0.997f, 0.004f),
	glm::vec3(-0.098f, 0.995f, 0.004f),
	glm::vec3(-0.122f, 0.993f, 0.004f),
	glm::vec3(-0.146f, 0.989f, 0.003f),
	glm::vec3(-0.168f, 0.986f, 0.003f),
	glm::vec3(-0.188f, 0.982f, 0.003f),
	glm::vec3(-0.206f, 0.978f, 0.002f),
	glm::vec3(-0.222f, 0.975f, 0.002f),
	glm::vec3(-0.234f, 0.972f, 0.001f),
	glm::vec3(-0.243f, 0.970f, 0.001f),
	glm::vec3(-0.249f, 0.969f, 0.000f),
	glm::vec3(-0.251f, 0.968f, 0.018f),
	glm::vec3(-0.248f, 0.966f, 0.072f),
	glm::vec3(-0.238f, 0.961f, 0.143f),
	glm::vec3(-0.223f, 0.951f, 0.213f),
	glm::vec3(-0.201f, 0.939f, 0.280f),
	glm::vec3(-0.173f, 0.923f, 0.344f),
	glm::vec3(-0.140f, 0.904f, 0.404f),
	glm::vec3(-0.102f, 0.882f, 0.460f),
	glm::vec3(-0.060f, 0.857f, 0.511f),
	glm::vec3(-0.013f, 0.830f, 0.557f),
	glm::vec3(0.037f, 0.801f, 0.597f),
	glm::vec3(0.090f, 0.771f, 0.631f),
	glm::vec3(0.146f, 0.738f, 0.659f),
	glm::vec3(0.204f, 0.705f, 0.679f),
	glm::vec3(0.263f, 0.671f, 0.694f),
	glm::vec3(0.323f, 0.636f, 0.701f),
	glm::vec3(0.385f, 0.600f, 0.701f),
	glm::vec3(0.443f, 0.567f, 0.695f),
	glm::vec3(0.501f, 0.533f, 0.682f),
	glm::vec3(0.558f, 0.500f, 0.662f),
	glm::vec3(0.613f, 0.469f, 0.636f),
	glm::vec3(0.665f, 0.439f, 0.605f),
	glm::vec3(0.714f, 0.410f, 0.567f),
	glm::vec3(0.759f, 0.384f, 0.525f),
	glm::vec3(0.801f, 0.360f, 0.478f),
	glm::vec3(0.838f, 0.339f, 0.427f),
	glm::vec3(0.871f, 0.320f, 0.372f),
	glm::vec3(0.899f, 0.303f, 0.315f),
	glm::vec3(0.923f, 0.290f, 0.254f),
	glm::vec3(0.941f, 0.280f, 0.192f),
	glm::vec3(0.954f, 0.272f, 0.129f),
	glm::vec3(0.961f, 0.268f, 0.065f),
	glm::vec3(0.975f, 0.223f, 0.016f),
	glm::vec3(0.984f, 0.175f, 0.002f),
	glm::vec3(0.986f, 0.164f, 0.006f),
	glm::vec3(0.989f, 0.148f, 0.010f),
	glm::vec3(0.992f, 0.129f, 0.014f),
	glm::vec3(0.994f, 0.109f, 0.018f),
	glm::vec3(0.996f, 0.091f, 0.021f),
	glm::vec3(0.997f, 0.077f, 0.022f),
	glm::vec3(0.998f, 0.067f, 0.021f),
	glm::vec3(0.998f, 0.064f, 0.018f),
	glm::vec3(0.998f, 0.065f, 0.012f),
	glm::vec3(0.998f, 0.070f, 0.005f),
	glm::vec3(0.997f, 0.078f, -0.004f),
	glm::vec3(0.996f, 0.088f, -0.013f),
	glm::vec3(0.995f, 0.099f, -0.023f),
	glm::vec3(0.993f, 0.112f, -0.033f),
	glm::vec3(0.991f, 0.124f, -0.046f),
	glm::vec3(0.988f, 0.136f, -0.070f),
	glm::vec3(0.985f, 0.146f, -0.093f),
	glm::vec3(0.982f, 0.154f, -0.111f),
	glm::vec3(0.979f, 0.160f, -0.124f),
	glm::vec3(0.978f, 0.162f, -0.131f),
	glm::vec3(0.978f, 0.160f, -0.132f),
	glm::vec3(0.980f, 0.154f, -0.128f),
	glm::vec3(0.983f, 0.144f, -0.113f),
	glm::vec3(0.989f, 0.123f, -0.087f),
	glm::vec3(0.994f, 0.091f, -0.057f),
	glm::vec3(0.998f, 0.052f, -0.033f),
	glm::vec3(1.000f, 0.010f, -0.013f),
	glm::vec3(1.000f, -0.030f, -0.000f),
	glm::vec3(0.998f, -0.064f, 0.006f),
	glm::vec3(0.996f, -0.087f, 0.006f),
	glm::vec3(0.995f, -0.095f, -0.021f),
	glm::vec3(0.991f, -0.091f, -0.094f),
	glm::vec3(0.979f, -0.078f, -0.186f),
	glm::vec3(0.960f, -0.058f, -0.275f),
	glm::vec3(0.933f, -0.032f, -0.358f),
	glm::vec3(0.900f, 0.001f, -0.437f),
	glm::vec3(0.860f, 0.041f, -0.509f),
	glm::vec3(0.814f, 0.085f, -0.574f),
	glm::vec3(0.765f, 0.133f, -0.631f),
	glm::vec3(0.711f, 0.185f, -0.679f),
	glm::vec3(0.654f, 0.240f, -0.717f),
	glm::vec3(0.596f, 0.295f, -0.747f),
	glm::vec3(0.538f, 0.351f, -0.766f),
	glm::vec3(0.480f, 0.407f, -0.777f),
	glm::vec3(0.422f, 0.463f, -0.779f),
	glm::vec3(0.363f, 0.546f, -0.755f),
	glm::vec3(0.314f, 0.592f, -0.742f),
	glm::vec3(0.244f, 0.591f, -0.769f),
	glm::vec3(0.184f, 0.649f, -0.738f),
	glm::vec3(0.129f, 0.702f, -0.701f),
	glm::vec3(0.079f, 0.750f, -0.657f),
	glm::vec3(0.035f, 0.794f, -0.607f),
	glm::vec3(-0.004f, 0.833f, -0.553f),
	glm::vec3(-0.037f, 0.867f, -0.496f),
	glm::vec3(-0.064f, 0.897f, -0.438f),
	glm::vec3(-0.086f, 0.921f, -0.379f),
	glm::vec3(-0.103f, 0.942f, -0.320f),
	glm::vec3(-0.115f, 0.958f, -0.262f),
	glm::vec3(-0.159f, 0.986f, -0.044f),
	glm::vec3(-0.094f, 0.987f, -0.132f),
	glm::vec3(-0.098f, 0.992f, -0.084f),
	glm::vec3(-0.100f, 0.994f, -0.038f),
	glm::vec3(-0.102f, 0.995f, -0.002f),
	glm::vec3(-0.102f, 0.994f, 0.024f),
	glm::vec3(-0.104f, 0.994f, 0.043f),
	glm::vec3(-0.106f, 0.993f, 0.059f),
	glm::vec3(-0.090f, 0.975f, -0.204f),
	glm::vec3(-0.077f, 0.994f, 0.075f),
	glm::vec3(-0.080f, 0.993f, 0.082f),
	glm::vec3(-0.083f, 0.993f, 0.087f),
	glm::vec3(-0.086f, 0.992f, 0.089f),
	glm::vec3(-0.089f, 0.992f, 0.088f),
	glm::vec3(-0.093f, 0.992f, 0.084f),
	glm::vec3(-0.096f, 0.992f, 0.078f),
	glm::vec3(-0.099f, 0.993f, 0.068f),
	glm::vec3(-0.102f, 0.993f, 0.056f),
	glm::vec3(-0.105f, 0.994f, 0.040f),
	glm::vec3(-0.107f, 0.994f, 0.021f),
	glm::vec3(-0.108f, 0.994f, -0.002f),
	glm::vec3(-0.094f, 0.995f, -0.018f),
	glm::vec3(-0.062f, 0.998f, -0.022f),
	glm::vec3(-0.024f, 1.000f, -0.015f),
	glm::vec3(-0.003f, 1.000f, -0.004f),
	glm::vec3(-0.000f, 1.000f, -0.000f),
	glm::vec3(-0.000f, 1.000f, 0.000f),
	glm::vec3(-0.000f, 1.000f, 0.000f),
	glm::vec3(-0.000f, 1.000f, 0.000f),
	glm::vec3(-0.000f, 1.000f, 0.000f),
	glm::vec3(-0.000f, 1.000f, 0.000f),
	glm::vec3(-0.000f, 1.000f, 0.000f),
	glm::vec3(-0.000f, 1.000f, -0.000f),
	glm::vec3(-0.000f, 1.000f, -0.000f),
	glm::vec3(-0.000f, 1.000f, -0.000f),
	glm::vec3(-0.000f, 1.000f, -0.002f),
	glm::vec3(-0.000f, 1.000f, -0.002f),
	glm::vec3(-0.000f, 1.000f, -0.000f),
	glm::vec3(-0.000f, 1.000f, -0.000f),
	glm::vec3(-0.000f, 1.000f, -0.002f),
};

//distancia que hay desde el primer punto hasta cada punto, se llena en InicializarRutaPista
float distAcumuladaPista[NUM_PUNTOS_PISTA + 1];
float longitudTotalPista = 0.0f;
float avanceCarrera = 0.0f; //distancia que lleva recorrida la primera nave
float velocidadCarrera = 2.5f;
const float ESCALA_INTERNA_PISTA = 0.45f; //los puntos se escalan igual que la pista, que lleva un segundo scale de 0.45

//lugar de la pista en el espacio, entre Xen y Verdania; ahi queda la meta y con esto su centro queda como en 340, 0, -280
glm::vec3 posPista = glm::vec3(413.0f, 0.0f, -254.0f);

//matrices de la pista que se guardan en el while para poner encima la meta, los edificios y las naves
glm::mat4 modelauxPista(1.0);
glm::mat4 modelauxNaves(1.0);

//calcula la distancia hasta cada punto de la ruta y lo que mide la vuelta completa
void InicializarRutaPista()
{
	longitudTotalPista = 0.0f;
	distAcumuladaPista[0] = 0.0f;
	for (int i = 0; i < NUM_PUNTOS_PISTA; i++)
	{
		//el ultimo punto se une con el primero porque la pista es un circuito cerrado
		glm::vec3 pActual = posRutaPista[i] * ESCALA_INTERNA_PISTA;
		glm::vec3 pSig = posRutaPista[(i + 1) % NUM_PUNTOS_PISTA] * ESCALA_INTERNA_PISTA;
		longitudTotalPista += glm::distance(pActual, pSig);
		distAcumuladaPista[i + 1] = longitudTotalPista;
	}
}

//regresa la matriz de una nave que va a cierta distancia del inicio, en su carril y flotando sobre el piso
//despues se multiplica por modelauxNaves para que quede en el lugar de la pista
glm::mat4 ObtenerMatrizNavePista(float distancia, float carrilX, float alturaFlotacion)
{
	int n = NUM_PUNTOS_PISTA;

	//la distancia se ajusta para que siempre quede entre 0 y lo que mide una vuelta, aunque sea negativa
	float d = fmod(distancia, longitudTotalPista);
	if (d < 0.0f)
		d += longitudTotalPista;

	//se busca en que tramo de la ruta va la nave
	int idx = 0;
	for (int i = 0; i < n; i++)
	{
		if (d >= distAcumuladaPista[i] && d <= distAcumuladaPista[i + 1])
		{
			idx = i;
			break;
		}
	}

	int idxSig = (idx + 1) % n;
	int idxSig2 = (idx + 2) % n;

	//t dice cuanto lleva recorrido del tramo, vale 0 al inicio y 1 al final
	float longSeg = distAcumuladaPista[idx + 1] - distAcumuladaPista[idx];
	float t = 0.0f;
	if (longSeg > 0.0001f)
		t = (d - distAcumuladaPista[idx]) / longSeg;

	//posicion y normal del piso entre los dos puntos del tramo
	glm::vec3 p0 = posRutaPista[idx] * ESCALA_INTERNA_PISTA;
	glm::vec3 p1 = posRutaPista[idxSig] * ESCALA_INTERNA_PISTA;
	glm::vec3 p2 = posRutaPista[idxSig2] * ESCALA_INTERNA_PISTA;

	glm::vec3 posCentro = glm::mix(p0, p1, t);
	glm::vec3 normalPiso = glm::normalize(glm::mix(normalRutaPista[idx], normalRutaPista[idxSig], t));

	//hacia donde va la nave, se mezcla la direccion de este tramo con la del siguiente para que gire suave en las curvas
	glm::vec3 tangenteActual = glm::normalize(p1 - p0);
	glm::vec3 tangenteSig = glm::normalize(p2 - p1);
	glm::vec3 frente = glm::normalize(glm::mix(tangenteActual, tangenteSig, t));

	//con el producto cruz salen los otros dos ejes de la nave, todos perpendiculares entre si
	glm::vec3 derecha = glm::normalize(glm::cross(frente, normalPiso));
	glm::vec3 arriba = glm::normalize(glm::cross(derecha, frente));

	//la nave se mueve hacia su carril y se levanta sobre el piso
	glm::vec3 posFinal = posCentro + (derecha * carrilX) + (arriba * alturaFlotacion);

	//cada columna de la matriz es un eje de la nave y la ultima es su posicion
	//las naves ven hacia Z negativo, por eso su eje Z va al reves del frente
	glm::mat4 transform(1.0f);
	transform[0] = glm::vec4(-derecha, 0.0f);
	transform[1] = glm::vec4(arriba, 0.0f);
	transform[2] = glm::vec4(-frente, 0.0f);
	transform[3] = glm::vec4(posFinal, 1.0f);

	return transform;
}

Model pistaM;
Model nave1Race;
Model meta;
Model cubiertaMeta;
Model puestoObservacion1;
Model puestoObservacion2;
Model puestoObservacion3;
Model puestoObservacion4;
Model warehousePort;
Model barcoEspacial;

//naves de F-Zero que corren en la pista, en el orden en que salen
const int NUM_NAVES_FZERO = 8;
Model navesFZero[NUM_NAVES_FZERO];
const char* nombresNavesFZero[NUM_NAVES_FZERO] = {
	"blueFalcon", "fireStingray", "goldenFox", "wildGoose", "nightThunder", "rainbowPhoenix", "redGazzelle", "blackBull" };


//--- satelites y cuerpos espaciales ---
Model sateliteRace;

//--- plataforma de lanzamiento y cohete ---

//--- estacion espacial y naves ---

//--- avatar ---

//--- camaras y recorridos ---
//viaje rapido: se comparan con los contadores de Window para saber cuando se presiono N o M
int viajeNodrizaActual = 0;
int viajePistaActual = 0;

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

	//--- nave de Among Us y crewmates ---
	AmongUsNave_M = Model();
	AmongUsNave_M.LoadModel("Models/AmongUs/amongus_nave.obj");
	CrewmateCuerpo_M = Model();
	CrewmateCuerpo_M.LoadModel("Models/AmongUs/amongus_crewmate_cuerpo.obj");
	CrewmateVisor_M = Model();
	CrewmateVisor_M.LoadModel("Models/AmongUs/amongus_crewmate_visor.obj");

	//--- planeta 3: Verdania ---
	//skybox de Verdania, generado por codigo con ruido; las caras van en el mismo orden: rt, lf, dn, up, bk, ft
	std::vector<std::string> skyboxVerdaniaFaces;
	skyboxVerdaniaFaces.push_back("Textures/Skybox/verdania_rt.png");
	skyboxVerdaniaFaces.push_back("Textures/Skybox/verdania_lf.png");
	skyboxVerdaniaFaces.push_back("Textures/Skybox/verdania_dn.png");
	skyboxVerdaniaFaces.push_back("Textures/Skybox/verdania_up.png");
	skyboxVerdaniaFaces.push_back("Textures/Skybox/verdania_bk.png");
	skyboxVerdaniaFaces.push_back("Textures/Skybox/verdania_ft.png");
	skyboxVerdania = Skybox(skyboxVerdaniaFaces);

	verdaniaPastoTexture = Texture("Textures/verdania_pasto.png");
	verdaniaPastoTexture.LoadTextureA();

	//las rutas se arman con el nombre de cada pieza, igual que la carnivora
	for (int i = 0; i < NUM_PIEZAS_PEASHOOTER; i++)
	{
		std::string ruta = std::string("Models/Peashooter/peashooter_") + piezasPeashooter[i] + ".obj";
		Peashooter_M[i] = Model();
		Peashooter_M[i].LoadModel(ruta);
	}

	PlanetaVerdania_M = Model();
	PlanetaVerdania_M.LoadModel("Models/Peashooter/peashooter_planeta.obj");

	for (int i = 0; i < NUM_PIEZAS_ICEPEA; i++)
	{
		std::string ruta = std::string("Models/IcePea/icepea_") + piezasIcePea[i] + ".obj";
		IcePea_M[i] = Model();
		IcePea_M[i].LoadModel(ruta);
	}

	for (int i = 0; i < NUM_PIEZAS_GATLINGPEA; i++)
	{
		std::string ruta = std::string("Models/GatlingPea/gatlingpea_") + piezasGatlingPea[i] + ".obj";
		GatlingPea_M[i] = Model();
		GatlingPea_M[i].LoadModel(ruta);
	}

	for (int i = 0; i < NUM_PIEZAS_FIREPEA; i++)
	{
		std::string ruta = std::string("Models/FirePea/firepea_") + piezasFirePea[i] + ".obj";
		FirePea_M[i] = Model();
		FirePea_M[i].LoadModel(ruta);
	}

	for (int i = 0; i < NUM_PIEZAS_CACTUS; i++)
	{
		std::string ruta = std::string("Models/Cactus/cactus_") + piezasCactus[i] + ".obj";
		Cactus_M[i] = Model();
		Cactus_M[i].LoadModel(ruta);
	}

	for (int i = 0; i < NUM_PIEZAS_SUNFLOWER; i++)
	{
		std::string ruta = std::string("Models/Sunflower/sunflower_") + piezasSunflower[i] + ".obj";
		Sunflower_M[i] = Model();
		Sunflower_M[i].LoadModel(ruta);
	}

	for (int i = 0; i < NUM_PIEZAS_SUNFLOWER2; i++)
	{
		std::string ruta = std::string("Models/Sunflower2/sunflower2_") + piezasSunflower2[i] + ".obj";
		Sunflower2_M[i] = Model();
		Sunflower2_M[i].LoadModel(ruta);
	}

	for (int i = 0; i < NUM_PIEZAS_STUFFYFLOWER; i++)
	{
		std::string ruta = std::string("Models/StuffyFlower/stuffyflower_") + piezasStuffyFlower[i] + ".obj";
		StuffyFlower_M[i] = Model();
		StuffyFlower_M[i].LoadModel(ruta);
	}

	for (int i = 0; i < NUM_PIEZAS_HYPNOSHROOM; i++)
	{
		std::string ruta = std::string("Models/HypnoShroom/hypnoshroom_") + piezasHypnoShroom[i] + ".obj";
		HypnoShroom_M[i] = Model();
		HypnoShroom_M[i].LoadModel(ruta);
	}

	for (int i = 0; i < NUM_PIEZAS_LASERBEAN; i++)
	{
		std::string ruta = std::string("Models/LaserBean/laserbean_") + piezasLaserBean[i] + ".obj";
		LaserBean_M[i] = Model();
		LaserBean_M[i].LoadModel(ruta);
	}

	for (int i = 0; i < NUM_PIEZAS_POTATOMINE; i++)
	{
		std::string ruta = std::string("Models/PotatoMine/potatomine_") + piezasPotatoMine[i] + ".obj";
		PotatoMine_M[i] = Model();
		PotatoMine_M[i].LoadModel(ruta);
	}

	for (int i = 0; i < NUM_PIEZAS_TORCHWOOD; i++)
	{
		std::string ruta = std::string("Models/Torchwood/torchwood_") + piezasTorchwood[i] + ".obj";
		Torchwood_M[i] = Model();
		Torchwood_M[i].LoadModel(ruta);
	}

	WallNut_M = Model();
	WallNut_M.LoadModel("Models/WallNut/wallnut_cuerpo.obj");

	//jardin de crazy dave
	JardinCasa_M = Model();
	JardinCasa_M.LoadModel("Models/JardinDave/jardin_casa.obj");
	JardinCalle_M = Model();
	JardinCalle_M.LoadModel("Models/JardinDave/jardin_calle.obj");
	JardinCalleFin_M = Model();
	JardinCalleFin_M.LoadModel("Models/JardinDave/jardin_calle_fin.obj");
	JardinCerca_M = Model();
	JardinCerca_M.LoadModel("Models/JardinDave/jardin_cerca.obj");
	JardinCercaPuerta_M = Model();
	JardinCercaPuerta_M.LoadModel("Models/JardinDave/jardin_cerca_puerta.obj");
	JardinFarol_M = Model();
	JardinFarol_M.LoadModel("Models/JardinDave/jardin_farol.obj");
	JardinArbol1_M = Model();
	JardinArbol1_M.LoadModel("Models/JardinDave/jardin_arbol1.obj");
	JardinArbol2_M = Model();
	JardinArbol2_M.LoadModel("Models/JardinDave/jardin_arbol2.obj");
	JardinArbusto_M = Model();
	JardinArbusto_M.LoadModel("Models/JardinDave/jardin_arbusto.obj");
	JardinArbustoFlores_M = Model();
	JardinArbustoFlores_M.LoadModel("Models/JardinDave/jardin_arbusto_flores.obj");
	JardinFlores1_M = Model();
	JardinFlores1_M.LoadModel("Models/JardinDave/jardin_flores1.obj");
	JardinFlores2_M = Model();
	JardinFlores2_M.LoadModel("Models/JardinDave/jardin_flores2.obj");
	JardinRoca_M = Model();
	JardinRoca_M.LoadModel("Models/JardinDave/jardin_roca.obj");
	JardinCaminoPiedra_M = Model();
	JardinCaminoPiedra_M.LoadModel("Models/JardinDave/jardin_camino_piedra1.obj");
	JardinHongo_M = Model();
	JardinHongo_M.LoadModel("Models/JardinDave/jardin_hongo.obj");
	JardinTrebol_M = Model();
	JardinTrebol_M.LoadModel("Models/JardinDave/jardin_trebol.obj");
	JardinHelecho_M = Model();
	JardinHelecho_M.LoadModel("Models/JardinDave/jardin_helecho.obj");

	for (int i = 0; i < NUM_PIEZAS_CRAZYDAVE; i++)
	{
		std::string ruta = std::string("Models/CrazyDave/crazydave_") + piezasCrazyDave[i] + ".obj";
		CrazyDave_M[i] = Model();
		CrazyDave_M[i].LoadModel(ruta);
	}

	//--- pista de carreras y naves ---
	pistaM = Model();
	pistaM.LoadModel("Models/pista/pista.obj");
	nave1Race = Model();
	nave1Race.LoadModel("Models/nave1Race/nave1Race.obj");
	meta = Model();
	meta.LoadModel("Models/meta/meta.obj");
	cubiertaMeta = Model();
	cubiertaMeta.LoadModel("Models/cubiertaMeta/cubiertaMeta.obj");
	puestoObservacion1 = Model();
	puestoObservacion1.LoadModel("Models/puestoObservacion1/puestoObservacion1.obj");
	puestoObservacion2 = Model();
	puestoObservacion2.LoadModel("Models/puestoObservacion2/puestoObservacion2.obj");
	puestoObservacion3 = Model();
	puestoObservacion3.LoadModel("Models/puestoObservacion3/puestoObservacion3.obj");
	puestoObservacion4 = Model();
	puestoObservacion4.LoadModel("Models/puestoObservacion4/puestoObservacion4.obj");
	warehousePort = Model();
	warehousePort.LoadModel("Models/warehousePort/warehousePort.obj");
	barcoEspacial = Model();
	barcoEspacial.LoadModel("Models/barcoEspacial/barcoEspacial.obj");

	InicializarRutaPista();

	//cada nave esta en su propia carpeta, con el mismo nombre que su .obj
	for (int i = 0; i < NUM_NAVES_FZERO; i++)
	{
		std::string ruta = std::string("Models/") + nombresNavesFZero[i] + "/" + nombresNavesFZero[i] + ".obj";
		navesFZero[i] = Model();
		navesFZero[i].LoadModel(ruta);
	}

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
		//viaje rapido con N y M; la camara se vuelve a crear en el lugar nuevo, igual que al cambiar de planeta
		//zonaActual se pone en 0 para que la seccion de planetas no regrese la camara al origen en este mismo frame
		if (mainWindow.getviajeNodriza() != viajeNodrizaActual)
		{
			viajeNodrizaActual = mainWindow.getviajeNodriza();
			zonaActual = 0;
			//a un lado de la nave nodriza, a su altura y viendo hacia X negativo
			camera = Camera(glm::vec3(500.0f, -130.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 180.0f, 0.0f, 0.3f, 0.5f);
		}
		if (mainWindow.getviajePista() != viajePistaActual)
		{
			viajePistaActual = mainWindow.getviajePista();
			zonaActual = 0;
			//atras de la meta y un poco arriba, viendo hacia Z negativo, que es hacia donde salen las naves
			camera = Camera(posPista + glm::vec3(0.0f, 40.0f, 120.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -15.0f, 0.3f, 0.5f);
		}

		//limpiar la pantalla
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//el skybox va primero, antes que todo lo demas, y cada planeta tiene el suyo segun zonaActual, que cambia con Z, X y C
		//solo el del espacio gira; los de los planetas se quedan quietos para que el horizonte no se incline
		if (zonaActual == 1)
			skyboxXen.DrawSkybox(view, projection, deltaTime, false);
		else if (zonaActual == 2)
			skyboxPlaneta2.DrawSkybox(view, projection, deltaTime, false);
		else if (zonaActual == 3)
			skyboxVerdania.DrawSkybox(view, projection, deltaTime, false);
		else
			skybox.DrawSkybox(view, projection, deltaTime, true);

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
		//el espacio no lleva piso porque todo flota, se deja comentado por si se ocupa para probar algo
		/*
		//piso
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pruebaTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[0]->RenderMeshModel();
		*/

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
		model = glm::translate(model, glm::vec3(-3.0f, -0.2f, -3.0f));
		model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
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
			else if (zonaActual == 3)
				camera = Camera(posVerdania + glm::vec3(0.0f, 1.5f, 8.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.3f, 0.5f);
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

		//--- nave de Among Us y crewmates ---
		anguloAmongUs += anguloAmongUsOffset * deltaTime;
		if (anguloAmongUs > 360.0f)
			anguloAmongUs -= 360.0f;

		//modelauxAmongUs guarda el lugar y el giro del grupo, y de ahi se ponen la nave y los crewmates
		glm::mat4 modelauxAmongUs = glm::mat4(1.0);
		modelauxAmongUs = glm::translate(modelauxAmongUs, posNaveAmongUs);
		modelauxAmongUs = glm::rotate(modelauxAmongUs, giroNaveAmongUs * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		//la nave va y viene en Z, que es hacia donde apunta su frente
		model = modelauxAmongUs;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, avanceNave * sin(anguloAmongUs * toRadians)));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		AmongUsNave_M.RenderModel();

		//cada crewmate va y viene en Z, alejandose y acercandose a la puerta de atras, y se ladea un poco con el mismo seno
		//se ladea desde la mitad del cuerpo y no desde los pies, por eso se sube 0.5 antes de girar y se baja despues
		//cada uno se dibuja dos veces, el cuerpo con su color y el visor con blanco
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		for (int i = 0; i < NUM_CREWMATES_NAVE; i++)
		{
			float onda = sin((anguloAmongUs + i * 60.0f) * toRadians);
			model = modelauxAmongUs;
			model = glm::translate(model, posCrewmates[i] + glm::vec3(0.0f, 0.0f, -avanceCrewmates * onda));
			model = glm::rotate(model, giroYCrewmates[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
			model = glm::rotate(model, (ladeoCrewmates[i] + 10.0f * onda) * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = colorCrewmates[i];
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			CrewmateCuerpo_M.RenderModel();
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			CrewmateVisor_M.RenderModel();
		}

		//crewmates escondidos; el primero va en Xen, el segundo en el planeta 2 y el tercero en Verdania
		//el de Xen sube y baja igual que la isla donde esta parado, que es la primera y no va desfasada
		glm::vec3 superficies[NUM_CREWMATES_ESCONDIDOS] = { posXen, posPlaneta2, posVerdania };
		for (int i = 0; i < NUM_CREWMATES_ESCONDIDOS; i++)
		{
			glm::vec3 pos = superficies[i] + posEscondidos[i];
			if (i == 0)
				pos.y += 0.8f * sin(anguloIslas * toRadians);
			model = glm::mat4(1.0);
			model = glm::translate(model, pos);
			model = glm::rotate(model, giroEscondidos[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = colorEscondidos[i];
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			CrewmateCuerpo_M.RenderModel();
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			CrewmateVisor_M.RenderModel();
		}
		//se regresa el material opaco, que es el que traen las secciones de abajo
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		//--- planeta 3: Verdania ---
		//anguloVerdania va de 0 a 360 grados y avanza un poco cada frame con su offset por deltaTime
		//cada parte que se mueve usa el seno de anguloVerdania por un numero, que va y viene suave entre -1 y 1,
		//y ese valor se multiplica por los grados maximos que se quiere mover la pieza
		//el numero que multiplica al angulo siempre es entero, asi el seno da lo mismo en 360 que en 0
		//y cuando el angulo regresa a 0 la pieza sigue su movimiento sin brincar
		//entre mas grande es ese numero mas rapido se mueve, y si se suman grados dentro del seno, como 120 o 240,
		//las plantas iguales quedan desfasadas y no se mueven todas al mismo tiempo
		//las animaciones solo avanzan mientras esten prendidas con la tecla V, igual que la caminata del headcrab con H;
		//si se apagan, los angulos y posiciones ya no cambian y las plantas se quedan quietas en su ultima pose
		if (mainWindow.getanimaVerdania())
		{
			//el angulo de todas las animaciones avanza y al llegar a 360 se regresa a 0
			anguloVerdania += anguloVerdaniaOffset * deltaTime;
			if (anguloVerdania > 360.0f)
				anguloVerdania -= 360.0f;
			//el peashooter avanza en su circulo
			anguloPeashooter += anguloPeashooterOffset * deltaTime;
			if (anguloPeashooter > 360.0f)
				anguloPeashooter -= 360.0f;
			//crazy dave avanza hacia un lado; al pasar el limite cambia de direccion
			if (daveAdelante)
			{
				posDave += posDaveOffset * deltaTime;
				if (posDave > limiteDave)
					daveAdelante = false;
			}
			else
			{
				posDave -= posDaveOffset * deltaTime;
				if (posDave < -limiteDave)
					daveAdelante = true;
			}
			//crazy dave se voltea poco a poco hacia donde camina, a 90 grados hacia X positivo y a -90 hacia X negativo,
			//en lugar de girar de golpe al llegar al limite
			if (daveAdelante && rumboDave < 90.0f)
			{
				rumboDave += rumboDaveOffset * deltaTime;
				if (rumboDave > 90.0f)
					rumboDave = 90.0f;
			}
			if (!daveAdelante && rumboDave > -90.0f)
			{
				rumboDave -= rumboDaveOffset * deltaTime;
				if (rumboDave < -90.0f)
					rumboDave = -90.0f;
			}
			//los canones de la gatling pea giran sin parar
			giroCanones += giroCanonesOffset * deltaTime;
			if (giroCanones > 360.0f)
				giroCanones -= 360.0f;
		}

		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		//piso de pasto, del mismo tamano que el de Xen (200 por 200)
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania);
		model = glm::scale(model, glm::vec3(10.0f, 1.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		verdaniaPastoTexture.UseTexture();
		meshListModel[0]->RenderMeshModel();

		//todas las plantas se dibujan igual, primero se ponen los grados que gira cada pieza,
		//luego la matriz model con su lugar en Verdania y hacia donde mira, y al final DibujaJerarquia
		//dibuja cada pieza encima de su padre girando en su pivote

		//peashooter, que es NPC y camina en circulo como el headcrab
		//pasoPea va de -1 a 1; las hojas-pata se mueven cruzadas: la delantera izquierda junto con la trasera derecha
		//y la delantera derecha junto con la trasera izquierda, como caminan los animales de cuatro patas
		float pasoPea = sin(anguloVerdania * 3.0f * toRadians);
		rotPeashooter[8].x = 25.0f * pasoPea;   //hoja_del_izq
		rotPeashooter[11].x = 25.0f * pasoPea;  //hoja_tras_der
		rotPeashooter[9].x = -25.0f * pasoPea;  //hoja_del_der
		rotPeashooter[10].x = -25.0f * pasoPea; //hoja_tras_izq
		//los brazos vienen extendidos en T en el modelo; se bajan 70 grados en Z en el hombro
		//y se balancean en X al caminar, cada uno al reves del otro
		rotPeashooter[6].z = -70.0f;            //brazo_izq abajo
		rotPeashooter[7].z = 70.0f;             //brazo_der abajo
		rotPeashooter[6].x = -20.0f * pasoPea;  //brazo_izq adelante y atras
		rotPeashooter[7].x = 20.0f * pasoPea;   //brazo_der adelante y atras
		//la cabeza rebota dos veces por cada paso, por eso su seno va al doble que el del paso, y el tallo se mece un poco
		rotPeashooter[3].x = 4.0f * sin(anguloVerdania * 6.0f * toRadians); //cabeza
		rotPeashooter[1].z = 3.0f * pasoPea;                                 //tallo1
		//la posicion en el circulo sale del seno y el coseno de su angulo, y se gira 90 grados mas para que mire hacia donde avanza
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + centroPeashooter +
			glm::vec3(radioPeashooter * sin(anguloPeashooter * toRadians), 0.0f, radioPeashooter * cos(anguloPeashooter * toRadians)));
		model = glm::rotate(model, (anguloPeashooter + 90.0f) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(Peashooter_M, padresPeashooter, pivotesPeashooter, rotPeashooter, NUM_PIEZAS_PEASHOOTER, model, uniformModel);

		//crazy dave, que es NPC y camina de un lado a otro en linea recta
		//las piernas van contrarias y cada brazo va al reves que su pierna, como cuando camina una persona;
		//la pantorrilla y el pie son hijos del muslo, asi que se mueven con el sin tocarlos
		float pasoDave = sin(anguloVerdania * 4.0f * toRadians);
		rotCrazyDave[11].x = 25.0f * pasoDave;  //muslo_der
		rotCrazyDave[14].x = -25.0f * pasoDave; //muslo_izq
		//los brazos vienen abiertos en el modelo; se bajan 45 grados en Z y se balancean en X
		rotCrazyDave[5].z = 45.0f;              //brazo_der abajo
		rotCrazyDave[8].z = -45.0f;             //brazo_izq abajo
		rotCrazyDave[5].x = -20.0f * pasoDave;  //brazo_der adelante y atras
		rotCrazyDave[8].x = 20.0f * pasoDave;   //brazo_izq adelante y atras
		//la olla de la cabeza se tambalea rapido porque su seno multiplica al angulo por 8
		rotCrazyDave[4].z = 6.0f * sin(anguloVerdania * 8.0f * toRadians); //olla
		//el modelo mira hacia Z positivo y rumboDave lo gira en Y hacia donde camina
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(posDave, 0.0f, -8.0f));
		model = glm::rotate(model, rumboDave * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(CrazyDave_M, padresCrazyDave, pivotesCrazyDave, rotCrazyDave, NUM_PIEZAS_CRAZYDAVE, model, uniformModel);

		//los tres peas en maceta son NPCs que vigilan, voltean la cabeza de un lado a otro hasta 35 grados en Y
		//y el tallo se inclina un poco hacia adelante y atras en X; cada uno va desfasado 120 grados del anterior
		rotIcePea[2].y = 35.0f * sin(anguloVerdania * toRadians);       //cabeza
		rotIcePea[1].x = 4.0f * sin(anguloVerdania * 2.0f * toRadians); //tallo
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(-6.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 40.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(IcePea_M, padresIcePea, pivotesIcePea, rotIcePea, NUM_PIEZAS_ICEPEA, model, uniformModel);

		rotGatlingPea[2].y = 35.0f * sin((anguloVerdania + 120.0f) * toRadians);       //cabeza
		rotGatlingPea[1].x = 4.0f * sin((anguloVerdania * 2.0f + 120.0f) * toRadians); //tallo
		//los canones giran sobre su propio eje, que es el eje Z del modelo; como son hijos de la boca, siguen a la cabeza
		rotGatlingPea[4].z = giroCanones;
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(-7.0f, 0.0f, -3.0f));
		model = glm::rotate(model, 55.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(GatlingPea_M, padresGatlingPea, pivotesGatlingPea, rotGatlingPea, NUM_PIEZAS_GATLINGPEA, model, uniformModel);

		rotFirePea[2].y = 35.0f * sin((anguloVerdania + 240.0f) * toRadians);       //cabeza
		rotFirePea[1].x = 4.0f * sin((anguloVerdania * 2.0f + 240.0f) * toRadians); //tallo
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(-6.5f, 0.0f, -6.5f));
		model = glm::rotate(model, 70.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(FirePea_M, padresFirePea, pivotesFirePea, rotFirePea, NUM_PIEZAS_FIREPEA, model, uniformModel);

		//el cactus es NPC y sube y baja los brazos en Z; con el mismo valor en los dos uno sube mientras el otro baja
		//porque estan de lados contrarios, y el tallo de arriba se mece
		rotCactus[4].z = 10.0f * sin(anguloVerdania * 2.0f * toRadians); //brazo_izq
		rotCactus[5].z = 10.0f * sin(anguloVerdania * 2.0f * toRadians); //brazo_der
		rotCactus[2].z = 3.0f * sin(anguloVerdania * toRadians);         //tallo2
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(6.5f, 0.0f, -1.0f));
		model = glm::rotate(model, -40.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(Cactus_M, padresCactus, pivotesCactus, rotCactus, NUM_PIEZAS_CACTUS, model, uniformModel);

		//los girasoles son de adorno y mecen la cabeza de lado en Z y el tallo hacia adelante y atras en X, como con el viento;
		//cada uno va desfasado 120 grados del anterior
		rotSunflower[6].z = -60.0f; //los brazos tambien vienen en T y se bajan 60 grados
		rotSunflower[7].z = 60.0f;
		rotSunflower[3].z = 8.0f * sin(anguloVerdania * toRadians); //cabeza
		rotSunflower[1].x = 3.0f * sin(anguloVerdania * toRadians); //tallo1
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(-3.0f, 0.0f, -12.0f));
		model = glm::rotate(model, 15.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(Sunflower_M, padresSunflower, pivotesSunflower, rotSunflower, NUM_PIEZAS_SUNFLOWER, model, uniformModel);

		rotSunflower2[6].z = -60.0f;
		rotSunflower2[7].z = 60.0f;
		rotSunflower2[3].z = 8.0f * sin((anguloVerdania + 120.0f) * toRadians); //cabeza
		rotSunflower2[1].x = 3.0f * sin((anguloVerdania + 120.0f) * toRadians); //tallo1
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(0.0f, 0.0f, -13.0f));
		DibujaJerarquia(Sunflower2_M, padresSunflower2, pivotesSunflower2, rotSunflower2, NUM_PIEZAS_SUNFLOWER2, model, uniformModel);

		rotStuffyFlower[6].z = -60.0f;
		rotStuffyFlower[7].z = 60.0f;
		rotStuffyFlower[3].z = 8.0f * sin((anguloVerdania + 240.0f) * toRadians); //cabeza
		rotStuffyFlower[1].x = 3.0f * sin((anguloVerdania + 240.0f) * toRadians); //tallo1
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(3.0f, 0.0f, -12.0f));
		model = glm::rotate(model, -15.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(StuffyFlower_M, padresStuffyFlower, pivotesStuffyFlower, rotStuffyFlower, NUM_PIEZAS_STUFFYFLOWER, model, uniformModel);

		//hypno-shroom de adorno, el sombrero se tambalea de lado y el tallo gira un poco sobre si mismo
		rotHypnoShroom[2].z = 6.0f * sin(anguloVerdania * 2.0f * toRadians); //sombrero
		rotHypnoShroom[1].y = 10.0f * sin(anguloVerdania * toRadians);       //tallo
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(7.0f, 0.0f, -6.0f));
		model = glm::rotate(model, -50.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(HypnoShroom_M, padresHypnoShroom, pivotesHypnoShroom, rotHypnoShroom, NUM_PIEZAS_HYPNOSHROOM, model, uniformModel);

		//laser bean de adorno, el cuerpo se balancea dentro de su maceta y el pelo rebota al doble de rapido
		rotLaserBean[1].z = 8.0f * sin(anguloVerdania * toRadians);         //cuerpo
		rotLaserBean[2].x = 15.0f * sin(anguloVerdania * 2.0f * toRadians); //pelo
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(5.0f, 0.0f, 3.0f));
		model = glm::rotate(model, -30.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(LaserBean_M, padresLaserBean, pivotesLaserBean, rotLaserBean, NUM_PIEZAS_LASERBEAN, model, uniformModel);

		//potato mine de adorno, la antena se mueve rapido de un lado a otro
		rotPotatoMine[2].z = 15.0f * sin(anguloVerdania * 3.0f * toRadians); //antena
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(-4.0f, 0.0f, 4.0f));
		model = glm::rotate(model, 30.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(PotatoMine_M, padresPotatoMine, pivotesPotatoMine, rotPotatoMine, NUM_PIEZAS_POTATOMINE, model, uniformModel);

		//wall-nut de adorno, como es una sola pieza se balancea todo el modelo hasta 6 grados en Z;
		//su origen esta en la base, asi que gira apoyada en el piso
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(8.5f, 0.0f, -9.0f));
		model = glm::rotate(model, -40.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 6.0f * sin(anguloVerdania * toRadians) * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		WallNut_M.RenderModel();

		//torchwood de adorno, los brazos suben y bajan en Z y la mandibula se abre entre 0 y 12 grados en X
		//como a 6 se le suma 6 por el seno, nunca baja de 0 y la boca no se cierra de mas
		rotTorchwood[5].z = 8.0f * sin(anguloVerdania * toRadians);               //brazo_izq
		rotTorchwood[7].z = -8.0f * sin(anguloVerdania * toRadians);              //brazo_der
		rotTorchwood[4].x = 6.0f + 6.0f * sin(anguloVerdania * 2.0f * toRadians); //mandibula
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(0.0f, 0.0f, -18.0f));
		DibujaJerarquia(Torchwood_M, padresTorchwood, pivotesTorchwood, rotTorchwood, NUM_PIEZAS_TORCHWOOD, model, uniformModel);

		//jardin de crazy dave
		//son objetos quietos, cada uno se pone en su lugar con translate, se voltea con rotate y se le da su tamano con scale
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		//casa de crazy dave al fondo; el modelo tiene la puerta hacia Z negativo, por eso se gira 180 grados para que vea al jardin
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(0.0f, 0.0f, -34.0f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaKenney, escalaKenney, escalaKenney));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JardinCasa_M.RenderModel();

		//en la calle los tramos rectos se giran 90 grados para que corran sobre X, y en la punta va el final de la calle
		for (int i = 0; i < NUM_TRAMOS_CALLE; i++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, posVerdania + glm::vec3(posCalleX[i], 0.01f, posCalleZ));
			model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(escalaKenney, escalaKenney, escalaKenney));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JardinCalle_M.RenderModel();
		}
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(posCalleX[0] - 6.5f, 0.01f, posCalleZ));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaKenney, escalaKenney, escalaKenney));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JardinCalleFin_M.RenderModel();

		//farol junto a donde termina la calle
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(posCalleX[0] - 5.0f, 0.0f, posCalleZ - 4.0f));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaKenney, escalaKenney, escalaKenney));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JardinFarol_M.RenderModel();

		//camino de piedra; cada piedra se gira distinto para que el camino no se vea repetido
		for (int i = 0; i < NUM_PIEDRAS_CAMINO; i++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, posVerdania + posCamino[i]);
			model = glm::rotate(model, i * 47.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JardinCaminoPiedra_M.RenderModel();
		}

		//cerca entre el jardin y la casa; en indicePuerta va la pieza con la puerta abierta
		for (int i = 0; i < NUM_CERCAS_JARDIN; i++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, posVerdania + glm::vec3(posCercaX[i], 0.0f, posCercaZ));
			if (i == indicePuerta)
			{
				model = glm::scale(model, glm::vec3(escalaKenney * estiraPuerta, escalaKenney, escalaKenney));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				JardinCercaPuerta_M.RenderModel();
			}
			else
			{
				model = glm::scale(model, glm::vec3(escalaKenney, escalaKenney, escalaKenney));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				JardinCerca_M.RenderModel();
			}
		}

		//en los arboles se alternan los dos modelos, los pares usan el 1 y los impares el 2
		for (int i = 0; i < NUM_ARBOLES_JARDIN; i++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, posVerdania + posArbolesJardin[i]);
			model = glm::rotate(model, giroArbolesJardin[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(escalaArbolesJardin[i], escalaArbolesJardin[i], escalaArbolesJardin[i]));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			if (i % 2 == 0)
				JardinArbol1_M.RenderModel();
			else
				JardinArbol2_M.RenderModel();
		}

		//en los arbustos se alternan el normal y el que tiene flores
		for (int i = 0; i < NUM_ARBUSTOS_JARDIN; i++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, posVerdania + posArbustosJardin[i]);
			model = glm::rotate(model, giroArbustosJardin[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(escalaArbustosJardin[i], escalaArbustosJardin[i], escalaArbustosJardin[i]));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			if (i % 2 == 0)
				JardinArbusto_M.RenderModel();
			else
				JardinArbustoFlores_M.RenderModel();
		}

		//en las flores se alternan los dos ramos, cada uno con un giro distinto
		for (int i = 0; i < NUM_FLORES_JARDIN; i++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, posVerdania + posFloresJardin[i]);
			model = glm::rotate(model, i * 63.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(escalaFloresJardin[i], escalaFloresJardin[i], escalaFloresJardin[i]));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			if (i % 2 == 0)
				JardinFlores1_M.RenderModel();
			else
				JardinFlores2_M.RenderModel();
		}

		//los detalles chicos del pasto se reparten en orden trebol, helecho y hongo, y el residuo de i entre 3 dice cual toca
		for (int i = 0; i < NUM_DETALLES_JARDIN; i++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, posVerdania + posDetallesJardin[i]);
			model = glm::rotate(model, i * 71.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(escalaDetallesJardin[i % 3], escalaDetallesJardin[i % 3], escalaDetallesJardin[i % 3]));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			if (i % 3 == 0)
				JardinTrebol_M.RenderModel();
			else if (i % 3 == 1)
				JardinHelecho_M.RenderModel();
			else
				JardinHongo_M.RenderModel();
		}

		//dos rocas grandes en las orillas del jardin
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(-17.0f, 0.0f, -13.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JardinRoca_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(15.0f, 0.0f, -24.0f));
		model = glm::rotate(model, 120.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JardinRoca_M.RenderModel();

		//planeta Verdania en el espacio, gira despacio sobre Y como Xen; mide como 0.73 de alto y con escala 130 queda de unas 95 unidades
		giroPlanetaVerdania += giroPlanetaVerdaniaOffset * deltaTime;
		if (giroPlanetaVerdania > 360.0f)
			giroPlanetaVerdania -= 360.0f;
		model = glm::mat4(1.0);
		model = glm::translate(model, posPlanetaVerdania);
		model = glm::rotate(model, giroPlanetaVerdania * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(130.0f, 130.0f, 130.0f));
		model = glm::translate(model, -centroCabezaPea);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PlanetaVerdania_M.RenderModel();

		//--- pista de carreras y naves ---
		//pista de carreras
		//modelauxNaves guarda la pista antes de voltearla, que es la matriz con la que se sacaron los puntos de la ruta
		//modelauxPista guarda la pista ya volteada en X y en Z, y sobre ella se ponen la meta y los edificios
		model = glm::mat4(1.0f);
		model = glm::translate(model, posPista);
		model = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
		modelauxNaves = model;
		model = glm::scale(model, glm::vec3(-1.0f, 1.0f, -1.0));
		modelauxPista = model;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pistaM.RenderModel();

		//meta
		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(0.0f, 0.05f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meta.RenderModel();

		//cubierta de la meta
		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cubiertaMeta.RenderModel();

		//la carrera avanza cada frame y al terminar una vuelta empieza otra
		avanceCarrera += velocidadCarrera * deltaTime;
		if (avanceCarrera > longitudTotalPista)
			avanceCarrera -= longitudTotalPista;

		//naves de F-Zero corriendo en la pista
		for (int i = 0; i < NUM_NAVES_FZERO; i++)
		{
			//las naves pares van en el carril izquierdo y las impares en el derecho
			float carrilX = 2.15f;
			if (i % 2 == 0)
				carrilX = -5.15f;

			//cada nave va 10 unidades detras de la anterior
			float desfaseAtras = -10.0f * (i + 1);

			//las naves suben y bajan un poco con un seno para que se vea que flotan
			float flotacion = 1.00f + 0.07f * sin((avanceCarrera * 0.1f) + i);

			model = modelauxNaves * ObtenerMatrizNavePista(avanceCarrera + desfaseAtras, carrilX, flotacion);
			model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			navesFZero[i].RenderModel();
		}

		//puesto de observacion 1, son los edificios del centro
		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(175.0f, 80.00f, 500.0f));
		model = glm::rotate(model, -45.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puestoObservacion1.RenderModel();

		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(175.0f, 80.00f, -375.0f));
		model = glm::rotate(model, -115.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puestoObservacion1.RenderModel();

		//puesto de observacion 2, son los edificios altos
		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(0.0f, 20.00f, 700.0f));
		model = glm::rotate(model, -25.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puestoObservacion2.RenderModel();

		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(300.0f, 200.00f, 700.0f));
		model = glm::rotate(model, 45.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puestoObservacion2.RenderModel();

		//puesto de observacion 3, son los edificios del centro que estan en la orilla
		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(150.0f, 180.00f, 800.0f));
		model = glm::scale(model, glm::vec3(0.75f, 0.75f, 0.75f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puestoObservacion3.RenderModel();

		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(150.0f, 140.00f, -680.0f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.75f, 0.75f, 0.75f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puestoObservacion3.RenderModel();

		//puesto de observacion 4
		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(0.0f, 20.00f, -600.0f));
		model = glm::rotate(model, 35.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puestoObservacion4.RenderModel();

		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(300.0f, 100.00f, -600.0f));
		model = glm::rotate(model, -35.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puestoObservacion4.RenderModel();

		//barco espacial, que es la nave nodriza en el centro de la escena
		//ya no cuelga de la pista; se gira 265 grados y se escala 0.315 para que quede igual que cuando estaba sobre ella
		//mide como 200 de alto, por eso se baja a -230 y la camara, que empieza en el origen, queda arriba de la nave
		model = glm::mat4(1.0f);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(0.0f, -230.0f, 0.0f));
		model = glm::rotate(model, 265.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.315f, 0.315f, 0.315f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		barcoEspacial.RenderModel();

		//almacen del puerto
		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(200.f, -150.00f, 50.0f));
		model = glm::rotate(model, 60.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		warehousePort.RenderModel();

		//nave 1 de la pista
		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(70.0f, 140.0f, 30.0f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 30.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		nave1Race.RenderModel();

		//nave 2 de la pista
		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(-70.0f, 140.0f, 30.0f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -30.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		nave1Race.RenderModel();

		//--- satelites y cuerpos espaciales ---
		//satelite
		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(-250.0f, 50.0f, -550.0f));
		model = glm::rotate(model, 240.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
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
