#include "AudioManager.h"
#include <cstdio>

AudioManager::AudioManager()
	: system(nullptr),
	grupoMusica(nullptr),
	grupoAmbiente(nullptr),
	soundtrack(nullptr),
	canalSoundtrack(nullptr),
	canalAmbiente(nullptr),
	indiceAmbienteActual(-1),
	volumenAmbiente(0.55f)
{
	for (int i = 0; i < NUM_AMBIENTES; i++)
		ambientes[i] = nullptr;
}

AudioManager::~AudioManager()
{
	Liberar();
}

bool AudioManager::VerificarError(FMOD_RESULT resultado, const char* contexto)
{
	if (resultado != FMOD_OK)
	{
		printf("[FMOD Error - %s]: %s\n", contexto, FMOD_ErrorString(resultado));
		return false;
	}
	return true;
}

bool AudioManager::Inicializar()
{
	if (!VerificarError(FMOD::System_Create(&system), "System_Create"))
		return false;

	if (!VerificarError(system->init(32, FMOD_INIT_NORMAL, nullptr), "init"))
		return false;

	if (!VerificarError(system->createChannelGroup("Musica", &grupoMusica), "createChannelGroup Musica"))
		return false;
	if (!VerificarError(system->createChannelGroup("Ambiente", &grupoAmbiente), "createChannelGroup Ambiente"))
		return false;

	return true;
}

bool AudioManager::IniciarSoundtrack(const std::string& ruta, float volumen)
{
	if (!system)
		return false;

	if (soundtrack)
	{
		soundtrack->release();
		soundtrack = nullptr;
	}

	FMOD_RESULT res = system->createStream(
		ruta.c_str(),
		FMOD_2D | FMOD_LOOP_NORMAL,
		nullptr,
		&soundtrack
	);
	if (!VerificarError(res, "createStream soundtrack"))
		return false;

	res = system->playSound(soundtrack, grupoMusica, true, &canalSoundtrack);
	if (!VerificarError(res, "playSound soundtrack"))
		return false;

	if (canalSoundtrack)
	{
		canalSoundtrack->setVolume(volumen);
		canalSoundtrack->setPaused(false);
	}

	return true;
}

bool AudioManager::CargarAmbientes(const std::string& rutaEspacio,
	const std::string& rutaXen,
	const std::string& rutaVerdaniaPopStar,
	float volumen)
{
	if (!system)
		return false;

	volumenAmbiente = volumen;
	const std::string rutas[NUM_AMBIENTES] = { rutaEspacio, rutaXen, rutaVerdaniaPopStar };

	for (int i = 0; i < NUM_AMBIENTES; i++)
	{
		if (ambientes[i])
		{
			ambientes[i]->release();
			ambientes[i] = nullptr;
		}

		FMOD_RESULT res = system->createStream(
			rutas[i].c_str(),
			FMOD_2D | FMOD_LOOP_NORMAL,
			nullptr,
			&ambientes[i]
		);
		if (!VerificarError(res, rutas[i].c_str()))
			return false;
	}

	return true;
}

int AudioManager::ObtenerIndiceAmbiente(int zona)
{
	if (zona == 0)
		return 0; // Espacio
	if (zona == 1)
		return 1; // Xen
	if (zona == 2 || zona == 3)
		return 2; // Pop Star y Verdania comparten pista
	return -1;
}

void AudioManager::ActualizarAmbiente(int zonaActual)
{
	if (!system)
		return;

	int nuevoIndice = ObtenerIndiceAmbiente(zonaActual);
	// Si es el mismo ambiente (ej. Pop Star <-> Verdania), no se corta ni se reinicia
	if (nuevoIndice == indiceAmbienteActual || nuevoIndice < 0 || !ambientes[nuevoIndice])
		return;

	if (canalAmbiente)
	{
		canalAmbiente->stop();
		canalAmbiente = nullptr;
	}

	FMOD_RESULT res = system->playSound(ambientes[nuevoIndice], grupoAmbiente, true, &canalAmbiente);
	if (VerificarError(res, "playSound ambiente") && canalAmbiente)
	{
		canalAmbiente->setVolume(volumenAmbiente);
		canalAmbiente->setPaused(false);
		indiceAmbienteActual = nuevoIndice;
	}
}

void AudioManager::Actualizar()
{
	if (system)
		system->update();
}

void AudioManager::SetVolumenSoundtrack(float volumen)
{
	if (canalSoundtrack)
		canalSoundtrack->setVolume(volumen);
}

void AudioManager::PausarSoundtrack(bool pausar)
{
	if (canalSoundtrack)
		canalSoundtrack->setPaused(pausar);
}

void AudioManager::Liberar()
{
	if (soundtrack)
	{
		soundtrack->release();
		soundtrack = nullptr;
	}
	for (int i = 0; i < NUM_AMBIENTES; i++)
	{
		if (ambientes[i])
		{
			ambientes[i]->release();
			ambientes[i] = nullptr;
		}
	}
	if (grupoMusica)
	{
		grupoMusica->release();
		grupoMusica = nullptr;
	}
	if (grupoAmbiente)
	{
		grupoAmbiente->release();
		grupoAmbiente = nullptr;
	}
	if (system)
	{
		system->close();
		system->release();
		system = nullptr;
	}
	canalSoundtrack = nullptr;
	canalAmbiente = nullptr;
	indiceAmbienteActual = -1;
}