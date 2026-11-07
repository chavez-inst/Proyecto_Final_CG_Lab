#pragma once
#include <cstdio>
#include <cmath>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include <glm.hpp>

//--- plataforma de lanzamiento y cohete: animacion por keyframes ---
//El cohete tiene 5 piezas: 0 base, 1 tanque, 2 cuerpo, 3 anillo y 4 punta (la capsula).
//Cada keyframe guarda, por pieza, cuanto se desplaza su centro respecto a donde esta armada y cuanto gira
//alrededor de su propio centro; tambien guarda el tamano de la llama y que pieza la lleva.
//Los keyframes se leen de un archivo de texto (keyframes_cohete.txt) y entre uno y otro se interpola de forma lineal.

const int KF_PIEZAS = 5;
const char* const KF_NOMBRES[KF_PIEZAS] = { "base", "tanque", "cuerpo", "anillo", "punta" };

struct KeyframeCohete
{
	float tiempo;               //segundos desde que empieza la animacion
	float fuego;                //tamano de la llama, de 0 (apagada) a 1 (completa)
	int piezaFuego;             //pieza que lleva la llama; no se interpola
	glm::vec3 pos[KF_PIEZAS];   //desplazamiento del centro de cada pieza
	glm::vec3 rot[KF_PIEZAS];   //giro de cada pieza en grados, orden X, Y, Z
};

class AnimacionCohete
{
public:
	std::vector<KeyframeCohete> frames;
	int loopDesde;              //al llegar al ultimo keyframe se regresa a este; -1 si no hay ciclo

	bool activa;                //true mientras se esta reproduciendo (o la capsula esta orbitando)
	float tiempo;               //segundos de animacion transcurridos

	//lo que se dibuja en este momento, ya interpolado
	float fuego;
	int piezaFuego;
	glm::vec3 pos[KF_PIEZAS];
	glm::vec3 rot[KF_PIEZAS];

	AnimacionCohete()
	{
		loopDesde = -1;
		activa = false;
		tiempo = 0.0f;
		ultimoImpreso = -1;
		avisoCiclo = false;
		fuego = 0.0f;
		piezaFuego = 0;
		for (int i = 0; i < KF_PIEZAS; i++)
		{
			pos[i] = glm::vec3(0.0f);
			rot[i] = glm::vec3(0.0f);
		}
	}

	//lee el archivo de keyframes; regresa false si no se pudo y avisa por que
	bool Carga(const char* ruta)
	{
		std::ifstream archivo(ruta);
		if (!archivo.is_open())
		{
			printf("[keyframes] no se pudo abrir %s; hay que ponerlo junto al .vcxproj (carpeta de trabajo del programa)\n", ruta);
			return false;
		}
		std::vector<KeyframeCohete> leidos;
		int ciclo = -1;
		std::string linea;
		int numLinea = 0;
		while (std::getline(archivo, linea))
		{
			numLinea++;
			size_t comentario = linea.find('#');
			if (comentario != std::string::npos)
				linea = linea.substr(0, comentario);
			for (size_t i = 0; i < linea.size(); i++)
			{
				if (linea[i] == '|' || linea[i] == ',' || linea[i] == '\r')
					linea[i] = ' ';
			}
			std::istringstream ss(linea);
			std::string palabra;
			if (!(ss >> palabra))
				continue;
			if (palabra == "LOOP_DESDE")
			{
				ss >> ciclo;
			}
			else if (palabra == "FRAME")
			{
				KeyframeCohete k;
				bool bien = (ss >> k.tiempo >> k.fuego >> k.piezaFuego) ? true : false;
				for (int i = 0; i < KF_PIEZAS && bien; i++)
				{
					bien = (ss >> k.pos[i].x >> k.pos[i].y >> k.pos[i].z >> k.rot[i].x >> k.rot[i].y >> k.rot[i].z) ? true : false;
				}
				if (!bien || k.piezaFuego < 0 || k.piezaFuego >= KF_PIEZAS)
				{
					printf("[keyframes] linea %d de %s mal escrita, se ignora\n", numLinea, ruta);
					continue;
				}
				if (!leidos.empty() && k.tiempo <= leidos.back().tiempo)
				{
					printf("[keyframes] linea %d: el tiempo tiene que ir creciendo, se ignora\n", numLinea);
					continue;
				}
				leidos.push_back(k);
			}
		}
		if (leidos.size() < 2)
		{
			printf("[keyframes] %s necesita al menos 2 keyframes\n", ruta);
			return false;
		}
		frames = leidos;
		loopDesde = (ciclo >= 0 && ciclo < (int)frames.size() - 1) ? ciclo : -1;
		printf("[keyframes] %d keyframes cargados de %s (duracion %.2f s%s)\n", (int)frames.size(), ruta, frames.back().tiempo,
			loopDesde >= 0 ? ", con ciclo" : "");
		return true;
	}

	//guarda los keyframes que estan en memoria en un archivo de texto con el mismo formato
	bool Guarda(const char* ruta)
	{
		if (frames.empty())
		{
			printf("[keyframes] no hay keyframes que guardar\n");
			return false;
		}
		std::ofstream f(ruta);
		if (!f.is_open())
		{
			printf("[keyframes] no se pudo escribir %s\n", ruta);
			return false;
		}
		char texto[256];
		f << "# Keyframes del cohete: FRAME tiempo fuego piezaFuego | por pieza (base, tanque, cuerpo, anillo, punta): posX posY posZ rotX rotY rotZ\n";
		if (loopDesde >= 0)
			f << "LOOP_DESDE " << loopDesde << "\n";
		for (size_t n = 0; n < frames.size(); n++)
		{
			const KeyframeCohete& k = frames[n];
			snprintf(texto, sizeof(texto), "FRAME %.3f %.3f %d", k.tiempo, k.fuego, k.piezaFuego);
			f << texto;
			for (int i = 0; i < KF_PIEZAS; i++)
			{
				snprintf(texto, sizeof(texto), " | %.3f %.3f %.3f %.2f %.2f %.2f", k.pos[i].x, k.pos[i].y, k.pos[i].z,
					k.rot[i].x, k.rot[i].y, k.rot[i].z);
				f << texto;
			}
			f << "\n";
		}
		f.close();
		printf("[keyframes] %d keyframes guardados en %s\n", (int)frames.size(), ruta);
		return true;
	}

	//imprime un keyframe en consola, una linea por pieza
	void ImprimeKeyframe(int n)
	{
		if (n < 0 || n >= (int)frames.size())
			return;
		const KeyframeCohete& k = frames[n];
		printf("KEYFRAME %3d/%d  t = %6.3f s  fuego = %.2f (pieza %d: %s)\n", n, (int)frames.size() - 1, k.tiempo, k.fuego,
			k.piezaFuego, KF_NOMBRES[k.piezaFuego]);
		for (int i = 0; i < KF_PIEZAS; i++)
		{
			printf("    %-7s pos(%8.3f, %8.3f, %8.3f)  rot(%8.2f, %8.2f, %8.2f)\n", KF_NOMBRES[i],
				k.pos[i].x, k.pos[i].y, k.pos[i].z, k.rot[i].x, k.rot[i].y, k.rot[i].z);
		}
	}

	//imprime todos los keyframes guardados
	void ImprimeTodos()
	{
		printf("===== KEYFRAMES DEL COHETE (%d)%s =====\n", (int)frames.size(), loopDesde >= 0 ? "" : " sin ciclo");
		if (loopDesde >= 0)
			printf("ciclo: al terminar regresa al keyframe %d\n", loopDesde);
		for (int n = 0; n < (int)frames.size(); n++)
			ImprimeKeyframe(n);
		printf("=====================================================\n");
	}

	//empieza la animacion desde el primer keyframe
	void Inicia()
	{
		if (frames.size() < 2)
		{
			printf("[keyframes] no hay keyframes cargados, no se puede reproducir\n");
			activa = false;
			return;
		}
		activa = true;
		tiempo = 0.0f;
		ultimoImpreso = -1;
		avisoCiclo = false;
		printf("[keyframes] reproduciendo la animacion del cohete\n");
		Interpola();
		ImprimeSiCambio();
	}

	//la quita y el cohete vuelve a la plataforma
	void Detiene()
	{
		if (activa)
			printf("[keyframes] animacion detenida, el cohete regresa a la plataforma\n");
		activa = false;
		tiempo = 0.0f;
		ultimoImpreso = -1;
	}

	//avanza la animacion los segundos que le pasen y deja en pos, rot y fuego lo que hay que dibujar
	void Actualiza(float segundos)
	{
		if (!activa || frames.size() < 2)
			return;
		tiempo += segundos;
		float ultimo = frames.back().tiempo;
		if (tiempo > ultimo)
		{
			if (loopDesde >= 0)
			{
				float inicioCiclo = frames[loopDesde].tiempo;
				tiempo = inicioCiclo + fmodf(tiempo - inicioCiclo, ultimo - inicioCiclo);
				if (!avisoCiclo)
				{
					printf("[keyframes] la capsula queda orbitando: el ciclo se repite desde el keyframe %d\n", loopDesde);
					avisoCiclo = true;
				}
			}
			else
			{
				tiempo = ultimo;
			}
		}
		Interpola();
		ImprimeSiCambio();
	}

private:
	int ultimoImpreso;   //ultimo keyframe que ya se imprimio al reproducir
	bool avisoCiclo;

	//keyframe de donde sale el segmento en el que esta el tiempo actual
	int Segmento()
	{
		int n = 0;
		while (n + 2 < (int)frames.size() && frames[n + 1].tiempo <= tiempo)
			n++;
		return n;
	}

	void Interpola()
	{
		int n = Segmento();
		const KeyframeCohete& a = frames[n];
		const KeyframeCohete& b = frames[n + 1];
		float u = (tiempo - a.tiempo) / (b.tiempo - a.tiempo);
		if (u < 0.0f)
			u = 0.0f;
		if (u > 1.0f)
			u = 1.0f;
		fuego = a.fuego + (b.fuego - a.fuego) * u;
		piezaFuego = a.piezaFuego;
		for (int i = 0; i < KF_PIEZAS; i++)
		{
			pos[i] = a.pos[i] + (b.pos[i] - a.pos[i]) * u;
			rot[i] = a.rot[i] + (b.rot[i] - a.rot[i]) * u;
		}
	}

	//cada vez que la animacion pasa por un keyframe nuevo se imprime en la consola
	void ImprimeSiCambio()
	{
		int n = Segmento();
		if (tiempo >= frames[n + 1].tiempo)
			n++;  //justo en el ultimo keyframe
		while (ultimoImpreso < n)
		{
			ultimoImpreso++;
			ImprimeKeyframe(ultimoImpreso);
		}
	}
};
