// Gordon Freeman: partes articuladas.
// Todas las piezas (.obj) comparten el MISMO sistema de coordenadas del modelo completo:
//   - Y hacia arriba, Gordon mira hacia +Z, escala en metros (mide ~1.4 de alto).
//   - "der" / "izq" son la derecha e izquierda DE GORDON (la derecha esta en X negativa).
// Con todos los angulos en 0 las piezas se dibujan en su lugar, sin traslaciones extra.
//
// Para girar una pieza alrededor de su articulacion:
//   model = matrizDelPadre;
//   model = glm::translate(model,  PIVOTE_X);   // origen en la articulacion
//   model = glm::rotate(model, angulo, eje);    // girar
//   model = glm::translate(model, -PIVOTE_X);   // regresar
//   -> dibujar la pieza. Esa matriz es la "matriz del padre" de la siguiente pieza.
#pragma once
#include <glm.hpp>

// ---- Pivotes (posicion de cada articulacion en el modelo) ----
const glm::vec3 PIVOTE_CADERA       = glm::vec3( 0.00000f, 0.73277f,  0.01014f); // raiz (torso)
const glm::vec3 PIVOTE_CABEZA       = glm::vec3( 0.00000f, 1.22766f, -0.01482f); // cuello

const glm::vec3 PIVOTE_HOMBRO_DER   = glm::vec3(-0.14675f, 1.11121f, -0.03717f);
const glm::vec3 PIVOTE_CODO_DER     = glm::vec3(-0.29002f, 0.94174f, -0.04760f);
const glm::vec3 PIVOTE_MUNECA_DER   = glm::vec3(-0.43085f, 0.77516f, -0.04468f);

const glm::vec3 PIVOTE_HOMBRO_IZQ   = glm::vec3( 0.14675f, 1.11121f, -0.03717f);
const glm::vec3 PIVOTE_CODO_IZQ     = glm::vec3( 0.29002f, 0.94174f, -0.04760f);
const glm::vec3 PIVOTE_MUNECA_IZQ   = glm::vec3( 0.43085f, 0.77516f, -0.04469f);

const glm::vec3 PIVOTE_CADERA_DER   = glm::vec3(-0.07392f, 0.73277f,  0.01014f); // muslo derecho
const glm::vec3 PIVOTE_RODILLA_DER  = glm::vec3(-0.07569f, 0.39412f, -0.00743f);
const glm::vec3 PIVOTE_TOBILLO_DER  = glm::vec3(-0.07732f, 0.08130f, -0.03441f);

const glm::vec3 PIVOTE_CADERA_IZQ   = glm::vec3( 0.07392f, 0.73277f,  0.01014f); // muslo izquierdo
const glm::vec3 PIVOTE_RODILLA_IZQ  = glm::vec3( 0.07569f, 0.39412f, -0.00743f);
const glm::vec3 PIVOTE_TOBILLO_IZQ  = glm::vec3( 0.07732f, 0.08130f, -0.03441f);

// ---- Jerarquia ----
//   torso (raiz)
//    +- cabeza
//    +- brazo_der -> antebrazo_der -> mano_der
//    +- brazo_izq -> antebrazo_izq -> mano_izq
//    +- muslo_der -> pantorrilla_der -> pie_der
//    +- muslo_izq -> pantorrilla_izq -> pie_izq
//
// Archivo por pieza: gordon_<pieza>.obj  (ej. gordon_antebrazo_izq.obj)
