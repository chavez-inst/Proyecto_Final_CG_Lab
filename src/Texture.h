#pragma once
#include<glew.h>

//textura 2D cargada desde una imagen (con stb_image)
//primero se crea con tex = Texture("Textures/archivo.png"); y luego se carga con tex.LoadTextureA(); o con LoadTexture si es jpg
class Texture
{
public:
	Texture();
	Texture(const char* FileLoc);
	bool LoadTexture();  //para imagenes sin transparencia (RGB), por ejemplo .jpg
	bool LoadTextureA(); //para imagenes con canal alfa (RGBA), por ejemplo .png o .tga
	void UseTexture();
	void ClearTexture();
	~Texture();
private:
	bool LoadTextureFile(bool useAlpha);
	GLuint textureID;
	int width, height, bitDepth;
	const char *fileLocation;

};
