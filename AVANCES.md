# Avances del proyecto final

Grupo 13, 2027-1. Entrega final: **7 de noviembre** (Classroom).

Este archivo es para que todos sepamos que esta hecho, quien lleva que y que falta.
Cada quien actualiza sus filas cuando avance (y lo sube junto con su commit).

Estado: `hecho` / `en proceso` / `pendiente` / `por definir`

## Personajes y universos

| Integrante | Personaje | Universo | Modelo en el repo |
|---|---|---|---|
| Alejandro | Gordon Freeman | Half-Life | `Models/GordonParts` (jerarquico) |
| Dylan | Capitan Falcon | F-Zero | `Models/capitanFalcon` |
| Abraham | Planta carnivora (Chomper) | Plants vs. Zombies | `Models/Carnivora` (jerarquico) |
| Andrea | Kirby | Kirby | `Models/Kirby` (jerarquico) |

## Elementos del escenario (60%, 410 pts + 60 extra)

| Punto de la rubrica | Pts | Responsable | Estado | Notas |
|---|---|---|---|---|
| Geometria 3D (modelos o geometria propia) | 10 | todos | en proceso | |
| Jerarquia en elementos del escenario | 10 | todos | en proceso | Gordon, carnivora y Kirby ya estan por piezas (`DibujaJerarquia`) |
| Ambientacion de los universos y tematicas | 40 | todos | pendiente | cada quien mete elementos de su universo |
| NPCs (otras especies intergalacticas) | 10 | por definir | pendiente | gradas con aliens (plano), criaturas de Xen |
| Pista de carreras con naves corriendo | 20 | Dylan | en proceso | pista, nave y satelite cargados; falta que las naves recorran el circuito |
| Plataforma de lanzamiento con cohete | 20 | por definir | pendiente | |
| Estacion espacial (entran y salen naves) | 20 | por definir | pendiente | |
| Satelites y cuerpos espaciales | 10 | Dylan | en proceso | lunas, asteroides, satelites |
| Al menos 2 planetas (evolucion de especies) | 40 | Alejandro | en proceso | planeta 1: Xen (Half-Life); planeta 2: Dream Land (Kirby). Falta su cadena de evolucion |
| Texturizado de todo | 20 | todos | en proceso | |
| Avatar jerarquico **no humanoide** | 10 | Alejandro | en proceso | Kirby por piezas, uno solo para todos los planetas |
| Avatar con textura y material | 10 | Alejandro | en proceso | su textura y `Material_avatar` |
| Avatar con caminata animada | 10 | Alejandro | en proceso | pies y manos se balancean al caminar |
| Recorrido 1: 3a persona en cada planeta (plano XZ) | 10 | Alejandro | en proceso | camara 2 |
| Recorrido 2: 3a persona como nave de la carrera | 10 | Alejandro | en proceso | camara 3, Q y E cambian de nave |
| Recorrido 3: 3a persona de nave volando entre planetas | 10 | por definir | pendiente | |
| Recorrido 4: lanzamiento, estacion y objetos celestes | 10 | por definir | pendiente | |
| Cambio correcto de camaras | 10 | Alejandro | en proceso | teclas 1 a 5 |
| Luz direccional con ciclo dia/noche | 10 | Alejandro | hecho | en los planetas el sol da una vuelta cada 2 minutos (con sol y luna); la N salta a dia o noche; en el espacio el sol esta fijo |
| Luces puntuales que se prenden/apagan con el ciclo (max 2 min) | 10 | Alejandro | hecho | de noche: plantas de luz en Xen, warp star en el planeta 2 y farol en Verdania; en el espacio, faros de la meta |
| Skybox espacial que cambia segun el planeta | 10 | Alejandro | hecho | uno para el espacio y uno de dia y otro de noche para cada planeta (Xen, planeta 2 y Verdania) |
| Spotlights que se prenden/apagan con teclado | 10 | Alejandro | hecho | K prende o apaga los de la zona y L la linterna (empieza apagada) |
| 3 animaciones basicas (se inician con teclado) | 30 | por definir | pendiente | ej. cohete, compuerta, criatura |
| 3 animaciones complejas (ciclo continuo con funciones) | 30 | por definir | pendiente | ej. naves en la pista, lunas en orbita, naves de la estacion |
| Estilo visual | 30 | todos | pendiente | |
| Extra: originalidad | +30 | todos | pendiente | |
| Extra: animaciones complejas adicionales | +30 | todos | pendiente | |

## Metodologia (40%)

| Entregable | Pts | Responsable | Estado | Notas |
|---|---|---|---|---|
| Repo con **varias ramas por alumno** (una por avance) | 10 | todos | en proceso | ver la tabla de ramas abajo; falta la de Andrea |
| Documento del manejo del repo y trabajo colaborativo (capturas) | 10 | por definir | pendiente | |
| Documento de propuesta corregido | 10 | Alejandro (borrador) | en proceso | todos complementan su parte |
| Plano/mapa final corregido | 10 | por definir | pendiente | base: plano de Dylan |
| Ejecutable Release (.exe) sin errores | 10 | por definir | pendiente | |
| Manual de usuario | 10 | por definir | pendiente | |
| Manual tecnico (capturas, codigo, conclusiones, bibliografia) | 10 | por definir | pendiente | |
| Bitacora individual (por dia: que me propongo, que hice, horas) | 10 | cada quien | en proceso | |
| Video demo (sin voz) | 20 | por definir | pendiente | |
| Videoblog | 20 | todos | pendiente | |

## Reglas que hay que cuidar

- No cuentan objetos, animaciones, texturas ni iluminacion que ya se presentaron en practicas.
- Si lo que se presenta no lo hicimos nosotros (fuera de modelos y texturas descargados), el 60% del
  escenario vale **cero**. Las animaciones y el codigo tienen que ser nuestros.
- Solo usamos lo que vimos en el lab (ver `README.md`).

## Ramas

| Rama | De quien | Para que |
|---|---|---|
| `main` | todos | version que compila y junta lo de todos |
| `alejandro-planetas` | Alejandro | planetas: Xen y Dream Land (terminada e integrada) |
| `alejandro-creatividad1` | Alejandro | nave de Among Us y crewmates escondidos (terminada e integrada) |
| `alejandro-avatar-camara` | Alejandro | avatar, camaras 1 a 3 y teclas nuevas (F por escena, P pausa) (integrada) |
| `alejandro-correcciones` | Alejandro | texturas faltantes de las naves, aviso de pausa y plantas de Xen (integrada) |
| `alejandro-iluminacion` | Alejandro | ciclo de dia y noche, luces por zona, sol y luna, skyboxes de dia y noche, materiales |
| `abraham-npcs` | Abraham | NPCs |
| `dylan-pista-y-cuerpos-espaciales` | Dylan | pista de carreras y cuerpos espaciales |

- Cada avance va en una rama nueva que sale de `main` (ver `README.md`, Trabajo en equipo con Git) y se
  agrega a esta tabla. Las ramas no se borran (el profesor las revisa).
- Nadie sube directo a `main`: se revisan e integran los avances de cada rama.
- Despues de cada integracion, cada quien trae `main` a su rama (ver `README.md`, Trabajo en equipo con Git).
