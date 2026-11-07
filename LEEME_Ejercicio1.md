# Ejercicio 1: cohete con keyframes (despegue, separacion por fases y capsula en orbita)

Esta carpeta sale de la rama `Cohete_keyframes` de `chavez-inst/Proyecto_Final_CG_Lab`.
Esa rama era igual a `main` (el cohete solo tenia el despegue con estados), asi que aqui esta lo que faltaba.

## Que archivos son

| Archivo | Que es |
|---|---|
| `keyframes_cohete.txt` | **Los keyframes** (141). Es el txt que se entrega y el que lee el programa |
| `src/KeyframesCohete.h` | NUEVO. Lee/guarda el txt, interpola entre keyframes e imprime en consola |
| `src/main.cpp` | Dibuja el cohete por keyframes, la llama y el humo siguen a la pieza de abajo |
| `src/Window.cpp`, `src/Window.h` | Teclas B, R, I y O |
| `src/DatosEscena.h` | Centros de las piezas del cohete |
| `Proyecto_Final.vcxproj` | Agrega `KeyframesCohete.h` a la lista de archivos |
| `README.md` | Tabla de controles con las teclas nuevas |
| `herramientas/generar_keyframes_cohete.py` | Script que genero el txt (por si quieren cambiar tiempos, alturas o donde caen las piezas) |
| `cohete_keyframes.patch` | Los mismos cambios como parche, por si su `main.cpp` ya cambio |

## Como ponerlo en su proyecto

1. Copien estos archivos sobre la carpeta del repo respetando las carpetas (`src/...`).
   `keyframes_cohete.txt` va junto a `Proyecto_Final.vcxproj`, que es la carpeta de trabajo del programa.
2. Si su `main.cpp` ya no es el de la rama, en vez de sobrescribir apliquen el parche desde la raiz del repo:
   `git apply --ignore-whitespace cohete_keyframes.patch`
3. Compilen en **x64** y ejecuten. Al arrancar la consola dice `[keyframes] 141 keyframes cargados`.

## Teclas

| Tecla | Que hace |
|---|---|
| B | Lleva al planeta 2 (Pop Star) y reproduce la animacion. Imprime cada keyframe al pasar por el |
| R | Detiene la animacion y regresa el cohete a la plataforma (se puede volver a presionar B) |
| I | Imprime en la consola **todos** los keyframes guardados |
| O | Guarda los keyframes en `keyframes_cohete_guardado.txt` |
| P | Pausa y continua (tambien la animacion) |

## Lo que pasa en la animacion (segundos)

| Tiempo | Fase |
|---|---|
| 0 - 3 | Cuenta regresiva: el cohete vibra y la llama crece |
| 3 | Despegue, sube cada vez mas rapido |
| 7.5 | Se suelta la **base**, cae y rebota; la llama pasa al tanque |
| 10 | Se suelta el **tanque** |
| 12.5 | Se suelta el **cuerpo** |
| 15 | Se suelta el **anillo**; la **capsula** (punta) hace una espiral y apaga su motor |
| 22 en adelante | La capsula orbita (una vuelta cada 8 s) y el ciclo se repite desde el keyframe 108 mientras las 4 piezas quedan en el piso |

Las piezas caen del lado de X negativo de la plataforma, lejos de la torre, de Gordon, del Capitan Falcon y de las colinas.
Gordon y el Capitan Falcon voltean a ver el cohete mientras dura.

## Que entregar

- **Capturas de la consola**: presionen `I` (lista completa de keyframes) y tambien capturen la consola mientras corre la animacion con `B`.
- **El txt**: `keyframes_cohete.txt` (cualquiera lo reproduce copiandolo junto al `.vcxproj` y presionando B).
- **Video**: graben la ventana mientras ejecutan B (Win + G, OBS, etc.). Con la camara libre (tecla 1) suban con Espacio y retrocedan
  para ver toda la secuencia: la orbita queda a unas 260 unidades de altura.

## Formato del txt

```
LOOP_DESDE 108
FRAME tiempo fuego piezaFuego | posX posY posZ rotX rotY rotZ (x5 piezas: base, tanque, cuerpo, anillo, punta)
```
- `pos` es el desplazamiento del centro de la pieza respecto a donde esta armada y `rot` el giro en grados sobre su propio centro.
- `fuego` es el tamano de la llama (0 a 1) y `piezaFuego` la pieza que la lleva.
- Entre keyframes se interpola linealmente; `LOOP_DESDE n` regresa al keyframe n al terminar.
