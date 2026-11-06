# Proyecto Final - Computacion Grafica

Base del proyecto final con lo que vimos en el lab: ventana, camara, figuras hechas a mano,
texturas, modelos cargados con Assimp, skybox e iluminacion (direccional, puntual y spotlight).

---

## Como empezar (solo la primera vez)

1. Clonar el repo (con GitHub Desktop o con `git clone`).
2. Extraer `Librerias.zip` (lo pasamos por fuera, no esta en GitHub) **dentro de la carpeta del repo**.
   Con clic derecho > "Extraer todo" y dejando la ruta que sugiere Windows ya queda bien.
   Al final tiene que quedar asi:
   ```
   Proyecto_Final_CG_Lab/
     Librerias/
       dll/  glm/  include/  lib/
     Models/
     shaders/
     src/
     Textures/
     Proyecto_Final.slnx
     ...
   ```
   Si queda `Librerias/Librerias/...` o las carpetas `glm`, `include`, etc. sueltas afuera, hay que acomodarlo.
3. Abrir `Proyecto_Final.slnx` con Visual Studio 2026.
4. Arriba revisar que diga **Debug** y **x64**, y darle a **F5** (o al boton verde).

Si al compilar sale `Falta la carpeta Librerias...` es que el paso 2 no quedo bien.

## Controles

| Tecla       | Que hace                     |
|-------------|------------------------------|
| W A S D     | Con la camara libre, moverla. Con la del avatar, caminar con el avatar (W y S avanzan y retroceden, A y D giran) |
| Mouse       | Voltear la camara libre      |
| Espacio / Ctrl | Subir y bajar la camara libre |
| Shift       | Ir al doble de rapido (camara libre o avatar) |
| T           | Cambiar el avatar entre Kirby y la planta carnivora |
| 1 2 3 4 5   | Cambiar de camara: 1 libre, 2 avatar, 3 nave de la carrera, 4 nave entre planetas, 5 recorrido del lanzamiento y la estacion |
| Q / E       | Nave anterior o siguiente en la camara 3 |
| Z           | Ir al espacio; si ya se esta ahi, ir al siguiente punto (inicio, nave nodriza y pista) |
| X / C / V   | Ir a Xen, al planeta 2 o a Verdania; si ya se esta ahi, regresar al punto de llegada |
| F           | Prender o apagar la animacion basica de la escena (headcrab en Xen, despegue del cohete en Pop Star, plantas en Verdania) |
| P           | Pausar o continuar todas las animaciones (tambien el ciclo de dia y noche) |
| L           | Prender o apagar la linterna (empieza apagada) |
| K           | Prender o apagar los spotlights de la zona donde se esta |
| N           | En un planeta, saltar a la manana o a la noche (el ciclo sigue desde ahi) |
| ESC         | Cerrar                       |

La camara 2 solo funciona en los planetas (desde el espacio lleva a Xen) y la 3 y la 4 solo en el
espacio (desde un planeta llevan al espacio). La 4 se comporta como la libre mientras no exista su
recorrido (la nave entre planetas). La 5 es un recorrido que se repite solo: ve despegar el cohete en Pop Star,
lo sigue en el espacio hasta que pasa por arriba de la estacion, se queda quieta viendolo alejarse con su estela de humo
y luego da una vuelta en circulo alrededor de la pista, la estacion y el satelite. Al regresar a la libre, aparece donde
estaba la camara anterior.

En los planetas el sol da una vuelta cada 2 minutos: de dia se ve el sol y el skybox de dia, y de noche
la luna, el skybox de noche y las luminarias prendidas. En el espacio el sol esta fijo y no hay noche.
La consola avisa cuando se prende o apaga algo con P, F, L, K y N, y cual es el avatar al presionar la T.

---

## Mapa del proyecto

```
Proyecto_Final_CG_Lab/
|-- Proyecto_Final.slnx        <- esto es lo que se abre con VS
|-- Proyecto_Final.vcxproj     <- configuracion del proyecto (rutas, librerias, etc.)
|-- README.md                  <- este archivo
|
|-- src/                       <- todo el codigo C++
|   |-- main.cpp               <- LA ESCENA: aqui se carga y se dibuja todo
|   |-- DatosEscena.h          <- arreglos largos de la escena (piezas, puntos de la pista, lugares)
|   |-- Window.h/.cpp          <- ventana, teclado y mouse (aqui se agregan teclas)
|   |-- Camera.h/.cpp          <- camara en primera persona
|   |-- Shader_light.h/.cpp    <- compila los shaders y les manda datos (luces, matrices)
|   |-- CommonValues.h         <- maximo de luces puntuales y spotlights
|   |-- Mesh_tn.h/.cpp         <- malla (VAO/VBO/IBO) con textura y normales
|   |-- Texture.h/.cpp         <- carga de imagenes como textura
|   |-- Model.h/.cpp           <- carga de modelos (.obj, etc.) con Assimp
|   |-- Skybox.h/.cpp          <- skybox (cubemap)
|   |-- Light.h/.cpp           <- clase base de las luces
|   |-- DirectionalLight.h/.cpp<- luz tipo sol
|   |-- PointLight.h/.cpp      <- luz tipo foco
|   |-- SpotLight.h/.cpp       <- luz tipo linterna (cono)
|   `-- Material.h/.cpp        <- brillo especular de los objetos
|
|-- shaders/
|   |-- shader_light.vert/.frag<- shader de todos los objetos (Phong con texturas y offset de UV para moverlas)
|   `-- skybox.vert/.frag      <- shader del skybox
|
|-- Models/                    <- un modelo por carpeta (.obj + .mtl), con sus texturas en su carpeta Textures/
|-- Textures/                  <- imagenes de los pisos, de los skybox y de los objetos hechos a mano
|   |-- plain.png              <- textura blanca, se usa si un modelo no tiene textura (no borrar)
|   `-- Skybox/                <- las 6 caras del skybox
|
|-- Librerias/   (NO esta en GitHub, viene en Librerias.zip)
|   |-- glm/                   <- matematicas (vectores, matrices)
|   |-- include/               <- headers de GLEW, GLFW, Assimp y stb_image
|   |-- lib/                   <- .lib para el linker
|   `-- dll/                   <- DLLs, se copian solas a build/ al compilar
|
`-- build/       (NO se sube, lo genera VS al compilar: el .exe y archivos temporales)
```

En el Explorador de soluciones de VS los archivos estan agrupados en carpetas virtuales:
**Escena** (main.cpp y DatosEscena.h), **Base**, **Graficos**, **Iluminacion** y **Shaders**.

### Como esta acomodado main.cpp

Esta dividido en 4 bloques marcados con `// ==================`:

1. **Variables globales**: aqui se declaran texturas, modelos, materiales y luces.
2. **Geometria hecha a mano**: piso y plano de vegetacion (`CreateObjects`) y `DibujaJerarquia`.
3. **Carga de recursos** (dentro de `main`): se cargan texturas, modelos, skybox, materiales y luces.
4. **Ciclo principal** (el `while`): se dibuja todo cada frame. Primero los objetos opacos y al final los transparentes.

Al principio de cada bloque esta lo de la base del lab (ventana, camara, skybox del espacio, materiales,
luces). Despues viene una seccion por actividad, siempre en el mismo orden:

```
//--- personajes ---
//--- planetas: Xen y planeta 2 ---
//--- planeta 3: Verdania ---
//--- nave de Among Us y crewmates ---
//--- pista de carreras y naves ---
//--- satelites y cuerpos espaciales ---
//--- plataforma de lanzamiento y cohete ---
//--- estacion espacial y naves ---
//--- avatar ---
//--- camaras y recorridos ---
//--- iluminacion: ciclo dia/noche, luminarias y spotlights ---
```

Dentro de cada seccion, todo lo de un mismo objeto va junto: su modelo, su textura y las variables de su
animacion. Los arreglos largos que casi no cambian (piezas, padres y pivotes de los modelos con jerarquia,
puntos de la pista, lugares de los objetos que se repiten y datos de las naves de la estacion) estan en
`src/DatosEscena.h`, con las mismas secciones. Solo se incluye en `main.cpp`.

Las mismas secciones estan en `Window.h` (getters y variables) y en `Window.cpp` (valores iniciales en
los dos constructores y teclas en `ManejaTeclado`). **Cada quien escribe solo dentro de las secciones de
su actividad**; asi, al juntar las ramas, cada quien cambio lineas distintas y Git no marca conflictos.

- En el `while`, la seccion de camaras va antes de dibujar: ahi se puede cambiar la matriz `view`
  (con la camara libre es la de `Camera`). `mainWindow.getmodoCamara()` dice que camara se esta usando,
  y `posVista` y `dirVista` tienen la posicion y la direccion de esa camara; usarlas en lugar de
  `camera.getCameraPosition()`, que solo es la de la camara libre.
- La seccion de iluminacion del `while` va antes de mandar las luces al shader: ahi se cambian
  cada frame (ciclo dia/noche, prender/apagar).
- Las luces se crean solo en la seccion de iluminacion. Si otra actividad necesita una, se le pide
  a quien lleva esa seccion. Cada frame solo se mandan al shader las luces de la zona donde se esta,
  asi que el limite (5 puntuales y 5 spotlights) es por zona y no para toda la escena.
- Si una actividad agrega geometria hecha a mano, su funcion va en el bloque 2 (en su seccion) y se
  llama en la carga de recursos (en su seccion). Como el indice en `meshListModel` depende del
  orden, se guarda en una variable global antes del `push_back`:
  ```cpp
  int idxCohete;                          //variables globales, en su seccion
  idxCohete = meshListModel.size();       //en la funcion, justo antes de meshListModel.push_back(cohete);
  meshListModel[idxCohete]->RenderMeshModel();  //en el while
  ```

### Teclas por actividad

Para que dos personas no usen la misma tecla. Si alguien necesita otra, la pide en el grupo y se
agrega a esta tabla.

| Teclas | Actividad | Uso |
|---|---|---|
| W A S D, mouse, Espacio, Ctrl, Shift | base y avatar | moverse con la camara libre o caminar con el avatar |
| T | avatar | cambiar el avatar entre Kirby y la carnivora |
| ESC | base | cerrar |
| P | base | pausar o continuar todas las animaciones |
| 1 2 3 4 5, Q / E | camaras y recorridos | cambiar de camara; Q y E cambian la nave que se sigue |
| Z X C V | planetas | ir al espacio, a Xen, al planeta 2 y a Verdania |
| F | todas, segun la escena | animacion basica de la escena: headcrab en Xen, despegue del cohete en el planeta 2 y plantas en Verdania |
| K L N | iluminacion | L la linterna, K los spotlights del escenario, N dia o noche en los planetas |
| G H J B M R I O Y U | libres | se piden en el grupo |

- La **F** es la tecla de las animaciones basicas (las que se inician con teclado). Cada actividad
  revisa en `ManejaTeclado` que se este en su zona (0 espacio, 1 Xen, 2 planeta 2, 3 Verdania),
  como el headcrab:
  ```cpp
  if (key == GLFW_KEY_F && action == GLFW_PRESS && theWindow->zonaPlaneta == 1)
  {
      theWindow->caminaHeadcrab = !theWindow->caminaHeadcrab;
  }
  ```
  Las animaciones continuas (las de ciclo con funciones) no llevan tecla, se mueven desde el inicio.
- La **P** pausa todo poniendo `deltaTime` en 0, asi que cada animacion tiene que avanzar con
  `deltaTime` (por ejemplo `anguloIslas += anguloIslasOffset * deltaTime;`) para que tambien se pause.

### Reglas importantes

- El programa corre desde la carpeta del proyecto, asi que las rutas son relativas a ella:
  `"Textures/algo.png"`, `"Models/algo.obj"`. Usar `/` en las rutas.
- Las texturas de los modelos se buscan primero en la carpeta `Textures/` del propio modelo
  (ej. `Models/GordonParts/Textures/`) y si no estan ahi, en `Textures/`. Aunque el .mtl diga otra
  ruta, solo se toma el nombre del archivo.
- En este proyecto las normales apuntan **hacia adentro** del objeto (asi lo manejamos en el lab
  por como el shader calcula la luz). Si una cara se ve oscura cuando deberia estar iluminada,
  probablemente su normal esta al reves.
- Los objetos con transparencia se dibujan **al final**, despues de todos los opacos.
- Si se cambia `MAX_POINT_LIGHTS` o `MAX_SPOT_LIGHTS` en `CommonValues.h`, hay que poner
  el mismo numero en `shaders/shader_light.frag`.
- La consola que se abre junto con la ventana muestra los errores (texturas o modelos que no
  se encontraron, errores de shaders, etc.). Revisarla siempre que algo no salga.

---

## Guia: como agregar cosas

### Plantilla para dibujar cualquier objeto

Dentro del `while`, en la parte de objetos opacos (copiar, pegar y cambiar lo necesario):

```cpp
//nombre del objeto
model = glm::mat4(1.0);
model = glm::translate(model, glm::vec3(x, y, z));
model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
glUniform3fv(uniformColor, 1, glm::value_ptr(color));
miTextura.UseTexture();                                                //no va si es un Model
Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
meshListModel[i]->RenderMeshModel();   //o  MiModelo_M.RenderModel();
```

El orden importa: primero translate, luego rotate y al final scale.

### Una textura nueva

1. Poner la imagen en `Textures/`.
2. En variables globales: `Texture pastoTexture;`
3. En la carga de recursos:
   ```cpp
   pastoTexture = Texture("Textures/pasto.png");
   pastoTexture.LoadTextureA();   //png o tga (con alfa). Para jpg usar LoadTexture()
   ```
4. Antes de dibujar el objeto: `pastoTexture.UseTexture();`

Primero se asigna la ruta y despues se carga, en ese orden.

### Un modelo nuevo (.obj)

1. Exportar desde Blender / 3ds Max / Maya como `.obj` con su `.mtl`.
2. Poner el `.obj` y el `.mtl` en su propia carpeta dentro de `Models/` (ej. `Models/arbol/`) y
   **sus texturas en la carpeta `Textures/` de esa carpeta** (ej. `Models/arbol/Textures/`).
3. Revisar en el `.mtl` que la linea `map_Kd` tenga el nombre correcto de la textura
   (la ruta no importa, solo el nombre).
4. En variables globales: `Model Arbol_M;`
5. En la carga de recursos:
   ```cpp
   Arbol_M = Model();
   Arbol_M.LoadModel("Models/arbol/arbol.obj");
   ```
6. En el `while` usar la plantilla y al final `Arbol_M.RenderModel();` (sin `UseTexture`,
   el modelo usa sus propias texturas).

Si sale blanco es que no encontro su textura (se le pone `plain.png`), revisar la consola.
Si sale muy grande o muy chico, ajustar con `glm::scale`.

### Un objeto hecho a mano (geometria propia)

1. Hacer una funcion como `CreateObjects()` con su arreglo de vertices e indices.
   Cada vertice lleva 8 valores: `x, y, z,  u, v,  nx, ny, nz`.
2. Crear la malla y meterla a la lista:
   ```cpp
   MeshModel* obj = new MeshModel();
   obj->CreateMeshModel(vertices, indices, numFloats, numIndices);
   meshListModel.push_back(obj); //meshListModel[2], [3], ...
   ```
   Ojo: el tercer parametro es la **cantidad de floats** del arreglo (vertices x 8),
   no la cantidad de vertices.
3. Si no quieres escribir las normales a mano, se pueden calcular con `calcAverageNormals(...)`
   **antes** de `CreateMeshModel`.
4. Llamar la funcion en la carga de recursos, en la seccion de su actividad, y guardar que indice le toco.

### Un material

```cpp
Material Material_metal;                    //variables globales
Material_metal = Material(2.0f, 64);        //carga de recursos: (intensidad especular, shininess)
Material_metal.UseMaterial(uniformSpecularIntensity, uniformShininess);  //antes de dibujar
```

Ya existen `Material_opaco` (casi sin brillo, lo de la mayoria), `Material_brillante` (reflejo chico y
fuerte, como el visor de los crewmates), `Material_metal` (naves de la carrera) y `Material_avatar`
(Kirby y la carnivora cuando son el avatar). Despues de usar uno distinto, se regresa `Material_opaco` para lo que sigue.

### Una luz

Cada luz es una variable global de la seccion de iluminacion (por ejemplo `luminariaVerdania`), se
crea en la carga de recursos y en el `while` se copia a los arreglos solo si esta prendida y es de la
zona donde se esta:

```cpp
//carga de recursos
luminariaVerdania = PointLight(1.0f, 0.85f, 0.6f,   //color r g b
    0.1f, 3.0f,                                      //intensidad ambiental, difusa
    x, y, z,                                         //posicion
    1.0f, 0.05f, 0.01f);                             //atenuacion: constante, lineal, exponente

//while, seccion de iluminacion
if (zonaLuces == 3 && esDeNoche)
{
    pointLights[pointLightCount] = luminariaVerdania;
    pointLightCount++;
}
```

- Maximo 5 puntuales y 5 spotlights por zona (`CommonValues.h`); la linterna cuenta como un spotlight.
- Las luminarias (puntuales) se prenden de noche y los spotlights con la K.
- Para mover una luz mientras corre el programa: `spot.SetPos(nuevaPos);` o `spot.SetFlash(pos, dir);`
  antes de copiarla al arreglo.
- Para un objeto que brille aunque no le llegue luz (como el sol), se le pone un color arriba de 1.

### Una tecla nueva / mover algo con el teclado

Se hace igual que `despegaCohete` (la F en el planeta 2) o `caminaHeadcrab` (la F en Xen):

1. `Window.h`: agregar la variable en `private` (ej. `GLfloat rotaPuerta;`) y su getter en `public`
   (`GLfloat getrotaPuerta() { return rotaPuerta; }`).
2. `Window.cpp`: darle valor inicial en **los dos** constructores.
3. `Window.cpp`, en `ManejaTeclado`:
   ```cpp
   if (key == GLFW_KEY_R && action == GLFW_PRESS)
   {
       theWindow->rotaPuerta += 10.0f;
   }
   ```
   (`action == GLFW_PRESS` = una vez por pulsacion, `action != GLFW_RELEASE` = mientras se mantenga)
4. En `main.cpp` usarla: `mainWindow.getrotaPuerta()`.

Usar solo las teclas de su actividad (ver "Teclas por actividad"). W, A, S, D son de la camara libre y
del avatar, T cambia el avatar, P pausa y ESC cierra.

### Cambiar el skybox

Poner las 6 imagenes en `Textures/Skybox/` y cambiar las rutas en `main.cpp`.
El orden tiene que ser: **rt, lf, dn, up, bk, ft** (derecha, izquierda, abajo, arriba, atras, enfrente).
Cada planeta tiene un skybox de dia y uno de noche (por ejemplo `skyboxXenDia` y `skyboxXenNoche`); en la
carga de recursos sus caras se arman con `carasSkybox` y el principio del nombre del archivo (`xen_dia_`, `xen_`).
El ultimo parametro de `DrawSkybox` en `true` lo hace girar en los tres ejes (espacio) y en `false` solo
sobre Y (planetas, para que no se incline el horizonte).

### Objetos con transparencia

- La imagen debe ser .png con canal alfa y cargarse con `LoadTextureA()`.
- Se dibujan al final del `while`, en la parte de objetos con transparencia, entre `glEnable(GL_BLEND)`
  y `glDisable(GL_BLEND)`. Para plantas o cosas planas sirve `meshListModel[1]`, que son dos planos cruzados:
  ```cpp
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //aqui va la matriz model como en la plantilla
  plantaTexture.UseTexture();
  Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
  meshListModel[1]->RenderMeshModel();
  glDisable(GL_BLEND);
  ```

### Textura que se mueve (offset de UV)

Como en la practica 9: el vertex shader le suma el uniform `toffset` a las coordenadas de textura. En el `while`
vale 0 para todo; antes de dibujar el objeto se cambia y despues se regresa a 0 (asi se hace el charco de Xen):
```cpp
toffset = glm::vec2(charcoU, charcoV); //charcoU y charcoV avanzan con deltaTime y se les resta 1 al pasar de 1
glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
//aqui se dibuja el objeto
toffset = glm::vec2(0.0f, 0.0f);
glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
```
La imagen tiene que poder repetirse (que la orilla derecha empate con la izquierda) para que no se note el salto.

### Una clase o archivo .cpp nuevo

Crear el archivo dentro de `src/` y agregarlo al proyecto con clic derecho en la carpeta virtual >
Agregar > Elemento existente. Si se usa "Agregar > Clase", cambiar la ubicacion a la carpeta `src`
(VS los pone en la raiz por defecto). Despues hacer commit tambien del `.vcxproj` y el `.vcxproj.filters`.

---

## Trabajo en equipo con Git

Se piden **varias ramas por alumno**, una por cada avance, y que todas se queden en el repo hasta
el final (sirven para ver como fue avanzando cada quien). Por eso:

- Cada quien trabaja **solo en sus ramas** (`nombre-tema`, por ejemplo `alejandro-planetas` o
  `alejandro-avatar-camara`). Nadie hace commit ni push directo a `main`.
- **Cada avance nuevo va en una rama nueva**, que sale de `main` ya actualizado. La rama anterior
  se deja como quedo, ya no se sigue usando:
  ```
  git checkout main
  git pull
  git checkout -b nombre-tema
  git push -u origin nombre-tema
  ```
  En GitHub Desktop: cambiarse a `main`, Fetch origin y Pull, luego Branch > New branch, escribir el
  nombre y Publish branch.
  Al crearla, agregarla a la tabla de ramas de `AVANCES.md`.
- **Nunca borrar ramas**, ni la propia ni las de otros (tampoco con el boton "Delete branch" de
  GitHub despues de un Pull Request).
- **Antes de empezar a trabajar:** cambiarse a la rama del avance en el que se esta trabajando y
  hacer Pull. Al terminar: commit y push a esa rama.
- **Cuando algo ya esta listo** (compila y funciona), avisar en el grupo (o abrir un
  Pull Request de su rama hacia `main`). Se revisan los cambios y se ajustan si hace falta y los integra a `main`.
- **Despues de cada integracion**, cada quien trae `main` a su rama antes de seguir, para trabajar
  sobre la version que ya tiene lo de todos y no regresar cambios que ya se ajustaron:
  ```
  git checkout mi-rama
  git pull
  git merge origin/main
  git push
  ```
  En GitHub Desktop: Fetch origin, luego Branch > Update from main, y Push.
- Commits chicos y con mensaje claro (`agrega modelo de la casa`, `arregla luz de la lampara`).
- `main.cpp` y `Window.h/.cpp` los tocamos todos: cada quien escribe solo en las secciones
  `//--- ... ---` de su actividad (ver "Como esta acomodado main.cpp") y no reacomoda ni re-formatea
  codigo de otros.
- No subir nada de `Librerias/` ni `build/` (ya esta en el `.gitignore`).
- GitHub no acepta archivos de mas de 100 MB. Mantener modelos y texturas ligeros
  (texturas de 1024 o 2048 px estan bien).
- Si alguien agrega una libreria nueva, se actualiza `Librerias.zip` y se le pasa a todos.

---

## Problemas comunes

| Problema | Solucion |
|---|---|
| `Falta la carpeta Librerias` al compilar | Extraer `Librerias.zip` como dice "Como empezar". |
| `No se encontro la textura: ...` en la consola | Revisar nombre y extension exactos y que este en la carpeta `Textures/` del modelo o en `Textures/`. |
| `Fallo al cargar el modelo: ...` | Revisar la ruta en `LoadModel` y que el .obj este en `Models/`. |
| Modelo blanco | No encontro su textura: revisar `map_Kd` del .mtl y que la imagen este en la carpeta `Textures/` del modelo. |
| Una cara se ve negra u oscura | Normal al reves (en este proyecto van hacia adentro) o no le llega ninguna luz. |
| Transparencia con fondo negro/blanco | Usar `LoadTextureA()` y dibujarlo al final con blending. |
| `Error al compilar el ... shader` | La consola dice la linea del error dentro del shader. |
| `LNK1168: cannot open ...exe for writing` | El programa sigue abierto, cerrarlo y volver a compilar. |
| Falta una DLL al correr | Volver a compilar (se copian solas de `Librerias/dll`) o revisar que el zip este completo. |
