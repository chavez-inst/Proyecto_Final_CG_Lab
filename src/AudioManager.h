#pragma once

#include <fmod.hpp>
#include <fmod_errors.h>
#include <string>

class AudioManager
{
public:
	static const int NUM_AMBIENTES = 3;

	AudioManager();
	~AudioManager();

	bool Inicializar();
	bool IniciarSoundtrack(const std::string& ruta, float volumen = 0.25f);

	// Carga los 3 archivos de ambiente (0: Espacio, 1: Xen, 2: PopStar/Verdania)
	bool CargarAmbientes(const std::string& rutaEspacio,
		const std::string& rutaXen,
		const std::string& rutaVerdaniaPopStar,
		float volumen = 0.55f);

	// Cambia de ambiente solo si la zona usa una pista distinta
	void ActualizarAmbiente(int zonaActual);

	void Actualizar();
	void Liberar();

	void SetVolumenSoundtrack(float volumen);
	void PausarSoundtrack(bool pausar);

private:
	FMOD::System* system;
	FMOD::ChannelGroup* grupoMusica;
	FMOD::ChannelGroup* grupoAmbiente;

	// Soundtrack
	FMOD::Sound* soundtrack;
	FMOD::Channel* canalSoundtrack;

	// Ambientes (0: Espacio, 1: Xen, 2: PopStar y Verdania)
	FMOD::Sound* ambientes[NUM_AMBIENTES];
	FMOD::Channel* canalAmbiente;
	int indiceAmbienteActual;
	float volumenAmbiente;

	int ObtenerIndiceAmbiente(int zona);
	bool VerificarError(FMOD_RESULT resultado, const char* contexto);
};