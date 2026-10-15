#include "Skybox.h"

Skybox::Skybox()
{
	skyMesh = nullptr;
	skyShader = nullptr;
	textureId = 0;
	uniformProjection = 0;
	uniformView = 0;
}

//faceLocations: las 6 imagenes en el orden rt, lf, dn, up, bk, ft
Skybox::Skybox(std::vector<std::string> faceLocations)
{
	skyShader = new Shader();
	skyShader->CreateFromFiles("shaders/skybox.vert", "shaders/skybox.frag");
	uniformProjection = skyShader->GetProjectionLocation();
	uniformView = skyShader->GetViewLocation();

	//malla del skybox: un cubo de 2x2x2
	unsigned int skyboxIndices[] = {
		// front
		0, 1, 2,
		2, 1, 3,
		// right
		2, 3, 5,
		5, 3, 7,
		// back
		5, 7, 4,
		4, 7, 6,
		// left
		4, 6, 0,
		0, 6, 1,
		// top
		4, 0, 5,
		5, 0, 2,
		// bottom
		1, 6, 3,
		3, 6, 7
	};

	//solo se usa la posicion, UV y normales van en 0 porque la textura se toma con la direccion del vertice
	float skyboxVertices[] = {
		-1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//v0
		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//v1
		1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//v2
		1.0f, -1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//v3
		-1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//v4
		1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//v5
		-1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//v6
		1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f//v7
	};
	skyMesh = new MeshModel();
	skyMesh->CreateMeshModel(skyboxVertices, skyboxIndices, 64, 36);

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	//voltear las imagenes igual que en Texture.cpp
	stbi_set_flip_vertically_on_load(true);
	int width, height, bitDepth;
	for (size_t i = 0; i < 6; i++)
	{
		unsigned char *texData = stbi_load(faceLocations[i].c_str(), &width, &height, &bitDepth, STBI_rgb);
		if (!texData)
		{
			//si falta una cara el skybox se ve negro, pero el programa sigue
			printf("No se encontro la cara del skybox: %s\n", faceLocations[i].c_str());
			return;
		}
		//sin canal alfa, el skybox no necesita transparencia
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
		stbi_image_free(texData);
	}
	//CLAMP_TO_EDGE para que no se noten las uniones entre caras
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

//se dibuja antes que el resto de la escena
void Skybox::DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, GLfloat deltaTime)
{
	// 1. Eliminar la traslación de la cámara (mantiene solo orientación)
	viewMatrix = glm::mat4(glm::mat3(viewMatrix));

	// 2. Acumuladores de ángulo continuos (velocidades ajustables)
	static float angleY = 0.0f; // Rotación principal (horizontal)
	static float angleX = 0.0f; // Inclinación axial
	static float angleZ = 0.0f; // Bamboleo sutil

	// Ajusta los multiplicadores para cambiar la velocidad de giro:
	angleY += 0.035f * deltaTime;  // Giro constante sobre el eje Y
	angleX += 0.025f * deltaTime;  // Giro lento sobre el eje X
	angleZ += 0.01f * deltaTime;  // Giro muy lento sobre el eje Z

	// 3. Aplicar las rotaciones a la matriz de vista del skybox
	viewMatrix = glm::rotate(viewMatrix, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(angleZ), glm::vec3(0.0f, 0.0f, 1.0f));

	glDepthMask(false);
	skyShader->UseShader();
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
	skyMesh->RenderMeshModel();
	glDepthMask(true);
}


Skybox::~Skybox()
{
}
