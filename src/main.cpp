/*
Proyecto Final - Computacion Grafica
Base del lab: ventana, camara, iluminacion, texturas, modelos (Assimp) y skybox
Controles: WASD y mouse mueven la camara, Y/U mueven el cubo, ESC cierra

Como esta acomodado este archivo (busquen los bloques ===):
 1. variables globales (texturas, modelos, materiales, luces)
 2. geometria hecha a mano (piso, plano de vegetacion, cubo)
 3. main: carga de texturas, modelos, skybox, materiales y luces
 4. while: aqui se dibuja todo cada frame
*/

//esto solo va aqui, en ningun otro archivo (si no da error de simbolos repetidos)
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
std::vector<MeshModel*> meshListModel; //mallas hechas a mano, cada vertice es x y z, u v, nx ny nz
std::vector<Shader> shaderList;

Camera camera;

//texturas (los archivos van en la carpeta Textures)
Texture pruebaTexture;
Texture vegetacionTexture;

//modelos (los archivos van en Models y sus texturas en Textures)
Model Prueba_M;

//MODELOS CARRERA
Model pistaM;
Model sateliteRace;
Model nave1Race;
Model capitanFalcon;

//modelos con jerarquia: cada uno esta separado en piezas (.obj) que comparten el mismo origen,
//todas con Y arriba, los pies en y = 0 y mirando hacia +Z. "der" e "izq" son la derecha e izquierda
//del personaje (la derecha queda en X negativa)
//por cada pieza hay: nombre del archivo, padre (indice de la pieza de la que cuelga, -1 = raiz),
//pivote (punto de la articulacion donde gira) y rotacion en grados (x, y, z) que se puede cambiar en el while
//el padre siempre va antes que sus hijos en los arreglos

//planta carnivora: mide ~1.4 de alto
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

//Gordon: mide ~1.4 de alto
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
	glm::vec3(0.0f, 0.73277f, 0.01014f),       //torso (cadera)
	glm::vec3(0.0f, 1.22766f, -0.01482f),      //cabeza (cuello)
	glm::vec3(-0.14675f, 1.11121f, -0.03717f), //brazo_der (hombro)
	glm::vec3(-0.29002f, 0.94174f, -0.04760f), //antebrazo_der (codo)
	glm::vec3(-0.43085f, 0.77516f, -0.04468f), //mano_der (muneca)
	glm::vec3(0.14675f, 1.11121f, -0.03717f),  //brazo_izq
	glm::vec3(0.29002f, 0.94174f, -0.04760f),  //antebrazo_izq
	glm::vec3(0.43085f, 0.77516f, -0.04469f),  //mano_izq
	glm::vec3(-0.07392f, 0.73277f, 0.01014f),  //muslo_der (cadera)
	glm::vec3(-0.07569f, 0.39412f, -0.00743f), //pantorrilla_der (rodilla)
	glm::vec3(-0.07732f, 0.08130f, -0.03441f), //pie_der (tobillo)
	glm::vec3(0.07392f, 0.73277f, 0.01014f),   //muslo_izq
	glm::vec3(0.07569f, 0.39412f, -0.00743f),  //pantorrilla_izq
	glm::vec3(0.07732f, 0.08130f, -0.03441f)   //pie_izq
};
glm::vec3 rotGordon[NUM_PIEZAS_GORDON];

//Kirby: mide ~0.73 de alto
const int NUM_PIEZAS_KIRBY = 5;
Model Kirby_M[NUM_PIEZAS_KIRBY];
const char* piezasKirby[NUM_PIEZAS_KIRBY] = {
	"cuerpo", "mano_der", "mano_izq", "pie_der", "pie_izq" };
int padresKirby[NUM_PIEZAS_KIRBY] = { -1, 0, 0, 0, 0 };
glm::vec3 pivotesKirby[NUM_PIEZAS_KIRBY] = {
	glm::vec3(0.0f, 0.4158f, 0.0f),         //cuerpo (centro)
	glm::vec3(-0.2024f, 0.3899f, 0.0164f),  //mano_der
	glm::vec3(0.2014f, 0.3897f, 0.0170f),   //mano_izq
	glm::vec3(-0.1323f, 0.1498f, -0.0135f), //pie_der
	glm::vec3(0.1317f, 0.1505f, -0.0139f)   //pie_izq
};
glm::vec3 rotKirby[NUM_PIEZAS_KIRBY];

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

//luz direccional (el "sol", solo hay una)
DirectionalLight mainLight;
//arreglos de luces, el maximo de cada una esta en CommonValues.h
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

//shaders que usan todos los objetos (los del skybox los carga Skybox.cpp)
static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";


// ================== 2. GEOMETRIA HECHA A MANO ==================

//calcula las normales promediando las de los triangulos que comparten cada vertice
//se llama antes de CreateMeshModel porque ahi es donde se copian los vertices a la GPU
//ojo: en este proyecto las normales apuntan hacia ADENTRO del objeto, asi que los triangulos
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

	//normales hacia abajo (hacia adentro del piso) por como el shader calcula la luz
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


//cubo de 1x1x1 centrado en el origen, con la imagen completa en cada cara
void CrearCubo()
{
	//cada normal apunta hacia ADENTRO del cubo (al reves de su cara): el shader calcula la luz con
	//(FragPos - posicion de la luz), entonces si apuntaran hacia afuera esa cara se veria oscura
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

		 // back (la S va al reves para que la imagen no salga en espejo)
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


//dibuja un modelo con jerarquia: la matriz de cada pieza es la de su padre mas su propia rotacion
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

	//texturas: primero se asigna la ruta y luego se carga
	//LoadTextureA para imagenes con canal alfa (png, tga) y LoadTexture para las que no tienen (jpg)
	pruebaTexture = Texture("Textures/prueba.png");
	pruebaTexture.LoadTextureA();
	vegetacionTexture = Texture("Textures/prueba_alpha.png");
	vegetacionTexture.LoadTextureA();

	//modelos
	Prueba_M = Model();
	Prueba_M.LoadModel("Models/prueba.obj");

	//MODELOS CARRERA
	pistaM = Model();
	pistaM.LoadModel("Models/pista/pista.obj");
	sateliteRace = Model();
	sateliteRace.LoadModel("Models/sateliteRace/sateliteRace.obj");
	nave1Race = Model();
	nave1Race.LoadModel("Models/nave1Race/nave1Race.obj");
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

	//skybox: las 6 caras van en este orden: rt, lf, dn, up, bk, ft
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/sBRight.png");// +X (Derecha)
	skyboxFaces.push_back("Textures/Skybox/Left.png");// -X (Izquierda)
	skyboxFaces.push_back("Textures/Skybox/DownF.png");// -Y (Abajo)
	skyboxFaces.push_back("Textures/Skybox/Top.png");// +Y (Arriba)
	skyboxFaces.push_back("Textures/Skybox/FrontF.png");// +Z (Atrás)
	skyboxFaces.push_back("Textures/Skybox/BackF.png");// -Z (Enfrente)

	skybox = Skybox(skyboxFaces);

	//materiales: Material(intensidad especular, shininess)
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	// ---- luces ----
	//los parametros van en este orden:
	//DirectionalLight(r, g, b, intensidad ambiental, intensidad difusa, direccion x, y, z)
	//PointLight(r, g, b, ambiental, difusa, posicion x, y, z, constante, lineal, exponente)
	//SpotLight(r, g, b, ambiental, difusa, posicion x, y, z, direccion x, y, z, constante, lineal, exponente, angulo del cono)

	//luz direccional: solo hay 1 y siempre tiene que existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.2f, 0.4f,
		0.0f, -1.0f, -1.0f);

	//contador de luces puntuales (no puede pasar de MAX_POINT_LIGHTS)
	unsigned int pointLightCount = 0;
	//luz puntual blanca
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,
		0.0f, 0.6f,
		2.0f, 1.5f, -3.0f,
		1.0f, 0.05f, 0.005f);
	pointLightCount++;

	//contador de spotlights (no puede pasar de MAX_SPOT_LIGHTS)
	unsigned int spotLightCount = 0;
	//linterna: su posicion y direccion se actualizan en el while con la camara
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
	//para agregar otra luz: se pone en el siguiente lugar libre del arreglo y se suma 1 al contador

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	//ojo: glm::perspective usa radianes, asi que el 45.0f no son 45 grados, pero asi venia del lab y se ve bien
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

		//limpiar la pantalla
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//el skybox va primero, antes que todo lo demas
		// Reemplaza la línea 185 en mainProyecto_3.cpp:
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection, deltaTime);

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
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		//linterna pegada a la camara, un poco abajo de los ojos
		//aqui en el while es donde se pueden cambiar cosas de las luces mientras corre el programa
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//mandar todas las luces al shader
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		//para dibujar cada objeto siempre es lo mismo: matriz model, color, textura, material y render
		glm::mat4 model(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f); //blanco = la textura se ve con sus colores normales

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

		//cubo: se mueve en X con las teclas Y/U (ver Window.cpp)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(mainWindow.getmuevex(), -0.5f, -4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pruebaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[2]->RenderMeshModel();

		//modelo cargado de archivo: trae sus propias texturas, por eso aqui no va UseTexture
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(4.0f, -1.0f, -3.0f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Prueba_M.RenderModel();

		//****************MODELOS OPACOS PARA CARRERA***********//////////////
		// 
		//PISTA RACE
		model = glm::mat4(1.0f);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(240.0f, 10.0f, -130.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pistaM.RenderModel();

		//SATELITE RACE
		model = glm::mat4(1.0f);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(340.0f, 50.0f, -530.0f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sateliteRace.RenderModel();

		//NAVE1 RACE
		model = glm::mat4(1.0f);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(270.0f, 40.0f, -130.0f));
		model = glm::rotate(model, 30.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		nave1Race.RenderModel();

		model = glm::mat4(1.0f);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(240.0f, 15.0f, -130.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		capitanFalcon.RenderModel();

		//modelos con jerarquia: los pies estan en y = 0, por eso se bajan a -1 (altura del piso)
		//para articular se cambian las rotaciones, ej. rotGordon[2].x = -30.0f; mueve el brazo derecho en el hombro
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

		// ---------- objetos con transparencia ----------
		//van al final, despues de todos los opacos, si no se ven mal

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.5f, 0.0f, -6.5f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		//blending: para transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		vegetacionTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[1]->RenderMeshModel();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
