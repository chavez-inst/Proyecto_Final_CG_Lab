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

## Escenarios

| Tecla | Zona | Que hay |
|---|---|---|
| Z | 0, espacio | nave nodriza, pista de carreras, estacion espacial, satelite, los planetas vistos desde lejos y la nave de Among Us |
| X | 1, Xen | planeta de Half-Life: islas flotantes, cristales, plantas de luz, headcrab y especies de Xen |
| C | 2, Pop Star | planeta de Kirby: colinas, arboles, nubes, warp star y la plataforma del cohete con Gordon y el Capitan Falcon |
| V | 3, Verdania | planeta de Plants vs. Zombies: jardin de Crazy Dave, plantas y NPCs caminando |

## Elementos del escenario (60%, 410 pts + 60 extra)

| Punto de la rubrica | Pts | Responsable | Estado | Notas |
|---|---|---|---|---|
| Geometria 3D (modelos o geometria propia) | 10 | todos | en proceso | |
| Jerarquia en elementos del escenario | 10 | todos | hecho | Gordon, carnivora, Kirby, headcrab, plantas de Verdania y el cohete van por piezas (`DibujaJerarquia`) |
| Ambientacion de los universos y tematicas | 40 | todos | en proceso | Half-Life en Xen, Kirby en Pop Star, Plants vs. Zombies en Verdania y F-Zero en la pista; Among Us como extra |
| NPCs (otras especies intergalacticas) | 10 | Abraham | hecho | en Verdania: peashooter y crazy dave caminando, ice pea, gatling pea, fire pea y cactus; en Xen, el headcrab |
| Pista de carreras con naves corriendo | 20 | Dylan | hecho | las 8 naves de F-Zero le dan vueltas al circuito |
| Plataforma de lanzamiento con cohete | 20 | Abraham | hecho | en Pop Star, con su torre; el cohete despega con la F, deja una estela de humo y en el recorrido 4 llega al espacio |
| Estacion espacial (entran y salen naves) | 20 | Dylan | hecho | cuatro landing pads; las naves salen a hipervelocidad y regresan solas |
| Satelites y cuerpos espaciales | 10 | Dylan | en proceso | hay un satelite; faltan lunas o asteroides |
| Al menos 2 planetas (evolucion de especies) | 40 | Alejandro | en proceso | Xen, Pop Star y Verdania; falta la cadena de evolucion de Pop Star |
| Texturizado de todo | 20 | todos | en proceso | |
| Avatar jerarquico **no humanoide** | 10 | Alejandro y Abraham | hecho | Kirby o la carnivora, los dos por piezas; la T cambia de personaje y sirve en todos los planetas |
| Avatar con textura y material | 10 | Alejandro | hecho | su textura y `Material_avatar` |
| Avatar con caminata animada | 10 | Alejandro y Abraham | hecho | en Kirby se balancean los pies y las manos, y en la carnivora las hojas de abajo |
| Recorrido 1: 3a persona en cada planeta (plano XZ) | 10 | Alejandro | hecho | camara 2 |
| Recorrido 2: 3a persona como nave de la carrera | 10 | Alejandro | hecho | camara 3, Q y E cambian de nave |
| Recorrido 3: 3a persona de nave volando entre planetas | 10 | por definir | pendiente | va en la camara 4 |
| Recorrido 4: lanzamiento, estacion y objetos celestes | 10 | Alejandro | hecho | camara 5: ve despegar el cohete en Pop Star, lo sigue en el espacio hasta que pasa la estacion, lo ve alejarse y da una vuelta en circulo alrededor de la pista, la estacion y el satelite |
| Cambio correcto de camaras | 10 | Alejandro | en proceso | teclas 1 a 5; la 4 usa la camara libre mientras no este su recorrido |
| Luz direccional con ciclo dia/noche | 10 | Alejandro | hecho | en los planetas el sol da una vuelta cada 2 minutos (con sol y luna); la N salta a dia o noche; en el espacio el sol esta fijo |
| Luces puntuales que se prenden/apagan con el ciclo (max 2 min) | 10 | Alejandro | hecho | de noche: plantas de luz en Xen, warp star en Pop Star y farol en Verdania; en el espacio, faros de la meta |
| Skybox espacial que cambia segun el planeta | 10 | Alejandro | hecho | uno para el espacio y uno de dia y otro de noche para cada planeta |
| Spotlights que se prenden/apagan con teclado | 10 | Alejandro | hecho | K prende o apaga los de la zona y L la linterna (empieza apagada) |
| 3 animaciones basicas (se inician con teclado) | 30 | Alejandro y Abraham | hecho | con la F: headcrab en Xen, plantas en Verdania y despegue del cohete en Pop Star |
| 3 animaciones complejas (ciclo continuo con funciones) | 30 | todos | hecho | naves de la pista, naves de la estacion, islas flotantes y plantas de luz de Xen, Pop Star girando y NPCs caminando |
| Estilo visual | 30 | todos | en proceso | |
| Extra: originalidad | +30 | todos | en proceso | nave de Among Us con crewmates escondidos, carnivora como segundo avatar |
| Extra: animaciones complejas adicionales | +30 | todos | en proceso | las complejas que pasan de 3 |

## Metodologia (40%)

| Entregable | Pts | Responsable | Estado | Notas |
|---|---|---|---|---|
| Repo con **varias ramas por alumno** (una por avance) | 10 | todos | en proceso | ver la tabla de ramas abajo |
| Documento del manejo del repo y trabajo colaborativo (capturas) | 10 | por definir | pendiente | |
| Documento de propuesta corregido | 10 | Alejandro (borrador) | en proceso | todos complementan su parte |
| Plano/mapa final corregido | 10 | por definir | pendiente | base: plano de Dylan |
| Ejecutable Release (.exe) sin errores | 10 | por definir | pendiente | se compila con el repo en una carpeta local, no en Drive |
| Manual de usuario | 10 | Alejandro | en proceso | esta la version del avance; falta actualizarla al final |
| Manual tecnico (capturas, codigo, conclusiones, bibliografia) | 10 | por definir | pendiente | la bibliografia lleva los creditos de los modelos |
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
| `alejandro-planetas` | Alejandro | planetas: Xen y Pop Star (integrada) |
| `alejandro-creatividad1` | Alejandro | nave de Among Us y crewmates escondidos (integrada) |
| `alejandro-avatar-camara` | Alejandro | avatar, camaras 1 a 3 y teclas nuevas (F por escena, P pausa) (integrada) |
| `alejandro-correcciones` | Alejandro | texturas faltantes de las naves, aviso de pausa y plantas de Xen (integrada) |
| `alejandro-iluminacion` | Alejandro | ciclo de dia y noche, luces por zona, sol y luna, skyboxes de dia y noche, materiales (integrada) |
| `alejandro-orden` | Alejandro | orden de `main.cpp` por secciones y datos largos en `DatosEscena.h` (integrada) |
| `alejandro-comentarios` | Alejandro | correccion de comentarios (integrada) |
| `dylan-pista-y-cuerpos-espaciales` | Dylan | pista de carreras, naves de F-Zero y satelite (integrada) |
| `dylan-estacionEspacial` | Dylan | estacion espacial y naves que entran y salen (integrada) |
| `Abraham-NPCs` | Abraham | Verdania, NPCs y plantas de adorno, y la carnivora como segundo avatar (integrada) |
| `plataforma-abraham-alejandro-dylan` | Abraham | plataforma de lanzamiento y cohete en Pop Star, Gordon y el Capitan Falcon junto a ella (integrada) |
| `andrea-plataforma` | Andrea | rama de Andrea; por ahora solo trae su fila en esta tabla |

- Cada avance va en una rama nueva que sale de `main` (ver `README.md`, Trabajo en equipo con Git) y se
  agrega a esta tabla. Las ramas no se borran (el profesor las revisa).
- Nadie sube directo a `main`: se revisan e integran los avances de cada rama.
- Despues de cada integracion, cada quien trae `main` a su rama (ver `README.md`, Trabajo en equipo con Git).
