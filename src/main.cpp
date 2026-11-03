/*
Proyecto Final - Computacion Grafica
Aqui se arma toda la escena. El archivo esta partido en 4 bloques que empiezan con una linea de ===: las variables
globales, la geometria hecha a mano, la carga de recursos dentro del main y el while, que dibuja todo cada frame.
En cada bloque hay una seccion //--- ... --- por actividad, siempre en el mismo orden. Los arreglos largos, como las
piezas de los modelos con jerarquia y los puntos de la pista, estan en DatosEscena.h con las mismas secciones.
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
#include "DatosEscena.h"

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

//skybox del espacio; los de los planetas estan en la seccion de cada uno
Skybox skybox;

//materiales, se crean con Material(intensidad especular, shininess) en la carga de recursos
Material Material_brillante;
Material Material_opaco;
Material Material_metal;
Material Material_avatar;

//deltaTime suma como 61 en un segundo, porque al tiempo del frame se le suma ese mismo tiempo dividido entre limitFPS
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

//la superficie de cada planeta es un piso aparte de 200 por 200 y muy lejos del espacio; como el plano lejano
//de la perspectiva esta en 3000, nunca se ven juntos
glm::vec3 posXen = glm::vec3(6000.0f, 0.0f, 0.0f);
glm::vec3 posPlaneta2 = glm::vec3(-6000.0f, 0.0f, 0.0f);
glm::vec3 posVerdania = glm::vec3(0.0f, 0.0f, 6000.0f);

//--- personajes ---
//modelos de los personajes; Kirby y la carnivora se dibujan como el avatar y los demas todavia no salen en la escena
//los que tienen jerarquia se dibujan con DibujaJerarquia y se articulan cambiando sus rotaciones en el while,
//por ejemplo rotGordon[2].x = -30.0f; gira el brazo derecho en el hombro
Model CapitanFalcon_M;
Model Carnivora_M[NUM_PIEZAS_CARNIVORA];
glm::vec3 rotCarnivora[NUM_PIEZAS_CARNIVORA]; //las globales empiezan en 0
Model Gordon_M[NUM_PIEZAS_GORDON];
glm::vec3 rotGordon[NUM_PIEZAS_GORDON];
Model Kirby_M[NUM_PIEZAS_KIRBY];
glm::vec3 rotKirby[NUM_PIEZAS_KIRBY];

//--- planetas: Xen y planeta 2 ---
//los tres planetas que se ven desde el espacio estan en un circulo de radio 1400 alrededor del centro de la escena,
//que esta como en 300, 0, -100, separados 120 grados y a distinta altura, para que se sienta que estan muy lejos
//aunque la camara se mueva

//Xen
Skybox skyboxXenDia;
Skybox skyboxXenNoche;
Texture xenSueloTexture;
//el planeta que se ve desde el espacio es una esfera que gira sobre su eje
Model XenPlaneta_M;
glm::vec3 posPlanetaXen = glm::vec3(300.0f, 250.0f, -1500.0f);
float giroPlanetaXen = 0.0f;
float giroPlanetaXenOffset = 0.05f;
//objetos de la superficie
Model XenCristal_M;
Model XenIsla_M;
Model XenTallo_M;
Model XenFoco_M;
//criaturas acomodadas de la mas simple a la mas evolucionada
Model Houndeye_M;
Model Bullsquid_M;
Model Vortigaunt_M;
Model AlienGrunt_M;
//las islas flotantes suben y bajan con el seno de un angulo que avanza cada frame
float anguloIslas = 0.0f;
float anguloIslasOffset = 0.6f;
//las plantas de luz se esconden en el piso y vuelven a salir cada cierto tiempo, como en Half-Life
//alturaPlantas escala el tallo en Y, vale 1 cuando la planta esta parada y 0.1 cuando esta escondida
float alturaPlantas[NUM_PLANTAS] = { 1.0f, 1.0f, 1.0f };
float altoTallo = 1.62f; //altura de la punta del tallo, ahi va el foco
//tiempoPlantas cuenta cuanto lleva el estado actual; con estos tiempos duran unos 5 segundos paradas y 3 escondidas
float tiempoPlantas = 0.0f;
bool plantasEscondidas = false;
float tiempoParadas = 300.0f;
float tiempoEscondidas = 180.0f;
//el headcrab da vueltas en circulo en la superficie y empieza o se detiene con F
Model Headcrab_M[NUM_PIEZAS_HEADCRAB];
glm::vec3 rotHeadcrab[NUM_PIEZAS_HEADCRAB];
glm::vec3 centroHeadcrab = glm::vec3(0.0f, 0.0f, 2.0f); //el centro del circulo se mide desde posXen
float radioHeadcrab = 2.5f;
float anguloHeadcrab = 0.0f;
float anguloHeadcrabOffset = 0.4f;
//las patas van y vienen entre -limitePaso y limitePaso
float pasoHeadcrab = 0.0f;
float pasoHeadcrabOffset = 2.5f;
float limitePaso = 20.0f;
bool pasoAdelante = true;

//planeta 2, que es Pop Star, el planeta de Kirby
Skybox skyboxPlaneta2Dia;
Skybox skyboxPlaneta2Noche;
Texture planeta2SueloTexture;
//desde el espacio tiene forma de estrella; se gira para que su frente vea hacia el centro de la escena
//y da vueltas sobre ese frente como rehilete
Model Planeta2Espacio_M;
glm::vec3 posPlaneta2Espacio = glm::vec3(-912.0f, -150.0f, 600.0f);
float giroPlaneta2 = 0.0f;
float giroPlaneta2Offset = 0.04f;
//objetos de la superficie
Model Planeta2Colina_M;
Model Planeta2Arbol_M;
Model Planeta2Nube1_M;
Model Planeta2Nube2_M;
Model WarpStar_M;
//la warp star flota un poco arriba del piso, enfrente de donde llega la camara
glm::vec3 posWarpStar = glm::vec3(4.0f, 0.8f, -4.0f);

//--- planeta 3: Verdania ---
Skybox skyboxVerdaniaDia;
Skybox skyboxVerdaniaNoche;
Texture verdaniaPastoTexture;
//desde el espacio Verdania se ve como la cabeza del peashooter en grande, lisa y con textura de planeta
//peashooter_planeta.obj es la cabeza sin cuello, sin cejas y redonda por abajo
Model PlanetaVerdania_M;
glm::vec3 posPlanetaVerdania = glm::vec3(1512.0f, 100.0f, 600.0f);
//centro de la cabeza en las coordenadas del modelo; ahi se gira y se escala para que no gire alrededor de los pies
glm::vec3 centroCabezaPea = glm::vec3(0.0f, 1.15f, 0.05f);
float giroPlanetaVerdania = 0.0f;
float giroPlanetaVerdaniaOffset = 0.05f;

//las plantas estan separadas en piezas por sus huesos
//anguloVerdania marca el tiempo de todas sus animaciones y solo avanza mientras la F las tenga prendidas
float anguloVerdania = 0.0f;
float anguloVerdaniaOffset = 2.0f;

//peashooter, que camina en circulo como el headcrab
Model Peashooter_M[NUM_PIEZAS_PEASHOOTER];
glm::vec3 rotPeashooter[NUM_PIEZAS_PEASHOOTER];
glm::vec3 centroPeashooter = glm::vec3(0.0f, 0.0f, -2.0f); //el centro del circulo se mide desde posVerdania
float radioPeashooter = 2.5f;
float anguloPeashooter = 0.0f;
float anguloPeashooterOffset = 0.4f;

//crazy dave, que va y viene en linea recta en X entre -limiteDave y limiteDave
Model CrazyDave_M[NUM_PIEZAS_CRAZYDAVE];
glm::vec3 rotCrazyDave[NUM_PIEZAS_CRAZYDAVE];
float posDave = 0.0f;
float posDaveOffset = 0.02f;
float limiteDave = 4.0f;
bool daveAdelante = true;
//hacia donde mira en grados alrededor de Y; vale 90 cuando camina hacia X positivo y -90 hacia X negativo, y al cambiar
//de direccion se va girando de rumboDaveOffset en rumboDaveOffset para que la vuelta no sea de golpe
float rumboDave = 90.0f;
float rumboDaveOffset = 6.0f;

//los tres peas en maceta y el cactus se quedan en su lugar
Model IcePea_M[NUM_PIEZAS_ICEPEA];
glm::vec3 rotIcePea[NUM_PIEZAS_ICEPEA];
Model GatlingPea_M[NUM_PIEZAS_GATLINGPEA];
glm::vec3 rotGatlingPea[NUM_PIEZAS_GATLINGPEA];
float giroCanones = 0.0f; //angulo de los canones de la gatling pea, que giran sin parar
float giroCanonesOffset = 8.0f;
Model FirePea_M[NUM_PIEZAS_FIREPEA];
glm::vec3 rotFirePea[NUM_PIEZAS_FIREPEA];
Model Cactus_M[NUM_PIEZAS_CACTUS];
glm::vec3 rotCactus[NUM_PIEZAS_CACTUS];

//plantas de adorno, cada una con una animacion sencilla
Model Sunflower_M[NUM_PIEZAS_SUNFLOWER];
glm::vec3 rotSunflower[NUM_PIEZAS_SUNFLOWER];
Model Sunflower2_M[NUM_PIEZAS_SUNFLOWER2];
glm::vec3 rotSunflower2[NUM_PIEZAS_SUNFLOWER2];
Model StuffyFlower_M[NUM_PIEZAS_STUFFYFLOWER];
glm::vec3 rotStuffyFlower[NUM_PIEZAS_STUFFYFLOWER];
Model HypnoShroom_M[NUM_PIEZAS_HYPNOSHROOM];
glm::vec3 rotHypnoShroom[NUM_PIEZAS_HYPNOSHROOM];
Model LaserBean_M[NUM_PIEZAS_LASERBEAN];
glm::vec3 rotLaserBean[NUM_PIEZAS_LASERBEAN];
Model PotatoMine_M[NUM_PIEZAS_POTATOMINE];
glm::vec3 rotPotatoMine[NUM_PIEZAS_POTATOMINE];
Model Torchwood_M[NUM_PIEZAS_TORCHWOOD];
glm::vec3 rotTorchwood[NUM_PIEZAS_TORCHWOOD];
Model WallNut_M; //es una sola pieza, por eso es un Model normal

//jardin de crazy dave con su casa, la calle, cercas, un farol, arboles, arbustos, flores y piedras
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
//la casa mide 1.2 de alto y con escala 6.5 queda de casi 8; la calle, las cercas y el farol usan la misma escala
float escalaCasa = 6.5f;

//--- nave de Among Us y crewmates ---
//nave de Among Us con la puerta abierta y 6 crewmates que se salieron y van flotando en el espacio
//la nave mide como 12 de largo y tiene el frente hacia Z positivo y la puerta atras
//el crewmate mide 1 de alto y esta separado en cuerpo y visor; el cuerpo tiene la textura en gris para pintarlo
//de cualquier color con el uniform color, y el visor se dibuja con blanco para que no se pinte
Model AmongUsNave_M;
Model CrewmateCuerpo_M;
Model CrewmateVisor_M;
//el grupo esta enfrente de donde empieza la camara, un poco a la izquierda, y se gira para que la puerta quede viendo
//hacia ella
glm::vec3 posNaveAmongUs = glm::vec3(9.0f, 2.0f, -36.0f);
float giroNaveAmongUs = 160.0f;
//la nave y los crewmates van hacia adelante y hacia atras con el seno de anguloAmongUs, que avanza cada frame;
//cada crewmate va desfasado 60 grados del anterior para que no se muevan todos juntos
float anguloAmongUs = 0.0f;
float anguloAmongUsOffset = 0.4f;
float avanceNave = 0.5f;      //lo mas que se mueve la nave hacia adelante o hacia atras
float avanceCrewmates = 0.4f; //lo mismo para los crewmates, que se acercan y se alejan de la puerta

//--- pista de carreras y naves ---
Model Pista_M;
Model Meta_M;
Model CubiertaMeta_M;
Model PuestoObservacion1_M;
Model PuestoObservacion2_M;
Model PuestoObservacion3_M;
Model PuestoObservacion4_M;
Model Almacen_M;
Model NavePista_M;
Model NaveNodriza_M; //el barco espacial
//naves de F-Zero que corren en la pista, en el orden en que salen
const int NUM_NAVES_FZERO = 8;
Model NavesFZero_M[NUM_NAVES_FZERO];
const char* nombresNavesFZero[NUM_NAVES_FZERO] = {
	"blueFalcon", "fireStingray", "goldenFox", "wildGoose", "nightThunder", "rainbowPhoenix", "redGazzelle", "blackBull" };

//lugar de la pista en el espacio, cerca del centro de la escena; ahi queda la meta y con esto su centro queda como en 340, 0, -280
glm::vec3 posPista = glm::vec3(413.0f, 0.0f, -254.0f);

//matrices de la pista que se guardan en el while para poner encima la meta, los edificios y las naves
glm::mat4 modelauxPista(1.0);
glm::mat4 modelauxNaves(1.0);

//las naves avanzan por los puntos de la ruta, que estan en DatosEscena.h
//distAcumuladaPista guarda la distancia desde el primer punto hasta cada punto y se llena en InicializarRutaPista
float distAcumuladaPista[NUM_PUNTOS_PISTA + 1];
float longitudTotalPista = 0.0f;
float avanceCarrera = 0.0f; //distancia que lleva recorrida la primera nave
float velocidadCarrera = 2.5f;
const float ESCALA_INTERNA_PISTA = 0.45f; //los puntos se escalan igual que la pista, que lleva un segundo scale de 0.45

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
		//la distancia entre los dos puntos es la raiz de la suma de los cuadrados de lo que se separan en X, en Y y en Z
		glm::vec3 tramo = pSig - pActual;
		longitudTotalPista += sqrt(tramo.x * tramo.x + tramo.y * tramo.y + tramo.z * tramo.z);
		distAcumuladaPista[i + 1] = longitudTotalPista;
	}
}

//regresa la matriz de una nave que va a cierta distancia del inicio, en su carril y flotando sobre el piso
//despues se multiplica por modelauxNaves para que quede en el lugar de la pista
glm::mat4 ObtenerMatrizNavePista(float distancia, float carrilX, float alturaFlotacion)
{
	int n = NUM_PUNTOS_PISTA;

	//la distancia se ajusta para que quede entre 0 y lo que mide una vuelta, porque las naves de atras empiezan en negativo
	float d = distancia;
	if (d < 0.0f)
		d += longitudTotalPista;
	if (d > longitudTotalPista)
		d -= longitudTotalPista;

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

	//posicion y normal del piso entre los dos puntos del tramo; se parte del primero y se avanza la fraccion t del camino al segundo
	glm::vec3 p0 = posRutaPista[idx] * ESCALA_INTERNA_PISTA;
	glm::vec3 p1 = posRutaPista[idxSig] * ESCALA_INTERNA_PISTA;
	glm::vec3 p2 = posRutaPista[idxSig2] * ESCALA_INTERNA_PISTA;

	glm::vec3 posCentro = p0 + (p1 - p0) * t;
	glm::vec3 normalPiso = glm::normalize(normalRutaPista[idx] + (normalRutaPista[idxSig] - normalRutaPista[idx]) * t);

	//hacia donde va la nave, se mezcla la direccion de este tramo con la del siguiente para que gire suave en las curvas
	glm::vec3 tangenteActual = glm::normalize(p1 - p0);
	glm::vec3 tangenteSig = glm::normalize(p2 - p1);
	glm::vec3 frente = glm::normalize(tangenteActual + (tangenteSig - tangenteActual) * t);

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

//--- satelites y cuerpos espaciales ---
Model Satelite_M;

//--- plataforma de lanzamiento y cohete ---
//la plataforma esta en la superficie de Pop Star, el planeta 2, y en ella estan el cohete y su torre
//su lugar se mide desde el centro del piso del planeta, como los arboles y las colinas; queda adelante a la izquierda
//de donde se llega, entre los arboles y las colinas de las orillas, para que no se encime con nada
//son modelos de Kenney muy chicos (el cohete mide 4.6 de alto), por eso todo se escala por escalaPlataforma;
//con 4 el cohete queda de unas 18 unidades y el piso de 24 por 24
Model Cohete_M[NUM_PIEZAS_COHETE];
glm::vec3 rotCohete[NUM_PIEZAS_COHETE]; //las globales empiezan en 0
Model PlataformaCohete_M;
Model TorreCohete_M;
glm::vec3 posPlataforma = glm::vec3(-42.0f, 0.0f, -32.0f);
float escalaPlataforma = 4.0f;
int pisosTorre = 5;
glm::mat4 modelauxPlataforma(1.0);
//despegue del cohete con la F: estadoCohete dice en que parte va, 0 parado, 1 cuenta regresiva, 2 subiendo y 3 bajando
//las alturas estan en las unidades de Kenney, asi que se multiplican por escalaPlataforma (con 4, subir 60 son 240 unidades)
int estadoCohete = 0;
float tiempoCuentaCohete = 3.0f; //la cuenta regresiva dura 3 segundos
float cuentaCohete = 0.0f;       //segundos que le faltan a la cuenta
float alturaCohete = 0.0f;       //cuanto ha subido el cohete desde la plataforma
float velocidadCohete = 0.0f;
float aceleracionCohete = 0.002f; //lo que crece la velocidad cada frame, asi sube cada vez mas rapido
float alturaMaxCohete = 60.0f;    //al llegar aqui deja de subir y regresa
float velBajadaCohete = 0.15f;    //regresa a velocidad constante y despacio, como aterrizando
float fuegoCohete = 0.0f;         //tamano de la llama: 0 apagada y 1 completa
//Gordon y el Capitan Falcon esperan junto a la plataforma como astronautas, del lado de donde llega la camara;
//su lugar tambien se mide desde el centro del planeta 2
glm::vec3 posGordonAstronauta = glm::vec3(-27.0f, 0.0f, -16.0f);
glm::vec3 posFalconAstronauta = glm::vec3(-23.0f, 0.0f, -17.0f);
float escalaGordonAstronauta = 1.5f; //igual que el avatar; Gordon mide 1.4 y queda de 2.1
float escalaFalconAstronauta = 1.8f; //el Capitan Falcon mide 0.95 y queda de 1.7, un poco mas bajo que Gordon


//--- estacion espacial y naves ---
//la estacion tiene cuatro landing pads alrededor, a 150 de su centro hacia X positivo, X negativo, Z positivo y Z negativo,
//y en cada pad hay naves que salen y regresan solas; hay tres modelos de nave y cada lugar usa uno
//modelauxEstacion guarda el lugar de la estacion, y desde ahi se ponen los pads y las naves
Model Estacion_M;
Model LandingPad_M;
Model NaveDorada_M;
Model NaveBlanca_M;
Model NaveCaza_M;
glm::vec3 posEstacion = glm::vec3(613.0f, -5.0f, -350.0f);
glm::mat4 modelauxEstacion(1.0);

//cada nave pasa por 8 estados que se repiten; en el 0 espera estacionada, en el 1 se eleva, en el 2 gira 180 grados
//para quedar viendo hacia afuera, en el 3 sale a hipervelocidad hasta perderse, en el 4 espera fuera de la escena
//sin dibujarse, en el 5 regresa a hipervelocidad, en el 6 se acerca despacio a su pad y en el 7 baja hasta posarse
float alturaNaveEstacion = 7.5f; //lo que sube sobre su pad antes de girar
float distanciaFrenado = 50.0f;  //a esta distancia de su pad deja la hipervelocidad y se acerca despacio
float distanciaSalida = 1800.0f; //a esta distancia de su pad ya no se alcanza a ver y deja de dibujarse
float velSubidaNave = 0.08f;
float velGiroNave = 2.0f;
float velHipervelocidad = 25.0f;
float velAcercamiento = 0.3f;
//las esperas van en segundos; a la minima se le suma la que le toque de esperasNaves, que esta en DatosEscena.h
float tiempoMinEstacionada = 10.0f;
float tiempoMinFuera = 15.0f;

//lo que va cambiando de cada nave; su pad, hacia donde sale y su modelo estan en DatosEscena.h
int estadoNavesEstacion[NUM_NAVES_ESTACION];    //en que estado va, del 0 al 7
float subidaNavesEstacion[NUM_NAVES_ESTACION];  //cuanto se ha elevado sobre su pad
float avanceNavesEstacion[NUM_NAVES_ESTACION];  //cuanto se ha alejado de su pad hacia afuera
float vueltaNavesEstacion[NUM_NAVES_ESTACION];  //lo que ha girado para salir, de 0 a 180
float esperaNavesEstacion[NUM_NAVES_ESTACION];  //segundos que le faltan para salir o para regresar
bool visibleNavesEstacion[NUM_NAVES_ESTACION];  //vale false cuando ya salio de la escena, y asi no se dibuja
int turnoEsperaNaves[NUM_NAVES_ESTACION];       //lugar de esperasNaves que le toca la siguiente vez

//regresa la espera que le toca a la nave y la deja apuntando a la siguiente de la lista
float SiguienteEspera(int nave)
{
	float espera = esperasNaves[turnoEsperaNaves[nave]];
	turnoEsperaNaves[nave] = (turnoEsperaNaves[nave] + 1) % NUM_ESPERAS_NAVES;
	return espera;
}

//pone cada nave en su pad; se llama una vez en la carga de recursos
//cada nave empieza en un lugar distinto de esperasNaves para que no salgan juntas
void InicializarNavesEstacion()
{
	for (int i = 0; i < NUM_NAVES_ESTACION; i++)
	{
		estadoNavesEstacion[i] = 0;
		subidaNavesEstacion[i] = 0.0f;
		avanceNavesEstacion[i] = 0.0f;
		vueltaNavesEstacion[i] = 0.0f;
		visibleNavesEstacion[i] = true;
		turnoEsperaNaves[i] = i % NUM_ESPERAS_NAVES;
		esperaNavesEstacion[i] = tiempoMinEstacionada + SiguienteEspera(i);
	}
}

//--- avatar ---
//el avatar puede ser Kirby o la carnivora y la T cambia entre los dos; Kirby usa sus piezas y sus padres, pero con sus
//propias rotaciones y sus propios pivotes, que estan en DatosEscena.h
glm::vec3 rotAvatar[NUM_PIEZAS_KIRBY];
//lugar del avatar medido desde el centro del piso del planeta donde esta, y hacia donde mira en grados alrededor de Y
//con rumbo 0 ve hacia Z positivo y con 180 hacia Z negativo, que es hacia donde estan las cosas de cada planeta
glm::vec3 posLlegadaAvatar = glm::vec3(0.0f, 0.0f, 6.0f);
glm::vec3 posAvatar = posLlegadaAvatar;
float rumboAvatar = 180.0f;
float escalaAvatar = 1.5f;
float velocidadAvatar = 0.08f;
float giroAvatarOffset = 2.5f;
//como el piso mide 200 por 200 y llega a 100, el avatar se detiene un poco antes de la orilla
float limitePisoAvatar = 95.0f;
//mientras camina, los pies y las manos van y vienen entre -limitePasoAvatar y limitePasoAvatar, como las patas del headcrab
float pasoAvatar = 0.0f;
float pasoAvatarOffset = 3.0f;
float limitePasoAvatar = 25.0f;
bool pasoAdelanteAvatar = true;
//la carnivora usa sus piezas, padres y pivotes de siempre, pero con sus propias rotaciones, igual que rotAvatar con Kirby
glm::vec3 rotAvatarCarnivora[NUM_PIEZAS_CARNIVORA];
//la carnivora ya mide como 1.4 y Kirby con su escala como 1.1, asi que ella se dibuja de su tamano para no tapar la camara 2
float escalaAvatarCarnivora = 1.0f;
//se comparan con la zona y el reinicio de Window para regresar al avatar al punto de llegada cuando cambian
int zonaAvatar = 0;
int reinicioAvatar = 0;

//--- camaras y recorridos ---
//zona donde esta la camara, 0 en el espacio, 1 en Xen, 2 en el planeta 2 y 3 en Verdania; se compara con la de Window
//para saber cuando cambio
int zonaActual = 0;
//se compara con el contador de Window para saber cuando se presiono la tecla del planeta donde ya se estaba
int reinicioPlanetaActual = 0;
//camara que se uso en el frame anterior; se compara con la de Window para saber cuando cambio
int modoCamaraActual = 1;
//se compara con el contador de Window para saber cuando se presiono la Z estando en el espacio
int puntoEspacioActual = 0;
//posicion y direccion de la camara que se esta usando, sea cual sea; las usan la luz especular, la linterna y el sol, que se dibuja lejos de ella
glm::vec3 posVista = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 dirVista = glm::vec3(0.0f, 0.0f, -1.0f);
//camara 2, que va detras y arriba del avatar y mira un poco arriba de sus pies
float distanciaCamaraAvatar = 4.0f;
float alturaCamaraAvatar = 2.2f;
float alturaMiraAvatar = 0.8f;
//camara 3, que va detras de una nave de la carrera; los dos puntos se miden desde la nave, que ve hacia Z negativo,
//asi que la camara va hacia Z positivo, que es atras de la nave, y mira hacia enfrente de ella
glm::vec3 camaraNaveAtras = glm::vec3(0.0f, 3.5f, 10.0f);
glm::vec3 camaraNaveMira = glm::vec3(0.0f, 1.5f, -8.0f);

//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---
//en los planetas el sol da una vuelta completa en unos 2 minutos; el angulo avanza 0.05 por cada unidad de deltaTime
float anguloSol = 60.0f;
float anguloSolOffset = 0.05f;
bool esDeNoche = false;
//en el espacio el sol no se mueve; esta 45 grados arriba del centro, viendo hacia Z negativo
glm::vec3 haciaSolEspacio = glm::vec3(0.0f, 0.707f, -0.707f);
//direccion de donde viene el sol en la zona actual y su posicion para dibujarlo lejos de la camara
glm::vec3 haciaSol = haciaSolEspacio;
glm::vec3 posSol = glm::vec3(0.0f, 0.0f, 0.0f);
//tiene que ser menor que el plano lejano de la proyeccion, que esta en 3000, si no, no se ve, y mayor que la distancia
//a los planetas del espacio para que quede detras de ellos
float distanciaSol = 2500.0f;
//la N salta a la manana si es de noche o a la noche si es de dia; el ciclo sigue corriendo desde ahi
int saltoDiaNocheActual = 0;
float anguloManana = 45.0f;
float anguloNoche = 225.0f;
//el sol y la luna son la misma esfera con distinta textura; se dibujan lejos para que parezca que de ahi sale la luz
//como estan tan lejos, se escalan mucho para que se vean del tamano que se quiere
Model Sol_M;
Model Luna_M;
float tamanoSol = 140.0f;
float tamanoLuna = 78.0f;

//la linterna se guarda aparte y solo se copia al arreglo de spotlights cuando esta prendida
SpotLight linternaLuz;

//luces de cada zona; en el while solo se mandan al shader las de la zona donde se esta,
//asi ninguna zona pasa de MAX_POINT_LIGHTS ni de MAX_SPOT_LIGHTS aunque entre todas haya mas
//las luminarias de los planetas se prenden de noche; las del espacio siempre estan prendidas porque ahi no hay ciclo
const int NUM_LUMINARIAS_ESPACIO = 5;
PointLight luminariasEspacio[NUM_LUMINARIAS_ESPACIO];
const int NUM_LUMINARIAS_XEN = 3;
PointLight luminariasXen[NUM_LUMINARIAS_XEN];
PointLight luminariaPlaneta2;
PointLight luminariaVerdania;
//spotlights de cada zona, se prenden y apagan con la K
const int NUM_SPOTS_ESPACIO = 4;
SpotLight spotsEspacio[NUM_SPOTS_ESPACIO];
SpotLight spotXen;
SpotLight spotPlaneta2;
SpotLight spotVerdania;


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


//funciones con la geometria de cada actividad, que se llaman en la carga de recursos dentro de su seccion
//como el indice en meshListModel depende del orden en que se agregan, se guarda en una variable global antes
//del push_back, por ejemplo idxCohete = meshListModel.size();, y se dibuja con meshListModel[idxCohete]

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
	CreateShaders();

	//Camera(posicion inicial, vector up, yaw, pitch, velocidad de movimiento, velocidad de giro)
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	// ================== 3. CARGA DE RECURSOS ==================

	//en las texturas primero se asigna la ruta y luego se carga
	//LoadTextureA es para imagenes con canal alfa, como png o tga, y LoadTexture para las que no tienen, como jpg

	//las 6 caras del skybox van en este orden: rt, lf, dn, up, bk, ft
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/sBRight.png");//derecha, hacia X positivo
	skyboxFaces.push_back("Textures/Skybox/Left.png");//izquierda, hacia X negativo
	skyboxFaces.push_back("Textures/Skybox/DownF.png");//abajo, hacia Y negativo
	skyboxFaces.push_back("Textures/Skybox/Top.png");//arriba, hacia Y positivo
	skyboxFaces.push_back("Textures/Skybox/FrontF.png");//atras, hacia Z positivo
	skyboxFaces.push_back("Textures/Skybox/BackF.png");//enfrente, hacia Z negativo

	skybox = Skybox(skyboxFaces);

	//los skyboxes de los planetas usan el mismo orden de caras y sus archivos solo cambian en el principio del nombre
	std::string carasSkybox[6] = { "rt", "lf", "dn", "up", "bk", "ft" };

	//materiales con Material(intensidad especular, shininess)
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);
	//el metal brilla mas que el opaco y con un reflejo mas grande que el brillante; lo usan las naves
	Material_metal = Material(2.0f, 64);
	//el del avatar da un brillo debil y extendido, como de goma, para que el avatar reaccione a las luces sin verse metalico
	Material_avatar = Material(0.4f, 8);

	//--- personajes ---
	CapitanFalcon_M = Model();
	CapitanFalcon_M.LoadModel("Models/capitanFalcon/capitanFalcon.obj");

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

	//--- planetas: Xen y planeta 2 ---
	//Xen
	std::vector<std::string> carasXenDia;
	std::vector<std::string> carasXenNoche;
	for (int i = 0; i < 6; i++)
	{
		carasXenDia.push_back("Textures/Skybox/xen_dia_" + carasSkybox[i] + ".png");
		carasXenNoche.push_back("Textures/Skybox/xen_" + carasSkybox[i] + ".png");
	}
	skyboxXenDia = Skybox(carasXenDia);
	skyboxXenNoche = Skybox(carasXenNoche);

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

	for (int i = 0; i < NUM_PIEZAS_HEADCRAB; i++)
	{
		std::string ruta = std::string("Models/Headcrab/headcrab_") + piezasHeadcrab[i] + ".obj";
		Headcrab_M[i] = Model();
		Headcrab_M[i].LoadModel(ruta);
	}

	//planeta 2
	std::vector<std::string> carasPlaneta2Dia;
	std::vector<std::string> carasPlaneta2Noche;
	for (int i = 0; i < 6; i++)
	{
		carasPlaneta2Dia.push_back("Textures/Skybox/planeta2_dia_" + carasSkybox[i] + ".png");
		carasPlaneta2Noche.push_back("Textures/Skybox/planeta2_" + carasSkybox[i] + ".png");
	}
	skyboxPlaneta2Dia = Skybox(carasPlaneta2Dia);
	skyboxPlaneta2Noche = Skybox(carasPlaneta2Noche);

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

	//--- planeta 3: Verdania ---
	std::vector<std::string> carasVerdaniaDia;
	std::vector<std::string> carasVerdaniaNoche;
	for (int i = 0; i < 6; i++)
	{
		carasVerdaniaDia.push_back("Textures/Skybox/verdania_" + carasSkybox[i] + ".png");
		carasVerdaniaNoche.push_back("Textures/Skybox/verdania_noche_" + carasSkybox[i] + ".png");
	}
	skyboxVerdaniaDia = Skybox(carasVerdaniaDia);
	skyboxVerdaniaNoche = Skybox(carasVerdaniaNoche);

	verdaniaPastoTexture = Texture("Textures/verdania_pasto.png");
	verdaniaPastoTexture.LoadTextureA();

	PlanetaVerdania_M = Model();
	PlanetaVerdania_M.LoadModel("Models/Peashooter/peashooter_planeta.obj");

	for (int i = 0; i < NUM_PIEZAS_PEASHOOTER; i++)
	{
		std::string ruta = std::string("Models/Peashooter/peashooter_") + piezasPeashooter[i] + ".obj";
		Peashooter_M[i] = Model();
		Peashooter_M[i].LoadModel(ruta);
	}

	for (int i = 0; i < NUM_PIEZAS_CRAZYDAVE; i++)
	{
		std::string ruta = std::string("Models/CrazyDave/crazydave_") + piezasCrazyDave[i] + ".obj";
		CrazyDave_M[i] = Model();
		CrazyDave_M[i].LoadModel(ruta);
	}

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

	//--- nave de Among Us y crewmates ---
	AmongUsNave_M = Model();
	AmongUsNave_M.LoadModel("Models/AmongUs/amongus_nave.obj");
	CrewmateCuerpo_M = Model();
	CrewmateCuerpo_M.LoadModel("Models/AmongUs/amongus_crewmate_cuerpo.obj");
	CrewmateVisor_M = Model();
	CrewmateVisor_M.LoadModel("Models/AmongUs/amongus_crewmate_visor.obj");

	//--- pista de carreras y naves ---
	Pista_M = Model();
	Pista_M.LoadModel("Models/pista/pista.obj");
	NavePista_M = Model();
	NavePista_M.LoadModel("Models/nave1Race/nave1Race.obj");
	Meta_M = Model();
	Meta_M.LoadModel("Models/meta/meta.obj");
	CubiertaMeta_M = Model();
	CubiertaMeta_M.LoadModel("Models/cubiertaMeta/cubiertaMeta.obj");
	PuestoObservacion1_M = Model();
	PuestoObservacion1_M.LoadModel("Models/puestoObservacion1/puestoObservacion1.obj");
	PuestoObservacion2_M = Model();
	PuestoObservacion2_M.LoadModel("Models/puestoObservacion2/puestoObservacion2.obj");
	PuestoObservacion3_M = Model();
	PuestoObservacion3_M.LoadModel("Models/puestoObservacion3/puestoObservacion3.obj");
	PuestoObservacion4_M = Model();
	PuestoObservacion4_M.LoadModel("Models/puestoObservacion4/puestoObservacion4.obj");
	Almacen_M = Model();
	Almacen_M.LoadModel("Models/warehousePort/warehousePort.obj");
	NaveNodriza_M = Model();
	NaveNodriza_M.LoadModel("Models/barcoEspacial/barcoEspacial.obj");

	InicializarRutaPista();

	//cada nave esta en su propia carpeta, con el mismo nombre que su .obj
	for (int i = 0; i < NUM_NAVES_FZERO; i++)
	{
		std::string ruta = std::string("Models/") + nombresNavesFZero[i] + "/" + nombresNavesFZero[i] + ".obj";
		NavesFZero_M[i] = Model();
		NavesFZero_M[i].LoadModel(ruta);
	}

	//--- satelites y cuerpos espaciales ---
	Satelite_M = Model();
	Satelite_M.LoadModel("Models/sateliteRace/sateliteRace.obj");

	//--- plataforma de lanzamiento y cohete ---
	//las rutas de las piezas del cohete se arman con su nombre, igual que las plantas: Models/Cohete/cohete_base.obj, ...
	for (int i = 0; i < NUM_PIEZAS_COHETE; i++)
	{
		std::string ruta = std::string("Models/Cohete/cohete_") + piezasCohete[i] + ".obj";
		Cohete_M[i] = Model();
		Cohete_M[i].LoadModel(ruta);
	}
	PlataformaCohete_M = Model();
	PlataformaCohete_M.LoadModel("Models/Cohete/plataforma.obj");
	TorreCohete_M = Model();
	TorreCohete_M.LoadModel("Models/Cohete/torre_estructura.obj");


	//--- estacion espacial y naves ---
	Estacion_M = Model();
	Estacion_M.LoadModel("Models/estacionEspacial/estacionEspacial.obj");
	LandingPad_M = Model();
	LandingPad_M.LoadModel("Models/landingPad/landingPad.obj");
	NaveDorada_M = Model();
	NaveDorada_M.LoadModel("Models/naveDorada/naveDorada.obj");
	NaveBlanca_M = Model();
	NaveBlanca_M.LoadModel("Models/naveBlanca/naveBlanca.obj");
	NaveCaza_M = Model();
	NaveCaza_M.LoadModel("Models/naveCaza/naveCaza.obj");

	InicializarNavesEstacion();

	//--- avatar ---
	//las piezas de Kirby se cargan en la seccion de personajes

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

	//contadores de luces, no pueden pasar de MAX_POINT_LIGHTS ni de MAX_SPOT_LIGHTS
	//en el while se vuelven a 0 y se llenan los arreglos con las luces de la zona donde se esta
	unsigned int pointLightCount = 0;
	unsigned int spotLightCount = 0;

	//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---
	Sol_M = Model();
	Sol_M.LoadModel("Models/Sol/sol.obj");
	Luna_M = Model();
	Luna_M.LoadModel("Models/Sol/luna.obj");

	//la posicion y la direccion de la linterna se actualizan en el while con la camara
	linternaLuz = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);

	//en el espacio hay dos faros rojos a los lados de la entrada de la meta y una luz azul arriba de la nave de Among Us
	//la cubierta de la meta llega hasta 52 en Z, asi que los faros van un poco antes, del lado de donde llegan las naves
	luminariasEspacio[0] = PointLight(1.0f, 0.15f, 0.1f,
		0.1f, 2.5f,
		posPista.x - 12.0f, posPista.y + 8.0f, posPista.z + 58.0f,
		1.0f, 0.02f, 0.002f);
	luminariasEspacio[1] = PointLight(1.0f, 0.15f, 0.1f,
		0.1f, 2.5f,
		posPista.x + 12.0f, posPista.y + 8.0f, posPista.z + 58.0f,
		1.0f, 0.02f, 0.002f);
	luminariasEspacio[2] = PointLight(0.4f, 0.7f, 1.0f,
		0.1f, 1.5f,
		posNaveAmongUs.x, posNaveAmongUs.y + 5.0f, posNaveAmongUs.z,
		1.0f, 0.05f, 0.01f);

	//en Xen cada planta de luz alumbra desde su foco, que esta en la punta del tallo
	for (int i = 0; i < NUM_LUMINARIAS_XEN; i++)
	{
		glm::vec3 pos = posXen + posPlantas[i] + glm::vec3(0.0f, altoTallo, 0.0f);
		luminariasXen[i] = PointLight(1.0f, 0.85f, 0.4f,
			0.1f, 2.5f,
			pos.x, pos.y, pos.z,
			1.0f, 0.1f, 0.02f);
	}

	//en el planeta 2 la warp star brilla amarillo
	luminariaPlaneta2 = PointLight(1.0f, 0.9f, 0.3f,
		0.1f, 1.5f,
		posPlaneta2.x + posWarpStar.x, posPlaneta2.y + posWarpStar.y + 1.5f, posPlaneta2.z + posWarpStar.z,
		1.0f, 0.1f, 0.02f);

	//en Verdania alumbra el foco del farol, que cuelga del brazo del poste hacia X negativo
	luminariaVerdania = PointLight(1.0f, 0.85f, 0.6f,
		0.1f, 3.0f,
		posVerdania.x + posCalleX[0] - 6.3f, posVerdania.y + 4.2f, posVerdania.z + posCalleZ - 4.0f,
		1.0f, 0.05f, 0.01f);

	//los spotlights del espacio son dos luces blancas que apuntan hacia abajo a la pista, antes de la entrada de la meta
	spotsEspacio[0] = SpotLight(1.0f, 1.0f, 0.9f,
		0.0f, 3.0f,
		posPista.x - 5.0f, posPista.y + 20.0f, posPista.z + 75.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.01f, 0.001f,
		30.0f);
	spotsEspacio[1] = SpotLight(1.0f, 1.0f, 0.9f,
		0.0f, 3.0f,
		posPista.x + 5.0f, posPista.y + 20.0f, posPista.z + 75.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.01f, 0.001f,
		30.0f);

	//tambien hay dos faros rojos arriba de los edificios redondos de la pista y dos spotlights blancos en las torres altas
	//su lugar sale de multiplicar un punto del edificio por la matriz del edificio, que se arma igual que en el while
	glm::mat4 matrizPista = glm::mat4(1.0f);
	matrizPista = glm::translate(matrizPista, posPista);
	matrizPista = glm::scale(matrizPista, glm::vec3(0.45f, 0.45f, 0.45f));
	matrizPista = glm::scale(matrizPista, glm::vec3(-1.0f, 1.0f, -1.0f));

	//el faro va a 76 de altura en el edificio redondo, un poco arriba de su esfera para que no quede tapado
	glm::mat4 edificio = glm::translate(matrizPista, glm::vec3(175.0f, 80.0f, 500.0f));
	edificio = glm::rotate(edificio, -45.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 posLuz = glm::vec3(edificio * glm::vec4(0.0f, 76.0f, 0.0f, 1.0f));
	luminariasEspacio[3] = PointLight(1.0f, 0.12f, 0.08f,
		0.15f, 4.0f,
		posLuz.x, posLuz.y, posLuz.z,
		1.0f, 0.008f, 0.0002f);

	edificio = glm::translate(matrizPista, glm::vec3(175.0f, 80.0f, -375.0f));
	edificio = glm::rotate(edificio, -115.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	posLuz = glm::vec3(edificio * glm::vec4(0.0f, 76.0f, 0.0f, 1.0f));
	luminariasEspacio[4] = PointLight(1.0f, 0.12f, 0.08f,
		0.15f, 4.0f,
		posLuz.x, posLuz.y, posLuz.z,
		1.0f, 0.008f, 0.0002f);

	//el spotlight va en la punta de la torre, a 374 de altura, y apunta hacia abajo y hacia la recta de la meta
	//la direccion no tiene que medir 1 porque SpotLight la normaliza
	edificio = glm::translate(matrizPista, glm::vec3(0.0f, 20.0f, -600.0f));
	edificio = glm::rotate(edificio, 35.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	edificio = glm::scale(edificio, glm::vec3(0.5f, 0.5f, 0.5f));
	posLuz = glm::vec3(edificio * glm::vec4(0.0f, 374.0f, 8.0f, 1.0f));
	spotsEspacio[2] = SpotLight(1.0f, 1.0f, 0.95f,
		0.0f, 6.0f,
		posLuz.x, posLuz.y, posLuz.z,
		0.10f, -0.65f, -0.75f,
		1.0f, 0.003f, 0.00005f,
		28.0f);

	edificio = glm::translate(matrizPista, glm::vec3(300.0f, 100.0f, -600.0f));
	edificio = glm::rotate(edificio, -35.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	edificio = glm::scale(edificio, glm::vec3(0.5f, 0.5f, 0.5f));
	posLuz = glm::vec3(edificio * glm::vec4(0.0f, 374.0f, 8.0f, 1.0f));
	spotsEspacio[3] = SpotLight(1.0f, 1.0f, 0.95f,
		0.0f, 6.0f,
		posLuz.x, posLuz.y, posLuz.z,
		-0.18f, -0.60f, -0.78f,
		1.0f, 0.003f, 0.00005f,
		28.0f);

	//spotlight morado sobre el cristal mas grande de Xen
	spotXen = SpotLight(0.7f, 0.3f, 1.0f,
		0.0f, 2.0f,
		posXen.x - 10.0f, posXen.y + 8.0f, posXen.z - 3.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.05f, 0.005f,
		25.0f);

	//spotlight rosa que alumbra la warp star desde arriba
	spotPlaneta2 = SpotLight(1.0f, 0.5f, 0.8f,
		0.0f, 2.0f,
		posPlaneta2.x + posWarpStar.x, posPlaneta2.y + 8.0f, posPlaneta2.z + posWarpStar.z,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.05f, 0.005f,
		25.0f);

	//spotlight que alumbra desde arriba el centro del jardin, donde estan las plantas
	spotVerdania = SpotLight(1.0f, 0.9f, 0.7f,
		0.0f, 3.0f,
		posVerdania.x, posVerdania.y + 12.0f, posVerdania.z - 8.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.02f, 0.002f,
		35.0f);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	//el angulo de glm::perspective va en radianes, por eso 45.0f da una vista de unos 58 grados y no de 45
	//el plano lejano esta en 3000 para que desde cualquier parte del espacio se vean los planetas, que estan lejos
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 3000.0f);

	//la carga tarda varios segundos; si lastTime se quedara en 0, el primer frame tendria un deltaTime enorme
	//y todas las animaciones darian un salto, por eso se toma la hora justo antes de empezar
	lastTime = glfwGetTime();

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
		//la camara libre solo se mueve cuando es la que se esta usando; la 4 y la 5 la usan mientras no existan sus recorridos
		//con las demas camaras el mouse se lee igual y no se usa, para que la libre no brinque al regresar a ella
		if (mainWindow.getmodoCamara() == 1 || mainWindow.getmodoCamara() == 4 || mainWindow.getmodoCamara() == 5)
		{
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}
		else
		{
			mainWindow.getXChange();
			mainWindow.getYChange();
		}

		//matriz de vista de la camara libre; con otras camaras se cambia en su seccion
		glm::mat4 view = camera.calculateViewMatrix();

		//con P se pausan todas las animaciones; como todas avanzan con deltaTime, basta con ponerlo en 0
		//deltaReal guarda el tiempo del frame sin pausa para el avatar, que se tiene que poder mover aunque todo lo demas este quieto
		GLfloat deltaReal = deltaTime;
		if (mainWindow.getpausaAnimaciones())
			deltaTime = 0.0f;

		//--- camaras y recorridos ---
		//al regresar a la camara libre desde la del avatar o la de la nave, la libre aparece donde estaba la otra y viendo hacia lo mismo
		//el yaw y el pitch salen de la direccion con atan2 y asin, al reves de como Camera::update saca la direccion a partir de ellos
		if (mainWindow.getmodoCamara() != modoCamaraActual)
		{
			if (mainWindow.getmodoCamara() == 1 && (modoCamaraActual == 2 || modoCamaraActual == 3))
			{
				float yaw = atan2(dirVista.z, dirVista.x) / toRadians;
				float pitch = asin(dirVista.y) / toRadians;
				camera = Camera(posVista, glm::vec3(0.0f, 1.0f, 0.0f), yaw, pitch, 0.3f, 0.5f);
				view = camera.calculateViewMatrix();
			}
			modoCamaraActual = mainWindow.getmodoCamara();
		}

		//cuando una tecla cambia la zona en Window, la camara libre se vuelve a crear en la otra zona, y tambien cuando
		//se vuelve a presionar la tecla del planeta donde ya se esta, para regresar al punto de llegada
		if (mainWindow.getzonaPlaneta() != zonaActual || mainWindow.getreinicioPlaneta() != reinicioPlanetaActual)
		{
			zonaActual = mainWindow.getzonaPlaneta();
			reinicioPlanetaActual = mainWindow.getreinicioPlaneta();
			if (zonaActual == 1)
				camera = Camera(posXen + glm::vec3(0.0f, 1.5f, 8.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.3f, 0.5f);
			else if (zonaActual == 2)
				camera = Camera(posPlaneta2 + glm::vec3(0.0f, 1.5f, 8.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.3f, 0.5f);
			else if (zonaActual == 3)
				camera = Camera(posVerdania + glm::vec3(0.0f, 1.5f, 8.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.3f, 0.5f);
			else
				camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);
			view = camera.calculateViewMatrix();
		}

		//la vista empieza siendo la de la camara libre y mas abajo se cambia si se usa la del avatar o la de la nave
		posVista = camera.getCameraPosition();
		dirVista = camera.getCameraDirection();

		//estando en el espacio, la Z lleva al siguiente punto de interes, que son el inicio, la nave nodriza y la pista, y despues regresa al inicio
		//la camara se vuelve a crear en el lugar nuevo, igual que al cambiar de planeta
		if (mainWindow.getpuntoEspacio() != puntoEspacioActual)
		{
			puntoEspacioActual = mainWindow.getpuntoEspacio();
			int punto = puntoEspacioActual % 3;
			if (punto == 1)
			{
				//del lado de la nave nodriza que da hacia X negativo, a la altura de su cubierta y viendo hacia X positivo,
				//asi sale completa y atras de ella la estacion
				camera = Camera(glm::vec3(-250.0f, 120.0f, -40.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, -5.0f, 0.3f, 0.5f);
			}
			else if (punto == 2)
			{
				//atras de la meta y un poco arriba, viendo hacia Z negativo, que es hacia donde salen las naves
				camera = Camera(posPista + glm::vec3(0.0f, 40.0f, 120.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -15.0f, 0.3f, 0.5f);
			}
			else
			{
				//el mismo lugar donde empieza el programa
				camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);
			}
			view = camera.calculateViewMatrix();
			posVista = camera.getCameraPosition();
			dirVista = camera.getCameraDirection();
		}

		//el avatar se mueve aqui y no en su seccion para que la camara 2 ya use el lugar donde queda en este frame
		//si cambia de planeta o se vuelve a presionar la tecla del planeta donde esta, regresa al punto de llegada
		if (mainWindow.getzonaPlaneta() != zonaAvatar || mainWindow.getreinicioPlaneta() != reinicioAvatar)
		{
			zonaAvatar = mainWindow.getzonaPlaneta();
			reinicioAvatar = mainWindow.getreinicioPlaneta();
			posAvatar = posLlegadaAvatar;
			rumboAvatar = 180.0f;
		}
		//el piso del planeta donde esta el avatar; en el espacio no se dibuja, asi que ahi no importa
		glm::vec3 centroAvatar = posXen;
		if (zonaAvatar == 2)
			centroAvatar = posPlaneta2;
		else if (zonaAvatar == 3)
			centroAvatar = posVerdania;

		//con la camara 2, W y S lo mueven hacia enfrente y hacia atras, y A y D lo giran; Shift lo hace ir al doble de rapido
		//su frente sale del seno y el coseno del rumbo, y como nunca cambia su altura, siempre camina sobre el piso
		bool avatarCamina = false;
		if (mainWindow.getmodoCamara() == 2)
		{
			bool* teclas = mainWindow.getsKeys();
			float velocidad = velocidadAvatar * deltaReal;
			if (teclas[GLFW_KEY_LEFT_SHIFT])
				velocidad = velocidad * 2.0f;

			if (teclas[GLFW_KEY_A])
				rumboAvatar += giroAvatarOffset * deltaReal;
			if (teclas[GLFW_KEY_D])
				rumboAvatar -= giroAvatarOffset * deltaReal;
			glm::vec3 frenteAvatar = glm::vec3(sin(rumboAvatar * toRadians), 0.0f, cos(rumboAvatar * toRadians));

			if (teclas[GLFW_KEY_W])
			{
				posAvatar += frenteAvatar * velocidad;
				avatarCamina = true;
			}
			if (teclas[GLFW_KEY_S])
			{
				posAvatar -= frenteAvatar * velocidad;
				avatarCamina = true;
			}

			//no puede salirse del piso
			if (posAvatar.x > limitePisoAvatar)
				posAvatar.x = limitePisoAvatar;
			if (posAvatar.x < -limitePisoAvatar)
				posAvatar.x = -limitePisoAvatar;
			if (posAvatar.z > limitePisoAvatar)
				posAvatar.z = limitePisoAvatar;
			if (posAvatar.z < -limitePisoAvatar)
				posAvatar.z = -limitePisoAvatar;

			//la camara 2 va atras del avatar y a una altura fija, asi que se mueve en un plano paralelo al piso
			glm::vec3 posAvatarMundo = centroAvatar + posAvatar;
			glm::vec3 puntoMira = posAvatarMundo + glm::vec3(0.0f, alturaMiraAvatar, 0.0f);
			posVista = posAvatarMundo - frenteAvatar * distanciaCamaraAvatar + glm::vec3(0.0f, alturaCamaraAvatar, 0.0f);
			dirVista = glm::normalize(puntoMira - posVista);
			view = glm::lookAt(posVista, puntoMira, glm::vec3(0.0f, 1.0f, 0.0f));
		}

		//la camara 3 va detras de una de las naves de la carrera, y Q y E cambian de nave
		//la nave se coloca con la misma funcion y el mismo carril y distancia que usa la seccion de la pista
		//como esa seccion mueve la carrera mas abajo, aqui se le suma lo que va a avanzar en este frame para que la camara no se quede atras
		if (mainWindow.getmodoCamara() == 3)
		{
			//el resto de dividir entre el numero de naves sale negativo si objetivoCamara es negativo, por eso se le suma una vuelta
			int naveSeguida = mainWindow.getobjetivoCamara() % NUM_NAVES_FZERO;
			if (naveSeguida < 0)
				naveSeguida += NUM_NAVES_FZERO;
			float carrilX = 2.15f;
			if (naveSeguida % 2 == 0)
				carrilX = -5.15f;
			float desfaseAtras = -10.0f * (naveSeguida + 1);
			glm::mat4 matrizNave = modelauxNaves * ObtenerMatrizNavePista(avanceCarrera + velocidadCarrera * deltaTime + desfaseAtras, carrilX, 1.0f);

			//los puntos de la camara se pasan de las coordenadas de la nave a las del mundo multiplicandolos por su matriz
			//la segunda columna de la matriz es el eje Y de la nave, que se usa como arriba para que la camara se incline con la pista
			posVista = glm::vec3(matrizNave * glm::vec4(camaraNaveAtras, 1.0f));
			glm::vec3 puntoMira = glm::vec3(matrizNave * glm::vec4(camaraNaveMira, 1.0f));
			glm::vec3 arribaNave = glm::normalize(glm::vec3(matrizNave[1]));
			dirVista = glm::normalize(puntoMira - posVista);
			view = glm::lookAt(posVista, puntoMira, arribaNave);
		}

		//limpiar la pantalla
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//el skybox va primero, antes que todo lo demas, y cada planeta tiene el suyo segun zonaActual, que cambia con Z, X, C y V
		//el del espacio gira en los tres ejes; los de los planetas solo sobre Y para que el horizonte no se incline
		//cada planeta tiene uno de dia y uno de noche, y se escoge con esDeNoche, que se calcula en la seccion de iluminacion
		if (zonaActual == 1 && esDeNoche)
			skyboxXenNoche.DrawSkybox(view, projection, deltaTime, false);
		else if (zonaActual == 1)
			skyboxXenDia.DrawSkybox(view, projection, deltaTime, false);
		else if (zonaActual == 2 && esDeNoche)
			skyboxPlaneta2Noche.DrawSkybox(view, projection, deltaTime, false);
		else if (zonaActual == 2)
			skyboxPlaneta2Dia.DrawSkybox(view, projection, deltaTime, false);
		else if (zonaActual == 3 && esDeNoche)
			skyboxVerdaniaNoche.DrawSkybox(view, projection, deltaTime, false);
		else if (zonaActual == 3)
			skyboxVerdaniaDia.DrawSkybox(view, projection, deltaTime, false);
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
		//posVista es la posicion de la camara que se este usando, que puede no ser la libre
		glUniform3f(uniformEyePosition, posVista.x, posVista.y, posVista.z);

		//linterna pegada a la camara que se este usando, un poco abajo de los ojos
		glm::vec3 lowerLight = posVista;
		lowerLight.y -= 0.3f;
		linternaLuz.SetFlash(lowerLight, dirVista);

		//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---
		int zonaLuces = mainWindow.getzonaPlaneta();
		if (zonaLuces == 0)
		{
			//en el espacio el sol esta fijo y alumbra igual todo el tiempo
			haciaSol = haciaSolEspacio;
			esDeNoche = false;
			mainLight = DirectionalLight(1.0f, 0.95f, 0.85f,
				0.3f, 0.7f,
				-haciaSol.x, -haciaSol.y, -haciaSol.z);
		}
		else
		{
			//si se presiono la N, el sol salta a la manana o a la noche y desde ahi sigue el ciclo
			if (mainWindow.getsaltoDiaNoche() != saltoDiaNocheActual)
			{
				saltoDiaNocheActual = mainWindow.getsaltoDiaNoche();
				if (esDeNoche)
				{
					anguloSol = anguloManana;
					printf("ahora es de dia\n");
				}
				else
				{
					anguloSol = anguloNoche;
					printf("ahora es de noche\n");
				}
			}
			//en los planetas el sol sale por X positivo, sube y se mete por X negativo; con la P tambien se pausa
			anguloSol += anguloSolOffset * deltaTime;
			if (anguloSol > 360.0f)
				anguloSol -= 360.0f;
			float alturaSol = sin(anguloSol * toRadians);
			haciaSol = glm::normalize(glm::vec3(cos(anguloSol * toRadians), alturaSol, -0.3f));
			esDeNoche = alturaSol < 0.0f;
			if (!esDeNoche)
			{
				//de dia la luz baja del sol; entre mas alto esta, mas fuerte alumbra
				//cerca del horizonte se ve naranja porque el verde y el azul dependen de la altura
				mainLight = DirectionalLight(1.0f, 0.6f + 0.4f * alturaSol, 0.4f + 0.5f * alturaSol,
					0.25f + 0.15f * alturaSol, 0.2f + 0.6f * alturaSol,
					-haciaSol.x, -haciaSol.y, -haciaSol.z);
			}
			else
			{
				//de noche alumbra la luna, que esta del lado contrario al sol, con poca luz y azulada
				mainLight = DirectionalLight(0.65f, 0.75f, 1.0f,
					0.15f, 0.2f,
					haciaSol.x, haciaSol.y, haciaSol.z);
			}
		}
		//el sol se dibuja lejos en la direccion de donde viene la luz y se mueve con la camara para que parezca que esta en el infinito
		posSol = posVista + haciaSol * distanciaSol;

		//se llenan los arreglos desde 0 con lo que esta prendido en la zona actual
		pointLightCount = 0;
		spotLightCount = 0;
		if (mainWindow.getlinterna())
		{
			spotLights[spotLightCount] = linternaLuz;
			spotLightCount++;
		}
		if (zonaLuces == 0)
		{
			for (int i = 0; i < NUM_LUMINARIAS_ESPACIO; i++)
			{
				pointLights[pointLightCount] = luminariasEspacio[i];
				pointLightCount++;
			}
			if (mainWindow.getlucesEscenario())
			{
				for (int i = 0; i < NUM_SPOTS_ESPACIO; i++)
				{
					spotLights[spotLightCount] = spotsEspacio[i];
					spotLightCount++;
				}
			}
		}
		else if (zonaLuces == 1)
		{
			if (esDeNoche)
			{
				for (int i = 0; i < NUM_LUMINARIAS_XEN; i++)
				{
					pointLights[pointLightCount] = luminariasXen[i];
					pointLightCount++;
				}
			}
			if (mainWindow.getlucesEscenario())
			{
				spotLights[spotLightCount] = spotXen;
				spotLightCount++;
			}
		}
		else if (zonaLuces == 2)
		{
			if (esDeNoche)
			{
				pointLights[pointLightCount] = luminariaPlaneta2;
				pointLightCount++;
			}
			if (mainWindow.getlucesEscenario())
			{
				spotLights[spotLightCount] = spotPlaneta2;
				spotLightCount++;
			}
		}
		else if (zonaLuces == 3)
		{
			if (esDeNoche)
			{
				pointLights[pointLightCount] = luminariaVerdania;
				pointLightCount++;
			}
			if (mainWindow.getlucesEscenario())
			{
				spotLights[spotLightCount] = spotVerdania;
				spotLightCount++;
			}
		}

		//mandar todas las luces al shader
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		//para dibujar cada objeto siempre se hace lo mismo, se pone la matriz model, el color, la textura y el material, y luego se hace el render
		glm::mat4 model(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f); //con blanco la textura se ve con sus colores normales

		// ---------- objetos opacos ----------
		//todo empieza con color blanco y material opaco, y lo que usa otro material regresa el opaco al terminar
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		//--- personajes ---

		//--- planetas: Xen y planeta 2 ---
		//Xen
		//el planeta Xen en el espacio es una esfera de radio 1, asi que con escala 220 queda de radio 220; gira despacio sobre Y
		giroPlanetaXen += giroPlanetaXenOffset * deltaTime;
		if (giroPlanetaXen > 360.0f)
			giroPlanetaXen -= 360.0f;
		model = glm::mat4(1.0);
		model = glm::translate(model, posPlanetaXen);
		model = glm::rotate(model, giroPlanetaXen * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(220.0f, 220.0f, 220.0f));
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
		//cuando se acaba el tiempo del estado actual, las plantas cambian y el contador vuelve a 0
		tiempoPlantas += deltaTime;
		if (plantasEscondidas && tiempoPlantas > tiempoEscondidas)
		{
			plantasEscondidas = false;
			tiempoPlantas = 0.0f;
		}
		else if (!plantasEscondidas && tiempoPlantas > tiempoParadas)
		{
			plantasEscondidas = true;
			tiempoPlantas = 0.0f;
		}
		//al esconderse bajan rapido y al salir suben mas despacio
		for (int i = 0; i < NUM_PLANTAS; i++)
		{
			glm::vec3 pos = posXen + posPlantas[i];
			if (plantasEscondidas)
			{
				if (alturaPlantas[i] > 0.1f)
					alturaPlantas[i] -= 0.05f * deltaTime;
			}
			else if (alturaPlantas[i] < 1.0f)
			{
				alturaPlantas[i] += 0.01f * deltaTime;
			}
			//se deja entre 0.1 y 1 aunque un frame tarde mucho; con una escala negativa el tallo se voltearia
			//y la parte que esta bajo el piso saldria hacia arriba como un poste muy largo
			if (alturaPlantas[i] < 0.1f)
				alturaPlantas[i] = 0.1f;
			if (alturaPlantas[i] > 1.0f)
				alturaPlantas[i] = 1.0f;
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

		//el headcrab camina en circulo en la superficie de Xen mientras su caminata este prendida con la tecla F
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

		//planeta 2
		//Pop Star mide como 1.8 de la punta al centro, asi que con escala 180 cada punta sale unas 320 unidades
		//la estrella esta parada viendo hacia Z positivo; se gira 120 grados en Y para que vea hacia el centro de la escena
		//y despues gira sobre su propio Z como rehilete
		giroPlaneta2 += giroPlaneta2Offset * deltaTime;
		if (giroPlaneta2 > 360.0f)
			giroPlaneta2 -= 360.0f;
		model = glm::mat4(1.0);
		model = glm::translate(model, posPlaneta2Espacio);
		model = glm::rotate(model, 120.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, giroPlaneta2 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(180.0f, 180.0f, 180.0f));
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

		//--- planeta 3: Verdania ---
		//cada parte que se mueve usa el seno de anguloVerdania por un numero entero, que va y viene suave entre -1 y 1 y no
		//brinca cuando el angulo regresa de 360 a 0; ese valor se multiplica por los grados maximos que se mueve la pieza
		//entre mas grande es el numero mas rapido se mueve, y si se suman grados dentro del seno, como 120 o 240, las plantas
		//iguales quedan desfasadas; si la F apaga las animaciones, las plantas se quedan quietas en su ultima pose
		if (mainWindow.getanimaVerdania())
		{
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

		//piso de pasto, del mismo tamano que el de Xen, que mide 200 por 200
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
		//pasoPea va de -1 a 1; las hojas de abajo le sirven de patas y se mueven cruzadas, la delantera izquierda junto con
		//la trasera derecha y la delantera derecha junto con la trasera izquierda, como caminan los animales de cuatro patas
		float pasoPea = sin(anguloVerdania * 3.0f * toRadians);
		rotPeashooter[8].x = 25.0f * pasoPea;   //hoja_del_izq
		rotPeashooter[11].x = 25.0f * pasoPea;  //hoja_tras_der
		rotPeashooter[9].x = -25.0f * pasoPea;  //hoja_del_der
		rotPeashooter[10].x = -25.0f * pasoPea; //hoja_tras_izq
		//los brazos vienen extendidos en T en el modelo; se bajan 70 grados en Z en el hombro y se quedan quietos
		rotPeashooter[6].z = -70.0f;            //brazo_izq abajo
		rotPeashooter[7].z = 70.0f;             //brazo_der abajo
		//la posicion en el circulo sale del seno y el coseno de su angulo, y se gira 90 grados mas para que mire hacia donde avanza
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + centroPeashooter +
			glm::vec3(radioPeashooter * sin(anguloPeashooter * toRadians), 0.0f, radioPeashooter * cos(anguloPeashooter * toRadians)));
		model = glm::rotate(model, (anguloPeashooter + 90.0f) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(Peashooter_M, padresPeashooter, pivotesPeashooter, rotPeashooter, NUM_PIEZAS_PEASHOOTER, model, uniformModel);

		//crazy dave, que es NPC y camina de un lado a otro en linea recta
		//solo mueve las piernas, una al reves de la otra, como cuando camina una persona;
		//la pantorrilla y el pie son hijos del muslo, asi que se mueven con el sin tocarlos
		float pasoDave = sin(anguloVerdania * 4.0f * toRadians);
		rotCrazyDave[11].x = 25.0f * pasoDave;  //muslo_der
		rotCrazyDave[14].x = -25.0f * pasoDave; //muslo_izq
		//los brazos vienen abiertos en el modelo; se bajan 45 grados en Z y se quedan quietos
		rotCrazyDave[5].z = 45.0f;              //brazo_der abajo
		rotCrazyDave[8].z = -45.0f;             //brazo_izq abajo
		//el modelo mira hacia Z positivo y rumboDave lo gira en Y hacia donde camina
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(posDave, 0.0f, -8.0f));
		model = glm::rotate(model, rumboDave * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(CrazyDave_M, padresCrazyDave, pivotesCrazyDave, rotCrazyDave, NUM_PIEZAS_CRAZYDAVE, model, uniformModel);

		//el ice pea y el fire pea son NPCs en maceta que vigilan y solo voltean la cabeza de un lado a otro hasta 35 grados en Y;
		//el fire pea va desfasado 240 grados para que no volteen al mismo tiempo
		rotIcePea[2].y = 35.0f * sin(anguloVerdania * toRadians);       //cabeza
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(-6.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 40.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(IcePea_M, padresIcePea, pivotesIcePea, rotIcePea, NUM_PIEZAS_ICEPEA, model, uniformModel);

		//el gatling pea es NPC en maceta y su cabeza se queda quieta; solo giran los canones sobre su propio eje,
		//que es el eje Z del modelo
		rotGatlingPea[4].z = giroCanones;
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(-7.0f, 0.0f, -3.0f));
		model = glm::rotate(model, 55.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(GatlingPea_M, padresGatlingPea, pivotesGatlingPea, rotGatlingPea, NUM_PIEZAS_GATLINGPEA, model, uniformModel);

		rotFirePea[2].y = 35.0f * sin((anguloVerdania + 240.0f) * toRadians);       //cabeza
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(-6.5f, 0.0f, -6.5f));
		model = glm::rotate(model, 70.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(FirePea_M, padresFirePea, pivotesFirePea, rotFirePea, NUM_PIEZAS_FIREPEA, model, uniformModel);

		//el cactus es NPC y solo sube y baja los brazos en Z; con el mismo valor en los dos uno sube mientras el otro baja
		//porque estan de lados contrarios
		rotCactus[4].z = 10.0f * sin(anguloVerdania * 2.0f * toRadians); //brazo_izq
		rotCactus[5].z = 10.0f * sin(anguloVerdania * 2.0f * toRadians); //brazo_der
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(6.5f, 0.0f, -1.0f));
		model = glm::rotate(model, -40.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(Cactus_M, padresCactus, pivotesCactus, rotCactus, NUM_PIEZAS_CACTUS, model, uniformModel);

		//los girasoles son de adorno y solo mecen la cabeza de lado en Z, como con el viento;
		//cada uno va desfasado 120 grados del anterior
		rotSunflower[6].z = -60.0f; //los brazos tambien vienen en T y se bajan 60 grados
		rotSunflower[7].z = 60.0f;
		rotSunflower[3].z = 8.0f * sin(anguloVerdania * toRadians); //cabeza
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(-3.0f, 0.0f, -12.0f));
		model = glm::rotate(model, 15.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(Sunflower_M, padresSunflower, pivotesSunflower, rotSunflower, NUM_PIEZAS_SUNFLOWER, model, uniformModel);

		rotSunflower2[6].z = -60.0f;
		rotSunflower2[7].z = 60.0f;
		rotSunflower2[3].z = 8.0f * sin((anguloVerdania + 120.0f) * toRadians); //cabeza
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(0.0f, 0.0f, -13.0f));
		DibujaJerarquia(Sunflower2_M, padresSunflower2, pivotesSunflower2, rotSunflower2, NUM_PIEZAS_SUNFLOWER2, model, uniformModel);

		rotStuffyFlower[6].z = -60.0f;
		rotStuffyFlower[7].z = 60.0f;
		rotStuffyFlower[3].z = 8.0f * sin((anguloVerdania + 240.0f) * toRadians); //cabeza
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(3.0f, 0.0f, -12.0f));
		model = glm::rotate(model, -15.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(StuffyFlower_M, padresStuffyFlower, pivotesStuffyFlower, rotStuffyFlower, NUM_PIEZAS_STUFFYFLOWER, model, uniformModel);

		//hypno-shroom de adorno, solo el sombrero se tambalea de lado
		rotHypnoShroom[2].z = 6.0f * sin(anguloVerdania * 2.0f * toRadians); //sombrero
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(7.0f, 0.0f, -6.0f));
		model = glm::rotate(model, -50.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		DibujaJerarquia(HypnoShroom_M, padresHypnoShroom, pivotesHypnoShroom, rotHypnoShroom, NUM_PIEZAS_HYPNOSHROOM, model, uniformModel);

		//laser bean de adorno, el cuerpo se balancea dentro de su maceta y el pelo se mueve con el porque es su hijo
		rotLaserBean[1].z = 8.0f * sin(anguloVerdania * toRadians);         //cuerpo
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

		//torchwood de adorno, solo la mandibula se abre entre 0 y 12 grados en X
		//como a 6 se le suma 6 por el seno, nunca baja de 0 y la boca no se cierra de mas
		rotTorchwood[4].x = 6.0f + 6.0f * sin(anguloVerdania * 2.0f * toRadians); //mandibula
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(0.0f, 0.0f, -18.0f));
		DibujaJerarquia(Torchwood_M, padresTorchwood, pivotesTorchwood, rotTorchwood, NUM_PIEZAS_TORCHWOOD, model, uniformModel);

		//jardin de crazy dave
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		//casa de crazy dave al fondo; el modelo tiene la puerta hacia Z negativo, por eso se gira 180 grados para que vea al jardin
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(0.0f, 0.0f, -34.0f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaCasa, escalaCasa, escalaCasa));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JardinCasa_M.RenderModel();

		//en la calle los tramos rectos se giran 90 grados para que corran sobre X, y en la punta va el final de la calle
		for (int i = 0; i < NUM_TRAMOS_CALLE; i++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, posVerdania + glm::vec3(posCalleX[i], 0.01f, posCalleZ));
			model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(escalaCasa, escalaCasa, escalaCasa));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			JardinCalle_M.RenderModel();
		}
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(posCalleX[0] - 6.5f, 0.01f, posCalleZ));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaCasa, escalaCasa, escalaCasa));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JardinCalleFin_M.RenderModel();

		//farol junto a donde termina la calle
		model = glm::mat4(1.0);
		model = glm::translate(model, posVerdania + glm::vec3(posCalleX[0] - 5.0f, 0.0f, posCalleZ - 4.0f));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escalaCasa, escalaCasa, escalaCasa));
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
				model = glm::scale(model, glm::vec3(escalaCasa * estiraPuerta, escalaCasa, escalaCasa));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				JardinCercaPuerta_M.RenderModel();
			}
			else
			{
				model = glm::scale(model, glm::vec3(escalaCasa, escalaCasa, escalaCasa));
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

		//planeta Verdania en el espacio, gira despacio sobre Y como Xen; mide como 0.73 de alto y con escala 450 queda de unas 330 unidades
		giroPlanetaVerdania += giroPlanetaVerdaniaOffset * deltaTime;
		if (giroPlanetaVerdania > 360.0f)
			giroPlanetaVerdania -= 360.0f;
		model = glm::mat4(1.0);
		model = glm::translate(model, posPlanetaVerdania);
		model = glm::rotate(model, giroPlanetaVerdania * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(450.0f, 450.0f, 450.0f));
		model = glm::translate(model, -centroCabezaPea);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PlanetaVerdania_M.RenderModel();

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
		//cada uno se dibuja dos veces, el cuerpo con su color y material opaco, y el visor con blanco y material brillante porque es de vidrio
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
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			CrewmateCuerpo_M.RenderModel();
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
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
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			CrewmateCuerpo_M.RenderModel();
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			CrewmateVisor_M.RenderModel();
		}
		//se regresa el material opaco, que es el que traen las secciones de abajo
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

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
		Pista_M.RenderModel();

		//meta
		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(0.0f, 0.05f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Meta_M.RenderModel();

		//cubierta de la meta
		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CubiertaMeta_M.RenderModel();

		//la carrera avanza cada frame y al terminar una vuelta empieza otra
		avanceCarrera += velocidadCarrera * deltaTime;
		if (avanceCarrera > longitudTotalPista)
			avanceCarrera -= longitudTotalPista;

		//naves de F-Zero corriendo en la pista
		//son de metal, por eso usan Material_metal y al terminar se regresa el opaco para los edificios
		Material_metal.UseMaterial(uniformSpecularIntensity, uniformShininess);
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
			NavesFZero_M[i].RenderModel();
		}
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		//puesto de observacion 1, son los edificios del centro
		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(175.0f, 80.00f, 500.0f));
		model = glm::rotate(model, -45.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PuestoObservacion1_M.RenderModel();

		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(175.0f, 80.00f, -375.0f));
		model = glm::rotate(model, -115.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PuestoObservacion1_M.RenderModel();

		//puesto de observacion 2, son los edificios altos
		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(0.0f, 20.00f, 700.0f));
		model = glm::rotate(model, -25.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PuestoObservacion2_M.RenderModel();

		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(300.0f, 200.00f, 700.0f));
		model = glm::rotate(model, 45.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PuestoObservacion2_M.RenderModel();

		//puesto de observacion 3, son los edificios del centro que estan en la orilla
		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(150.0f, 180.00f, 800.0f));
		model = glm::scale(model, glm::vec3(0.75f, 0.75f, 0.75f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PuestoObservacion3_M.RenderModel();

		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(150.0f, 140.00f, -680.0f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.75f, 0.75f, 0.75f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PuestoObservacion3_M.RenderModel();

		//puesto de observacion 4
		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(0.0f, 20.00f, -600.0f));
		model = glm::rotate(model, 35.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PuestoObservacion4_M.RenderModel();

		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(300.0f, 100.00f, -600.0f));
		model = glm::rotate(model, -35.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PuestoObservacion4_M.RenderModel();

		//barco espacial, que es la nave nodriza; con su escala mide como 540 de largo y 200 de alto, y queda adelante
		//a la derecha de donde empieza la camara
		model = glm::mat4(1.0f);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(200.0f, 30.0f, -40.0f));
		model = glm::rotate(model, 265.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.315f, 0.315f, 0.315f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		NaveNodriza_M.RenderModel();

		//almacen del puerto
		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(700.f, -350.00f, 50.0f));
		model = glm::rotate(model, 60.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Almacen_M.RenderModel();

		//nave 1 de la pista
		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(70.0f, 140.0f, 30.0f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 30.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		NavePista_M.RenderModel();

		//nave 2 de la pista
		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(-70.0f, 140.0f, 30.0f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -30.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		NavePista_M.RenderModel();

		//--- satelites y cuerpos espaciales ---
		//satelite
		model = modelauxPista;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(-125.0f, 50.0f, -750.0f));
		model = glm::rotate(model, 240.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Satelite_M.RenderModel();

		//--- plataforma de lanzamiento y cohete ---
		//despegue del cohete; cada parte avanza y al terminar pasa a la siguiente, como las naves de la estacion
		//las esperas van en segundos, por eso deltaTime se divide entre 61, que es lo que suma en un segundo
		if (estadoCohete == 0)
		{
			//parado en la plataforma; si la F tiene prendidos los despegues, empieza la cuenta regresiva
			if (mainWindow.getdespegaCohete())
			{
				cuentaCohete = tiempoCuentaCohete;
				estadoCohete = 1;
			}
		}
		else if (estadoCohete == 1)
		{
			//cuenta regresiva: la llama crece hasta la mitad y el cohete tiembla cada vez mas desde la base,
			//que es la raiz de su jerarquia, asi tiembla todo junto; el seno da unas 5 sacudidas por segundo
			cuentaCohete -= deltaTime / 61.0f;
			fuegoCohete = 0.5f * (1.0f - cuentaCohete / tiempoCuentaCohete);
			rotCohete[0].z = 3.0f * fuegoCohete * sin(cuentaCohete * 2000.0f * toRadians);
			if (cuentaCohete <= 0.0f)
			{
				rotCohete[0].z = 0.0f;
				velocidadCohete = 0.0f;
				estadoCohete = 2;
			}
		}
		else if (estadoCohete == 2)
		{
			//sube acelerando con la llama completa: la velocidad crece cada frame y la altura crece con la velocidad
			fuegoCohete = 1.0f;
			velocidadCohete += aceleracionCohete * deltaTime;
			alturaCohete += velocidadCohete * deltaTime;
			if (alturaCohete >= alturaMaxCohete)
			{
				alturaCohete = alturaMaxCohete;
				estadoCohete = 3;
			}
		}
		else if (estadoCohete == 3)
		{
			//regresa a su lugar bajando despacio con la llama mas chica; al tocar la plataforma se apaga
			fuegoCohete = 0.6f;
			alturaCohete -= velBajadaCohete * deltaTime;
			if (alturaCohete <= 0.0f)
			{
				alturaCohete = 0.0f;
				fuegoCohete = 0.0f;
				estadoCohete = 0;
			}
		}

		//modelauxPlataforma guarda el lugar y el tamano de la plataforma, y desde ahi se ponen el piso, la torre y el cohete;
		//asi todo se mide en las unidades de los modelos de Kenney y despues se escala junto
		model = glm::mat4(1.0f);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, posPlaneta2 + posPlataforma);
		model = glm::scale(model, glm::vec3(escalaPlataforma, escalaPlataforma, escalaPlataforma));
		modelauxPlataforma = model;
		//piso: 3 por 3 placas de 2 por 2 con el cohete en medio; van sobre el piso del planeta (y = 0) y miden 0.1 de grueso,
		//por eso la torre y el cohete se suben 0.1, para que queden parados encima de las placas y no se hundan
		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				model = modelauxPlataforma;
				model = glm::translate(model, glm::vec3(i * 2.0f, 0.0f, j * 2.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				PlataformaCohete_M.RenderModel();
			}
		}
		//la torre y el cohete son de metal, como las naves, y al terminar se regresa el material opaco
		Material_metal.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//torre: cubos de vigas de 1 por lado, uno encima de otro, a un lado del cohete; con 5 queda un poco mas alta que el
		for (int i = 0; i < pisosTorre; i++)
		{
			model = modelauxPlataforma;
			model = glm::translate(model, glm::vec3(1.6f, 0.1f + i * 1.0f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			TorreCohete_M.RenderModel();
		}
		//cohete por piezas en medio de la plataforma; se sube 0.1 para quedar sobre las placas, mas lo que lleva de despegue
		//raizCohete se guarda para poner la llama abajo del motor
		model = modelauxPlataforma;
		model = glm::translate(model, glm::vec3(0.0f, 0.1f + alturaCohete, 0.0f));
		glm::mat4 raizCohete = model;
		DibujaJerarquia(Cohete_M, padresCohete, pivotesCohete, rotCohete, NUM_PIEZAS_COHETE, model, uniformModel);
		//llama del motor: es la pieza del fuego del torchwood de Verdania, que ya esta cargada, puesta de cabeza
		//abajo del motor, que termina en y = 0.2; mientras mas grande fuegoCohete, mas larga la llama
		if (fuegoCohete > 0.0f)
		{
			model = raizCohete;
			model = glm::translate(model, glm::vec3(0.0f, 0.2f, 0.0f));
			model = glm::rotate(model, 180.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.6f, 1.2f * fuegoCohete, 0.6f));
			//el fuego del torchwood esta arriba de su cabeza, entre y = 1.9 y 2.72; se baja para que su base quede en el origen
			model = glm::translate(model, glm::vec3(0.0f, -1.9f, 0.23f));
			//un color arriba de 1 hace que brille aunque no le llegue luz, como el sol
			color = glm::vec3(2.0f, 2.0f, 2.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Torchwood_M[3].RenderModel();
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		}
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		//astronautas: Gordon y el Capitan Falcon parados junto a la plataforma, viendo hacia donde llega la camara
		//Gordon es por piezas y tiene los pies en y = 0, asi que solo se pone en su lugar y se escala
		model = glm::mat4(1.0f);
		model = glm::translate(model, posPlaneta2 + posGordonAstronauta);
		model = glm::scale(model, glm::vec3(escalaGordonAstronauta, escalaGordonAstronauta, escalaGordonAstronauta));
		DibujaJerarquia(Gordon_M, padresGordon, pivotesGordon, rotGordon, NUM_PIEZAS_GORDON, model, uniformModel);
		//el Capitan Falcon es una sola pieza y su modelo tiene los pies en y = -0.627, por eso se sube eso despues de escalar
		model = glm::mat4(1.0f);
		model = glm::translate(model, posPlaneta2 + posFalconAstronauta);
		model = glm::scale(model, glm::vec3(escalaFalconAstronauta, escalaFalconAstronauta, escalaFalconAstronauta));
		model = glm::translate(model, glm::vec3(0.0f, 0.627f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CapitanFalcon_M.RenderModel();


		//--- estacion espacial y naves ---
		//modelauxEstacion se guarda antes de escalar la estacion, asi los pads y las naves no se hacen chicos con ella
		model = glm::mat4(1.0f);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, posEstacion);
		modelauxEstacion = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Estacion_M.RenderModel();

		//landing pads; el modelo tiene hacia X negativo el lado donde se posan las naves, por eso cada pad se gira
		//para que ese lado quede hacia afuera de la estacion
		model = modelauxEstacion;
		model = glm::translate(model, glm::vec3(-150.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LandingPad_M.RenderModel();

		model = modelauxEstacion;
		model = glm::translate(model, glm::vec3(150.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LandingPad_M.RenderModel();

		model = modelauxEstacion;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 150.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LandingPad_M.RenderModel();

		model = modelauxEstacion;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -150.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LandingPad_M.RenderModel();

		//naves de la estacion; cada una avanza en su estado y al terminarlo pasa al siguiente
		//las esperas van en segundos, por eso deltaTime se divide entre 61, que es lo que suma en un segundo
		//son de metal, como las de la carrera, y al terminar se regresa el material opaco
		float segundosFrame = deltaTime / 61.0f;
		Material_metal.UseMaterial(uniformSpecularIntensity, uniformShininess);
		for (int i = 0; i < NUM_NAVES_ESTACION; i++)
		{
			if (estadoNavesEstacion[i] == 0)
			{
				//estacionada, esperando a que se acabe su tiempo
				esperaNavesEstacion[i] -= segundosFrame;
				if (esperaNavesEstacion[i] <= 0.0f)
					estadoNavesEstacion[i] = 1;
			}
			else if (estadoNavesEstacion[i] == 1)
			{
				//se eleva sobre su pad
				subidaNavesEstacion[i] += velSubidaNave * deltaTime;
				if (subidaNavesEstacion[i] >= alturaNaveEstacion)
				{
					subidaNavesEstacion[i] = alturaNaveEstacion;
					estadoNavesEstacion[i] = 2;
				}
			}
			else if (estadoNavesEstacion[i] == 2)
			{
				//gira 180 grados en Y para quedar viendo hacia afuera
				vueltaNavesEstacion[i] += velGiroNave * deltaTime;
				if (vueltaNavesEstacion[i] >= 180.0f)
				{
					vueltaNavesEstacion[i] = 180.0f;
					estadoNavesEstacion[i] = 3;
				}
			}
			else if (estadoNavesEstacion[i] == 3)
			{
				//sale a hipervelocidad; al llegar a distanciaSalida deja de dibujarse y toma cuanto tarda en regresar
				avanceNavesEstacion[i] += velHipervelocidad * deltaTime;
				if (avanceNavesEstacion[i] >= distanciaSalida)
				{
					avanceNavesEstacion[i] = distanciaSalida;
					visibleNavesEstacion[i] = false;
					esperaNavesEstacion[i] = tiempoMinFuera + SiguienteEspera(i);
					estadoNavesEstacion[i] = 4;
				}
			}
			else if (estadoNavesEstacion[i] == 4)
			{
				//fuera de la escena; cuando se acaba su tiempo vuelve a aparecer lejos, ya viendo hacia la estacion
				esperaNavesEstacion[i] -= segundosFrame;
				if (esperaNavesEstacion[i] <= 0.0f)
				{
					vueltaNavesEstacion[i] = 0.0f;
					subidaNavesEstacion[i] = alturaNaveEstacion;
					avanceNavesEstacion[i] = distanciaSalida;
					visibleNavesEstacion[i] = true;
					estadoNavesEstacion[i] = 5;
				}
			}
			else if (estadoNavesEstacion[i] == 5)
			{
				//regresa a hipervelocidad hasta que le faltan distanciaFrenado para llegar
				avanceNavesEstacion[i] -= velHipervelocidad * deltaTime;
				if (avanceNavesEstacion[i] <= distanciaFrenado)
				{
					avanceNavesEstacion[i] = distanciaFrenado;
					estadoNavesEstacion[i] = 6;
				}
			}
			else if (estadoNavesEstacion[i] == 6)
			{
				//se acerca despacio hasta quedar arriba de su pad
				avanceNavesEstacion[i] -= velAcercamiento * deltaTime;
				if (avanceNavesEstacion[i] <= 0.0f)
				{
					avanceNavesEstacion[i] = 0.0f;
					estadoNavesEstacion[i] = 7;
				}
			}
			else
			{
				//baja hasta posarse y toma cuanto tiempo se queda estacionada
				subidaNavesEstacion[i] -= velSubidaNave * deltaTime;
				if (subidaNavesEstacion[i] <= 0.0f)
				{
					subidaNavesEstacion[i] = 0.0f;
					esperaNavesEstacion[i] = tiempoMinEstacionada + SiguienteEspera(i);
					estadoNavesEstacion[i] = 0;
				}
			}

			//su lugar es el de su pad mas lo que ha subido y lo que ha avanzado hacia afuera
			if (visibleNavesEstacion[i])
			{
				glm::vec3 posNave = posNavesEstacion[i] + glm::vec3(0.0f, subidaNavesEstacion[i], 0.0f) + salidaNavesEstacion[i] * avanceNavesEstacion[i];
				model = modelauxEstacion;
				model = glm::translate(model, posNave);
				model = glm::rotate(model, (giroNavesEstacion[i] + vueltaNavesEstacion[i]) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				if (modeloNavesEstacion[i] == 0)
					NaveDorada_M.RenderModel();
				else if (modeloNavesEstacion[i] == 1)
					NaveBlanca_M.RenderModel();
				else
					NaveCaza_M.RenderModel();
			}
		}
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		//--- avatar ---
		//mientras el avatar camina, pasoAvatar va y viene entre sus limites como las patas del headcrab
		//usa deltaReal para que siga caminando aunque las demas animaciones esten en pausa
		if (avatarCamina)
		{
			if (pasoAdelanteAvatar)
			{
				pasoAvatar += pasoAvatarOffset * deltaReal;
				if (pasoAvatar > limitePasoAvatar)
					pasoAdelanteAvatar = false;
			}
			else
			{
				pasoAvatar -= pasoAvatarOffset * deltaReal;
				if (pasoAvatar < -limitePasoAvatar)
					pasoAdelanteAvatar = true;
			}
		}
		else
		{
			//cuando se detiene, los pies y las manos regresan poco a poco a su lugar
			if (pasoAvatar > 0.0f)
			{
				pasoAvatar -= pasoAvatarOffset * deltaReal;
				if (pasoAvatar < 0.0f)
					pasoAvatar = 0.0f;
			}
			else if (pasoAvatar < 0.0f)
			{
				pasoAvatar += pasoAvatarOffset * deltaReal;
				if (pasoAvatar > 0.0f)
					pasoAvatar = 0.0f;
			}
		}
		//los pies se balancean en X desde la cadera, cada uno al reves del otro
		rotAvatar[3].x = pasoAvatar;
		rotAvatar[4].x = -pasoAvatar;
		//las manos giran en Y con el mismo angulo; como estan en lados opuestos del cuerpo, una va hacia adelante
		//mientras la otra va hacia atras, y cada una va al reves del pie de su lado
		rotAvatar[1].y = pasoAvatar * 1.2f;
		rotAvatar[2].y = pasoAvatar * 1.2f;
		//en la carnivora, las hojas de abajo le sirven de patas y usan el mismo pasoAvatar; se mueven cruzadas como las del
		//peashooter, la delantera izquierda junto con la trasera derecha y la delantera derecha junto con la trasera izquierda
		rotAvatarCarnivora[8].x = pasoAvatar;   //hoja_del_izq
		rotAvatarCarnivora[9].x = pasoAvatar;   //hoja_tras_der
		rotAvatarCarnivora[7].x = -pasoAvatar;  //hoja_del_der
		rotAvatarCarnivora[10].x = -pasoAvatar; //hoja_tras_izq
		//el avatar da un pequeno salto en cada paso, mas alto mientras mas separados van los pies
		float saltoAvatar = 0.03f * fabs(pasoAvatar) / limitePasoAvatar;

		//el avatar solo esta en los planetas y se queda donde se dejo aunque se cambie a la camara libre
		if (zonaActual != 0)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, centroAvatar + posAvatar + glm::vec3(0.0f, saltoAvatar, 0.0f));
			model = glm::rotate(model, rumboAvatar * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			Material_avatar.UseMaterial(uniformSpecularIntensity, uniformShininess);
			//los dos personajes se colocan y giran igual; la T cambia cual modelo se dibuja y cada uno lleva su escala
			if (mainWindow.getpersonajeAvatar() == 0)
			{
				model = glm::scale(model, glm::vec3(escalaAvatar, escalaAvatar, escalaAvatar));
				DibujaJerarquia(Kirby_M, padresKirby, pivotesAvatar, rotAvatar, NUM_PIEZAS_KIRBY, model, uniformModel);
			}
			else
			{
				model = glm::scale(model, glm::vec3(escalaAvatarCarnivora, escalaAvatarCarnivora, escalaAvatarCarnivora));
				DibujaJerarquia(Carnivora_M, padresCarnivora, pivotesCarnivora, rotAvatarCarnivora, NUM_PIEZAS_CARNIVORA, model, uniformModel);
			}
		}

		//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---
		//el sol se dibuja en posSol en el espacio y en los planetas de dia; de noche sale la luna del lado contrario,
		//que es de donde viene la luz azulada; el color va arriba de 1 para que brillen aunque la luz les pegue por atras
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		model = glm::mat4(1.0);
		if (mainWindow.getzonaPlaneta() == 0 || !esDeNoche)
		{
			model = glm::translate(model, posSol);
			model = glm::scale(model, glm::vec3(tamanoSol, tamanoSol, tamanoSol));
			color = glm::vec3(3.0f, 3.0f, 3.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Sol_M.RenderModel();
		}
		else
		{
			model = glm::translate(model, posVista - haciaSol * distanciaSol);
			model = glm::scale(model, glm::vec3(tamanoLuna, tamanoLuna, tamanoLuna));
			color = glm::vec3(5.0f, 5.0f, 5.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Luna_M.RenderModel();
		}
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		// ---------- objetos con transparencia ----------
		//van al final, despues de todos los opacos, si no se ven mal
		//se dibujan entre glEnable(GL_BLEND) y glDisable(GL_BLEND)

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
