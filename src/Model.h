#pragma once
#include <vector>
#include<string>
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include "Mesh_tn.h"
#include "Texture.h"

//modelo cargado con Assimp desde un archivo .obj, .fbx u otro formato
//sus texturas se buscan en la carpeta Textures, sin importar la ruta que diga el archivo del modelo
class Model
{
public:
	Model();

	void LoadModel(const std::string& fileName);
	void RenderModel();
	void ClearModel(); //libera las mallas y texturas del modelo (despues ya no se puede dibujar)

	~Model();

private:
	void LoadNode(aiNode* node, const aiScene* scene);
	void LoadMesh(aiMesh* mesh, const aiScene* scene);
	void LoadMaterials(const aiScene* scene);
	std::vector<MeshModel*>MeshList;
	std::vector<Texture*>TextureList;
	std::vector<unsigned int>meshTotex; //que material usa cada malla, y con eso que textura le toca
	std::string carpeta; //carpeta del modelo, por ejemplo "Models/Carnivora/", donde se buscan primero sus texturas
};
