#pragma once
//datos fijos de la escena: piezas de los modelos con jerarquia, puntos de la pista y lugares de los objetos que se repiten
//estan aparte para que main.cpp quede mas corto; lo que cambia mientras corre el programa se queda en main.cpp
//solo se incluye en main.cpp, porque los arreglos se crean aqui y en otro .cpp darian simbolos repetidos
//tiene las mismas secciones que main.cpp

#include <glm.hpp>

//los modelos con jerarquia estan separados en piezas .obj que comparten el mismo origen, todas con Y hacia arriba,
//los pies en y = 0 y mirando hacia Z positivo; "der" e "izq" son la derecha y la izquierda del modelo, y su derecha
//queda hacia X negativo
//por cada pieza se guarda el nombre de su .obj, el padre, que es el indice de la pieza de la que cuelga y vale -1 si es
//la raiz, y el pivote, que es el punto de la articulacion donde gira; el padre siempre va antes que sus hijos

//--- personajes ---
//la planta carnivora mide como 1.4 de alto
const int NUM_PIEZAS_CARNIVORA = 15;
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

//Gordon mide como 1.4 de alto
const int NUM_PIEZAS_GORDON = 15;
const char* piezasGordon[NUM_PIEZAS_GORDON] = {
	"torso", "cabeza",
	"brazo_der", "antebrazo_der", "mano_der",
	"brazo_izq", "antebrazo_izq", "mano_izq",
	"muslo_der", "pantorrilla_der", "pie_der",
	"muslo_izq", "pantorrilla_izq", "pie_izq",
	"logo" };
int padresGordon[NUM_PIEZAS_GORDON] = {
	-1, 0,
	0, 2, 3,
	0, 5, 6,
	0, 8, 9,
	0, 11, 12,
	7 };
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
	glm::vec3(0.07732f, 0.08130f, -0.03441f),  //pie_izq
	glm::vec3(0.43085f, 0.77516f, -0.04469f)   //logo de WhatsApp en la mano izquierda, no gira por su cuenta
};

//Kirby mide como 0.73 de alto
const int NUM_PIEZAS_KIRBY = 5;
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

//el Capitan Falcon mide como 0.95 de alto y tiene los pies en y = -0.627; viene en pose de T, asi que sus brazos van sobre X
const int NUM_PIEZAS_FALCON = 6;
const char* piezasFalcon[NUM_PIEZAS_FALCON] = {
	"cuerpo", "cabeza", "brazo_der", "antebrazo_der", "brazo_izq", "antebrazo_izq" };
int padresFalcon[NUM_PIEZAS_FALCON] = { -1, 0, 0, 2, 0, 4 };
glm::vec3 pivotesFalcon[NUM_PIEZAS_FALCON] = {
	glm::vec3(0.0f, 0.0f, 0.0f),         //cuerpo
	glm::vec3(0.0f, 0.19f, 0.0f),        //cabeza, gira en el cuello
	glm::vec3(-0.12f, 0.16f, -0.03f),    //brazo_der, gira en el hombro
	glm::vec3(-0.25f, 0.16f, -0.03f),    //antebrazo_der, gira en el codo
	glm::vec3(0.12f, 0.16f, -0.03f),     //brazo_izq
	glm::vec3(0.25f, 0.16f, -0.03f)      //antebrazo_izq
};

//--- planetas: Xen y planeta 2 ---
//el headcrab de Xen mide como 0.3 de alto y 0.6 de largo contando las patas
const int NUM_PIEZAS_HEADCRAB = 10;
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

//criaturas de Xen; todas miran hacia Z positivo y tienen los pies en y = 0
//el houndeye mide 0.75 de alto y tiene dos patas adelante y una atras, que giran donde tocan el cuerpo
const int NUM_PIEZAS_HOUNDEYE = 4;
const char* piezasHoundeye[NUM_PIEZAS_HOUNDEYE] = {
	"cuerpo", "pata_del_der", "pata_del_izq", "pata_tras" };
int padresHoundeye[NUM_PIEZAS_HOUNDEYE] = { -1, 0, 0, 0 };
glm::vec3 pivotesHoundeye[NUM_PIEZAS_HOUNDEYE] = {
	glm::vec3(0.0f, 0.5f, 0.0f),         //cuerpo
	glm::vec3(-0.22f, 0.5f, 0.14f),      //pata_del_der
	glm::vec3(0.22f, 0.5f, 0.14f),       //pata_del_izq
	glm::vec3(0.0f, 0.4f, -0.31f)        //pata_tras
};
//el bullsquid mide 0.9 de alto y 2.6 de largo con la cola; el cuerpo y las patas giran en la misma linea de la cadera,
//asi el cuerpo se puede echar hacia atras mientras las patas se quedan en el piso
const int NUM_PIEZAS_BULLSQUID = 5;
const char* piezasBullsquid[NUM_PIEZAS_BULLSQUID] = {
	"cuerpo", "tentaculos", "cola", "pata_der", "pata_izq" };
int padresBullsquid[NUM_PIEZAS_BULLSQUID] = { -1, 0, 0, 0, 0 };
glm::vec3 pivotesBullsquid[NUM_PIEZAS_BULLSQUID] = {
	glm::vec3(0.0f, 0.7f, 0.15f),        //cuerpo, gira en la cadera
	glm::vec3(-0.11f, 0.5f, 0.65f),      //tentaculos, giran donde empieza la boca
	glm::vec3(0.0f, 0.7f, 0.0f),         //cola
	glm::vec3(-0.4f, 0.7f, 0.15f),       //pata_der
	glm::vec3(0.4f, 0.7f, 0.15f)         //pata_izq
};
//el vortigaunt mide 1.3 de alto, encorvado; las piernas van con el cuerpo porque se queda en su lugar
const int NUM_PIEZAS_VORTIGAUNT = 4;
const char* piezasVortigaunt[NUM_PIEZAS_VORTIGAUNT] = {
	"cuerpo", "cabeza", "brazo_der", "brazo_izq" };
int padresVortigaunt[NUM_PIEZAS_VORTIGAUNT] = { -1, 0, 0, 0 };
glm::vec3 pivotesVortigaunt[NUM_PIEZAS_VORTIGAUNT] = {
	glm::vec3(0.0f, 0.9f, 0.0f),         //cuerpo
	glm::vec3(0.0f, 0.95f, 0.3f),        //cabeza, gira en el cuello
	glm::vec3(-0.3f, 0.86f, -0.05f),     //brazo_der, gira en el hombro
	glm::vec3(0.3f, 0.86f, -0.05f)       //brazo_izq
};
//el alien grunt mide 1.9 de alto; trae el arma en el brazo derecho, la garra en el izquierdo y un brazo chico en el pecho
//el modelo viene a medio paso, con la pierna derecha adelante y la izquierda atras
const int NUM_PIEZAS_GRUNT = 6;
const char* piezasGrunt[NUM_PIEZAS_GRUNT] = {
	"cuerpo", "arma", "brazo_izq", "brazo_chico", "pierna_der", "pierna_izq" };
int padresGrunt[NUM_PIEZAS_GRUNT] = { -1, 0, 0, 0, 0, 0 };
glm::vec3 pivotesGrunt[NUM_PIEZAS_GRUNT] = {
	glm::vec3(0.0f, 0.9f, -0.2f),        //cuerpo
	glm::vec3(-0.58f, 1.25f, -0.3f),     //arma, gira en el hombro
	glm::vec3(0.58f, 1.28f, -0.35f),     //brazo_izq
	glm::vec3(-0.05f, 1.25f, 0.0f),      //brazo_chico
	glm::vec3(-0.25f, 0.85f, -0.1f),     //pierna_der, gira en la cadera
	glm::vec3(0.22f, 0.85f, -0.3f)       //pierna_izq
};
//el boid es plano y mide 6.2 de punta a punta; cada mitad del ala gira en el centro del cuerpo y la punta cuelga de ella,
//por eso las dos mitades no tienen padre
const int NUM_PIEZAS_BOID = 4;
const char* piezasBoid[NUM_PIEZAS_BOID] = {
	"ala_der", "punta_der", "ala_izq", "punta_izq" };
int padresBoid[NUM_PIEZAS_BOID] = { -1, 0, -1, 2 };
glm::vec3 pivotesBoid[NUM_PIEZAS_BOID] = {
	glm::vec3(0.0f, 0.25f, 0.0f),        //ala_der
	glm::vec3(-1.95f, 0.27f, -0.3f),     //punta_der
	glm::vec3(0.0f, 0.25f, 0.0f),        //ala_izq
	glm::vec3(1.95f, 0.27f, -0.3f)       //punta_izq
};
//el arbol de Xen mide 5 de alto y el brazo sale de la punta del tronco hacia el frente
const int NUM_PIEZAS_ARBOL_XEN = 2;
const char* piezasArbolXen[NUM_PIEZAS_ARBOL_XEN] = { "tronco", "brazo" };
int padresArbolXen[NUM_PIEZAS_ARBOL_XEN] = { -1, 0 };
glm::vec3 pivotesArbolXen[NUM_PIEZAS_ARBOL_XEN] = {
	glm::vec3(0.0f, 2.5f, 0.0f),         //tronco
	glm::vec3(-0.2f, 4.75f, 0.35f)       //brazo, gira donde se une con el tronco
};

//arboles de Xen, cada uno girado para que su brazo vea hacia el centro
const int NUM_ARBOLES_XEN = 4;
glm::vec3 posArbolesXen[NUM_ARBOLES_XEN] = {
	glm::vec3(-17.0f, 0.0f, -18.0f),
	glm::vec3(15.0f, 0.0f, -20.0f),
	glm::vec3(-24.0f, 0.0f, -3.0f),
	glm::vec3(24.0f, 0.0f, 0.0f) };
float giroArbolesXen[NUM_ARBOLES_XEN] = { 45.0f, -38.0f, 98.0f, -90.0f };
float escalaArbolesXen[NUM_ARBOLES_XEN] = { 1.4f, 1.6f, 1.3f, 1.5f };

//cristales de Xen; el spotlight morado alumbra el primero
const int NUM_CRISTALES = 12;
glm::vec3 posCristales[NUM_CRISTALES] = {
	glm::vec3(-10.0f, 0.0f, -3.0f),
	glm::vec3(11.0f, 0.0f, 3.0f),
	glm::vec3(-4.0f, 0.0f, 6.0f),
	glm::vec3(12.0f, 0.0f, -12.0f),
	glm::vec3(-15.0f, 0.0f, -13.0f),
	glm::vec3(16.0f, 0.0f, -6.0f),
	glm::vec3(-21.0f, 0.0f, 5.0f),
	glm::vec3(19.0f, 0.0f, -24.0f),
	glm::vec3(-22.0f, 0.0f, -26.0f),
	glm::vec3(3.0f, 0.0f, -23.0f),
	glm::vec3(26.0f, 0.0f, 12.0f),
	glm::vec3(-11.0f, 0.0f, 13.0f) };
float giroCristales[NUM_CRISTALES] = { 0.0f, 70.0f, 150.0f, 0.0f, 30.0f, 200.0f, 100.0f, 15.0f, 250.0f, 80.0f, 160.0f, 300.0f };
float escalaCristales[NUM_CRISTALES] = { 1.2f, 1.0f, 0.7f, 2.0f, 1.6f, 1.1f, 2.4f, 3.0f, 2.2f, 1.8f, 2.0f, 0.9f };

//islas flotantes de Xen; con cosaIsla en 0 la isla no lleva nada encima, con 1 lleva un cristal y con 2 una planta de luz
//el crewmate escondido de Xen esta parado en la primera
const int NUM_ISLAS = 7;
glm::vec3 posIslas[NUM_ISLAS] = {
	glm::vec3(-20.0f, 12.0f, -30.0f),
	glm::vec3(25.0f, 18.0f, -20.0f),
	glm::vec3(5.0f, 25.0f, -45.0f),
	glm::vec3(-32.0f, 20.0f, 6.0f),
	glm::vec3(30.0f, 30.0f, -38.0f),
	glm::vec3(-8.0f, 35.0f, -55.0f),
	glm::vec3(38.0f, 15.0f, 18.0f) };
float escalaIslas[NUM_ISLAS] = { 3.0f, 4.0f, 6.0f, 5.0f, 7.0f, 9.0f, 4.0f };
int cosaIsla[NUM_ISLAS] = { 0, 1, 0, 2, 1, 0, 2 };

//plantas de luz de Xen; solo las primeras NUM_LUMINARIAS_XEN tienen luz para no pasar del limite de la zona
const int NUM_PLANTAS = 6;
glm::vec3 posPlantas[NUM_PLANTAS] = {
	glm::vec3(-7.0f, 0.0f, 2.0f),
	glm::vec3(8.5f, 0.0f, 0.0f),
	glm::vec3(2.0f, 0.0f, -15.0f),
	glm::vec3(-14.0f, 0.0f, -5.0f),
	glm::vec3(15.0f, 0.0f, -17.0f),
	glm::vec3(-4.0f, 0.0f, -21.0f) };

//las colinas del planeta 2 van en las orillas del mapa como paisaje, porque la camara no choca con nada y las atravesaria
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

//habitantes de Pop Star; los Waddle Dee y el Waddle Doo tienen las mismas piezas que Kirby, cuerpo, manos y pies,
//y las manos y los pies giran en la parte que toca el cuerpo
//el Waddle Dee mide 0.9 de alto
const int NUM_PIEZAS_WADDLE = 5;
const char* piezasWaddle[NUM_PIEZAS_WADDLE] = {
	"cuerpo", "mano_der", "mano_izq", "pie_der", "pie_izq" };
int padresWaddle[NUM_PIEZAS_WADDLE] = { -1, 0, 0, 0, 0 };
glm::vec3 pivotesWaddleDee[NUM_PIEZAS_WADDLE] = {
	glm::vec3(0.0f, 0.45f, 0.0f),        //cuerpo
	glm::vec3(-0.28f, 0.45f, 0.0f),      //mano_der
	glm::vec3(0.28f, 0.45f, 0.0f),       //mano_izq
	glm::vec3(-0.18f, 0.18f, 0.0f),      //pie_der
	glm::vec3(0.18f, 0.18f, 0.0f)        //pie_izq
};
//el Waddle Doo mide 1 de alto con las pestanas
glm::vec3 pivotesWaddleDoo[NUM_PIEZAS_WADDLE] = {
	glm::vec3(0.0f, 0.5f, 0.0f),         //cuerpo
	glm::vec3(-0.36f, 0.49f, -0.05f),    //mano_der
	glm::vec3(0.36f, 0.49f, -0.05f),     //mano_izq
	glm::vec3(-0.19f, 0.19f, -0.03f),    //pie_der
	glm::vec3(0.19f, 0.19f, -0.03f)      //pie_izq
};
//el Bandana Dee mide 1 de alto y la lanza cuelga de su mano derecha, que la agarra mas o menos a la mitad
const int NUM_PIEZAS_BANDANA = 6;
const char* piezasBandana[NUM_PIEZAS_BANDANA] = {
	"cuerpo", "mano_der", "lanza", "mano_izq", "pie_der", "pie_izq" };
int padresBandana[NUM_PIEZAS_BANDANA] = { -1, 0, 1, 0, 0, 0 };
glm::vec3 pivotesBandana[NUM_PIEZAS_BANDANA] = {
	glm::vec3(0.0f, 0.5f, 0.0f),         //cuerpo
	glm::vec3(-0.35f, 0.53f, 0.0f),      //mano_der
	glm::vec3(-0.70f, 0.53f, -0.05f),    //lanza, gira donde la agarra la mano
	glm::vec3(0.35f, 0.53f, 0.0f),       //mano_izq
	glm::vec3(-0.21f, 0.2f, 0.0f),       //pie_der
	glm::vec3(0.21f, 0.2f, 0.0f)         //pie_izq
};
//King Dedede mide 2.4 de alto; el martillo cuelga de la mano derecha y la mano del brazo
const int NUM_PIEZAS_DEDEDE = 8;
const char* piezasDedede[NUM_PIEZAS_DEDEDE] = {
	"cuerpo", "brazo_der", "mano_der", "martillo", "brazo_izq", "mano_izq", "pie_der", "pie_izq" };
int padresDedede[NUM_PIEZAS_DEDEDE] = { -1, 0, 1, 2, 0, 4, 0, 0 };
glm::vec3 pivotesDedede[NUM_PIEZAS_DEDEDE] = {
	glm::vec3(0.0f, 1.0f, 0.0f),         //cuerpo
	glm::vec3(-0.45f, 1.1f, 0.0f),       //brazo_der, gira en el hombro
	glm::vec3(-1.2f, 1.07f, -0.08f),     //mano_der, gira en la muneca
	glm::vec3(-1.601f, 0.966f, -0.08f),  //martillo, gira sobre el centro del mango
	glm::vec3(0.45f, 1.1f, 0.0f),        //brazo_izq
	glm::vec3(1.2f, 1.07f, -0.08f),      //mano_izq
	glm::vec3(-0.4f, 0.3f, 0.0f),        //pie_der
	glm::vec3(0.4f, 0.3f, 0.0f)          //pie_izq
};

//--- planeta 3: Verdania ---
//el peashooter mide como 1.5 de alto
const int NUM_PIEZAS_PEASHOOTER = 12;
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

//crazy dave mide como 2.3 de alto contando la olla
const int NUM_PIEZAS_CRAZYDAVE = 17;
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

//ice pea, un pea de hielo en maceta; los tres peas en maceta miden como 1.4 de alto
const int NUM_PIEZAS_ICEPEA = 8;
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

//gatling pea, un pea con casco y canones de metralleta; los canones son una pieza aparte para que puedan girar
const int NUM_PIEZAS_GATLINGPEA = 9;
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

//fire pea, un pea de fuego en maceta
const int NUM_PIEZAS_FIREPEA = 8;
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

//cactus, mide como 1.45 de alto y tiene un tallo de dos partes y un brazo de cada lado
const int NUM_PIEZAS_CACTUS = 6;
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

//girasol, mide como 1.1 de alto
const int NUM_PIEZAS_SUNFLOWER = 12;
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

//girasol mas grande, mide como 1.9 de alto
const int NUM_PIEZAS_SUNFLOWER2 = 12;
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

//girasol de peluche, tiene las mismas piezas que los otros girasoles
const int NUM_PIEZAS_STUFFYFLOWER = 12;
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

//hypno-shroom, un hongo con tallo, sombrero y boca
const int NUM_PIEZAS_HYPNOSHROOM = 4;
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

//laser bean, un frijol en maceta; la maceta es la raiz y el cuerpo cuelga de ella
const int NUM_PIEZAS_LASERBEAN = 3;
const char* piezasLaserBean[NUM_PIEZAS_LASERBEAN] = {
	"maceta", "cuerpo", "pelo" };
int padresLaserBean[NUM_PIEZAS_LASERBEAN] = {
	-1, 0, 1 };
glm::vec3 pivotesLaserBean[NUM_PIEZAS_LASERBEAN] = {
	glm::vec3(0.0000f, 0.3107f, 0.0000f),       //maceta
	glm::vec3(0.0000f, 0.5511f, 0.2324f),       //cuerpo
	glm::vec3(0.0000f, 1.1489f, -0.1837f)       //pelo
};

//potato mine, una papa enterrada; la tierra es la raiz, el cuerpo cuelga de ella y la antena del cuerpo
const int NUM_PIEZAS_POTATOMINE = 3;
const char* piezasPotatoMine[NUM_PIEZAS_POTATOMINE] = {
	"tierra", "cuerpo", "antena" };
int padresPotatoMine[NUM_PIEZAS_POTATOMINE] = {
	-1, 0, 1 };
glm::vec3 pivotesPotatoMine[NUM_PIEZAS_POTATOMINE] = {
	glm::vec3(-0.0042f, 0.1899f, 0.0095f),      //tierra
	glm::vec3(-0.0042f, 0.1899f, 0.0095f),      //cuerpo
	glm::vec3(-0.0042f, 0.4880f, 0.0104f)       //antena
};

//torchwood, un tronco con cabeza, boca y brazos que mide como 2.7 de alto
const int NUM_PIEZAS_TORCHWOOD = 9;
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

//jardin de crazy dave
//la calle va en linea recta sobre X; cada tramo mide 1 por 1, que con escalaCasa queda de 6.5 por 6.5, y el ultimo es el final de la calle
const int NUM_TRAMOS_CALLE = 4;
float posCalleX[NUM_TRAMOS_CALLE] = { 19.5f, 26.0f, 32.5f, 39.0f };
float posCalleZ = -27.0f;

//el camino de piedra sigue donde termina la calle, pasa por la puerta de la cerca y entra al jardin
const int NUM_PIEDRAS_CAMINO = 9;
glm::vec3 posCamino[NUM_PIEDRAS_CAMINO] = {
	glm::vec3(12.0f, 0.0f, -26.5f), glm::vec3(10.0f, 0.0f, -25.8f), glm::vec3(8.0f, 0.0f, -24.8f),
	glm::vec3(6.5f, 0.0f, -23.4f), glm::vec3(5.5f, 0.0f, -21.8f), glm::vec3(5.2f, 0.0f, -20.0f),
	glm::vec3(5.4f, 0.0f, -18.2f), glm::vec3(5.9f, 0.0f, -16.4f), glm::vec3(6.6f, 0.0f, -14.7f) };

//la cerca separa el jardin de la casa; la tercera pieza, que es la de en medio del lado derecho, es la puerta, por donde pasa el camino
const int NUM_CERCAS_JARDIN = 4;
float posCercaX[NUM_CERCAS_JARDIN] = { -15.6f, -5.2f, 5.2f, 15.6f };
float posCercaZ = -22.0f;
int indicePuerta = 2;
//la puerta mide 1.2 de ancho y las otras cercas 1.6, por eso la puerta se estira en X para que no queden huecos
float estiraPuerta = 1.33f;

//de los arboles, arbustos, flores y piedras se guarda la posicion, el tamano y el giro de cada uno; el mismo modelo se repite cambiando el giro
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
float escalaDetallesJardin[3] = { 0.6f, 0.25f, 2.0f }; //en orden, trebol, helecho y hongo

//--- nave de Among Us y crewmates ---
//el lugar de cada crewmate se mide desde el centro de la nave; el primero apenas va saliendo por la puerta y los demas estan
//cada vez mas lejos, girados en Y hacia distintos lados y ladeados en Z como si dieran vueltas
//la puerta esta en z = -4.5 y entre los motores, por eso los crewmates cercanos van mas al centro en X que los lejanos
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

//--- pista de carreras y naves ---
//puntos del centro de la pista, en el orden en que avanzan las naves
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

//--- satelites y cuerpos espaciales ---

//--- plataforma de lanzamiento y cohete ---
//el cohete son 5 piezas que van una encima de otra; armado mide 4.6 de alto
//cada pieza cuelga de la de abajo, asi al mover la base se mueve todo el cohete
const int NUM_PIEZAS_COHETE = 5;
const char* piezasCohete[NUM_PIEZAS_COHETE] = {
	"base", "tanque", "cuerpo", "anillo", "punta" };
int padresCohete[NUM_PIEZAS_COHETE] = {
	-1, 0, 1, 2, 3 };
glm::vec3 pivotesCohete[NUM_PIEZAS_COHETE] = {
	glm::vec3(0.0f, 0.0f, 0.0f), //base, con el motor y las patas
	glm::vec3(0.0f, 1.6f, 0.0f), //tanque
	glm::vec3(0.0f, 2.1f, 0.0f), //cuerpo, con las ventanas
	glm::vec3(0.0f, 3.1f, 0.0f), //anillo
	glm::vec3(0.0f, 3.8f, 0.0f)  //punta
};

//--- estacion espacial y naves ---
//las naves de un mismo pad estan en tres niveles, una arriba de otra, y el pad de X negativo solo tiene dos
//de cada nave se guarda el lugar donde se posa, medido desde el centro de la estacion, hacia donde sale, que siempre es
//hacia afuera de la estacion, el giro en Y con el que queda viendo hacia la estacion y el modelo que usa, que es 0 para
//la nave dorada, 1 para la blanca y 2 para la caza
const int NUM_NAVES_ESTACION = 11;
glm::vec3 posNavesEstacion[NUM_NAVES_ESTACION] = {
	//pad de Z negativo
	glm::vec3(0.0f, 35.60f, -207.2f),
	glm::vec3(0.0f, 13.70f, -197.5f),
	glm::vec3(0.0f, -8.05f, -197.5f),
	//pad de Z positivo
	glm::vec3(0.0f, 35.60f, 207.2f),
	glm::vec3(0.0f, 13.70f, 197.5f),
	glm::vec3(0.0f, -8.05f, 197.5f),
	//pad de X positivo
	glm::vec3(207.2f, 35.60f, 0.0f),
	glm::vec3(197.5f, 13.70f, 0.0f),
	glm::vec3(197.5f, -8.05f, 0.0f),
	//pad de X negativo
	glm::vec3(-207.2f, 35.60f, 0.0f),
	glm::vec3(-197.5f, 13.70f, 0.0f) };
glm::vec3 salidaNavesEstacion[NUM_NAVES_ESTACION] = {
	glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f) };
float giroNavesEstacion[NUM_NAVES_ESTACION] = {
	-180.0f, -180.0f, -180.0f,
	0.0f, 0.0f, 0.0f,
	90.0f, 90.0f, 90.0f,
	-90.0f, -90.0f };
int modeloNavesEstacion[NUM_NAVES_ESTACION] = {
	0, 1, 2,
	2, 0, 1,
	1, 2, 0,
	0, 2 };
//segundos que se le suman a cada espera; cada nave va tomando el siguiente de la lista, empezando en un lugar distinto,
//para que no salgan ni regresen todas al mismo tiempo
const int NUM_ESPERAS_NAVES = 16;
float esperasNaves[NUM_ESPERAS_NAVES] = {
	0.0f, 10.0f, 5.0f, 20.0f, 15.0f, 0.0f, 10.0f, 20.0f,
	5.0f, 15.0f, 10.0f, 0.0f, 20.0f, 5.0f, 15.0f, 10.0f };

//--- avatar ---
//pivotes del avatar, que usa las piezas de Kirby; los pies giran desde la cadera, mas arriba que en las de Kirby, para que
//se balanceen hacia adelante y hacia atras al caminar, y las manos giran en Y alrededor del centro del cuerpo para que
//se muevan pegadas a el
glm::vec3 pivotesAvatar[NUM_PIEZAS_KIRBY] = {
	glm::vec3(0.0f, 0.4158f, 0.0f),  //cuerpo
	glm::vec3(0.0f, 0.34f, 0.02f),   //mano_der
	glm::vec3(0.0f, 0.34f, 0.02f),   //mano_izq
	glm::vec3(-0.2f, 0.35f, 0.04f),  //pie_der
	glm::vec3(0.2f, 0.35f, 0.04f)    //pie_izq
};

//--- camaras y recorridos ---
//recorrido de la camara 5 en el espacio; el cohete sale de Pop Star en linea recta, pasa por arriba de la nave nodriza y
//de la estacion y sigue de largo; la camara lo sigue hasta finCoheteEspacio, que esta arriba de la estacion
glm::vec3 inicioCoheteEspacio = glm::vec3(-800.0f, 250.0f, 480.0f);
glm::vec3 finCoheteEspacio = glm::vec3(613.0f, 600.0f, -350.0f);

//recorrido de la camara 4; cada viaje va de la estacion a un planeta y en todos los arreglos el 0 es Xen, el 1 Pop Star
//y el 2 Verdania, en el orden de las teclas G, H y J
const int NUM_VIAJES = 3;
//zona del planeta a donde va, como las de Window
int zonaViajes[NUM_VIAJES] = { 1, 2, 3 };
//lo que sube en diagonal antes de girar; para ir a Verdania sube mas, porque si no pasaria por la estacion
float subidaViajes[NUM_VIAJES] = { 60.0f, 40.0f, 240.0f };
//rumbo con el que apunta al planeta en el espacio; sale del arco tangente del camino de donde termina de subir al planeta,
//sin contar la altura, y se le resto una vuelta a los que asi giran por el lado corto desde -90, que es como esta en el pad
float rumboViajes[NUM_VIAJES] = { 0.7f, -237.1f, -122.2f };
//lo que vuela antes de que la camara pase al planeta; es la distancia al planeta menos unas dos veces y media su radio,
//asi se ve muy lejos pero no lo toca
float distanciaViajes[NUM_VIAJES] = { 1600.0f, 2330.0f, 2400.0f };
//camara en la estacion, medida desde su centro; cada una ve el pad y hacia donde se va la nave en ese viaje
glm::vec3 camaraEstacionViajes[NUM_VIAJES] = {
	glm::vec3(-265.0f, 8.0f, 45.0f),
	glm::vec3(-265.0f, 8.0f, -45.0f),
	glm::vec3(-300.0f, 60.0f, 200.0f) };
//en el planeta, donde se estaciona la nave, el rumbo con el que llega y donde esta la camara, medidos desde el centro del piso
//son lugares libres: en Xen adelante de donde se llega, en Pop Star entre las dos colinas de Z positivo y en Verdania
//del lado contrario al jardin
glm::vec3 lugarLlegadaViajes[NUM_VIAJES] = {
	glm::vec3(6.0f, 5.0f, 22.0f),
	glm::vec3(45.0f, 5.0f, 50.0f),
	glm::vec3(28.0f, 5.0f, 30.0f) };
float rumboLlegadaViajes[NUM_VIAJES] = { 0.0f, 90.0f, 90.0f };
glm::vec3 camaraLlegadaViajes[NUM_VIAJES] = {
	glm::vec3(-25.0f, 8.0f, 30.0f),
	glm::vec3(10.0f, 6.0f, 15.0f),
	glm::vec3(0.0f, 6.0f, 10.0f) };

//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---
