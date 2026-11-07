#!/usr/bin/env python3
"""
Genera keyframes_cohete.txt: despegue del cohete con separacion por fases.

Unidades: las del modelo del cohete (armado mide 4.6; en pantalla se escala x4 con escalaPlataforma).
Cada keyframe guarda, por pieza, el DESPLAZAMIENTO de su centro respecto a donde esta armado el cohete
y su GIRO (grados, orden X, Y, Z) alrededor de su propio centro.

Uso:  python3 generar_keyframes_cohete.py  [carpeta_del_repo]
Lee los .obj de Models/Cohete para que ninguna pieza se entierre en el piso.
"""
import math
import os
import sys

REPO = sys.argv[1] if len(sys.argv) > 1 else "."
NOMBRES = ["base", "tanque", "cuerpo", "anillo", "punta"]
# centro de cada pieza armada (sale de las cajas de los .obj)
CENTROS = [(0, 0.8, 0), (0, 1.85, 0), (0, 2.6, 0), (0, 3.45, 0), (0, 4.2, 0)]
SUELO = -0.1  # el cohete se dibuja 0.1 arriba del suelo (las placas de la plataforma miden 0.1)


# ---------- geometria para no enterrar piezas ----------
def carga_vertices(nombre):
    vs = []
    with open(os.path.join(REPO, "Models", "Cohete", "cohete_%s.obj" % nombre)) as f:
        for l in f:
            if l.startswith("v "):
                p = l.split()
                vs.append((float(p[1]), float(p[2]), float(p[3])))
    return vs


VERTS = [carga_vertices(n) for n in NOMBRES]


def rot_mat(rx, ry, rz):
    """R = Rx * Ry * Rz, igual que en el codigo C++ (primero se aplica Rz)."""
    a, b, c = (math.radians(rx), math.radians(ry), math.radians(rz))
    Rx = [[1, 0, 0], [0, math.cos(a), -math.sin(a)], [0, math.sin(a), math.cos(a)]]
    Ry = [[math.cos(b), 0, math.sin(b)], [0, 1, 0], [-math.sin(b), 0, math.cos(b)]]
    Rz = [[math.cos(c), -math.sin(c), 0], [math.sin(c), math.cos(c), 0], [0, 0, 1]]

    def mul(A, B):
        return [[sum(A[i][k] * B[k][j] for k in range(3)) for j in range(3)] for i in range(3)]
    return mul(mul(Rx, Ry), Rz)


def altura_apoyo(pieza, rot):
    """cuanto tiene que estar el centro arriba del suelo para que el punto mas bajo lo toque."""
    R = rot_mat(*rot)
    c = CENTROS[pieza]
    ymin = 1e9
    for v in VERTS[pieza]:
        d = (v[0] - c[0], v[1] - c[1], v[2] - c[2])
        y = R[1][0] * d[0] + R[1][1] * d[1] + R[1][2] * d[2]
        ymin = min(ymin, y)
    return -ymin


# ---------- parametros de la animacion ----------
T_CUENTA = 3.0          # cuenta regresiva en la plataforma
ACEL = 0.7              # aceleracion del cohete (unidades/s^2)
GRAV = 6.0              # gravedad de las piezas que caen
SEPARA = [7.5, 10.0, 12.5, 15.0]   # segundos en que se suelta base, tanque, cuerpo y anillo
# donde cae cada pieza (x, z) respecto al centro de la plataforma; lejos de la torre, de Gordon y de las colinas
DESTINO = [(-7.0, -2.5), (-10.5, -4.5), (-5.0, -5.8), (-13.0, -1.0)]
# giro final de cada pieza al caer (rx, ry, rz): quedan acostadas
GIRO_FINAL = [(0, 40, 450), (0, -25, -450), (0, 70, 450), (0, -60, -450)]
REBOTE = [0.45, 0.30, 0.25, 0.20]  # altura del rebote al tocar el suelo
T_REBOTE = 0.5
T_ESPIRAL = 4.0         # la punta pasa de subir recto a orbitar
T_PREORBITA = 3.0       # orbita antes del ciclo, mientras terminan de caer las otras piezas
RADIO_ORBITA = 16.0
VEL_ORBITA = 45.0       # grados por segundo -> una vuelta cada 8 s
THETA0 = 0.0
T_ESP0 = SEPARA[3]
T_LOOP0 = T_ESP0 + T_ESPIRAL + T_PREORBITA   # 22.0
T_FIN = T_LOOP0 + 360.0 / VEL_ORBITA          # 30.0


def y_pila(t):
    """altura del cohete armado desde el despegue."""
    if t <= T_CUENTA:
        return 0.0
    return 0.5 * ACEL * (t - T_CUENTA) ** 2


def v_pila(t):
    return 0.0 if t <= T_CUENTA else ACEL * (t - T_CUENTA)


def suave(s):
    s = max(0.0, min(1.0, s))
    return s * s * (3 - 2 * s)


# ---------- caida de una pieza ----------
class Caida:
    def __init__(self, i):
        self.i = i
        self.ts = SEPARA[i]
        self.y0 = CENTROS[i][1] + y_pila(self.ts)
        self.v0 = v_pila(self.ts)
        self.rot_f = GIRO_FINAL[i]
        self.y_rest = SUELO + altura_apoyo(i, self.rot_f)
        # y0 + v0 t - g t^2 / 2 = y_rest
        a, b, c = -0.5 * GRAV, self.v0, self.y0 - self.y_rest
        self.tvuelo = (-b - math.sqrt(b * b - 4 * a * c)) / (2 * a)
        self.t_suelo = self.ts + self.tvuelo
        self.dest = DESTINO[i]

    def estado(self, t):
        """devuelve (centro absoluto x, y, z) y (rx, ry, rz) de la pieza."""
        if t <= self.t_suelo:
            tau = t - self.ts
            f = tau / self.tvuelo
            y = self.y0 + self.v0 * tau - 0.5 * GRAV * tau * tau
            rot = tuple(self.rot_f[k] * f for k in range(3))
            # nunca por debajo del piso con la orientacion que lleva en ese momento
            y = max(y, SUELO + altura_apoyo(self.i, rot))
            return (self.dest[0] * f, y, self.dest[1] * f), rot
        u = min(1.0, (t - self.t_suelo) / T_REBOTE)
        y = self.y_rest + 4 * REBOTE[self.i] * u * (1 - u)
        desliza = 0.35 * suave(u)
        d = math.hypot(*self.dest)
        x = self.dest[0] + desliza * self.dest[0] / d
        z = self.dest[1] + desliza * self.dest[1] / d
        return (x, y, z), self.rot_f


CAIDAS = [Caida(i) for i in range(4)]


# ---------- la punta: sube, hace espiral y orbita ----------
def punta(t):
    """centro y giro de la capsula."""
    if t <= T_ESP0:
        return (0.0, CENTROS[4][1] + y_pila(t), 0.0), (0.0, 0.0, 0.0)
    s = (t - T_ESP0) / T_ESPIRAL
    s_c = min(1.0, s)
    y0 = CENTROS[4][1] + y_pila(T_ESP0)
    v0 = v_pila(T_ESP0)
    y = y0 + v0 * T_ESPIRAL * (1 - (1 - s_c) ** 3) / 3.0
    r = RADIO_ORBITA * suave(s_c)
    th = THETA0 + VEL_ORBITA * (t - T_ESP0)
    b = suave(s_c)  # mezcla de vertical a acostada con la punta hacia donde va
    rz = -90.0 * b
    ry = (-th - 90.0) * b
    c = (r * math.cos(math.radians(th)), y, r * math.sin(math.radians(th)))
    return c, (0.0, ry, rz)


# ---------- fuego ----------
def fuego(t):
    """(tamano 0..1, pieza que lleva la llama)."""
    if t < T_CUENTA:
        return 0.5 * t / T_CUENTA, 0
    etapas = [T_CUENTA] + SEPARA
    fin_motor = T_ESP0 + T_ESPIRAL  # la punta apaga su motor al entrar en orbita
    for k in range(5):
        ini = etapas[k]
        fin = etapas[k + 1] if k < 4 else fin_motor
        if t < ini or (k < 4 and t >= fin):
            continue
        # la llama de cada etapa se prende de golpe a 0.25 (0.5 la primera) y crece a 1 en medio segundo
        f_ini = 0.5 if k == 0 else 0.25
        f = f_ini + (1.0 - f_ini) * min(1.0, (t - ini) / 0.5)
        if k < 4:
            # un instante antes de soltarse esa pieza la llama se apaga un poco
            if fin - t < 0.25:
                f = min(f, 1.0 - 0.75 * (0.25 - (fin - t)) / 0.25)
        else:
            # la punta apaga su motor poco a poco
            f *= max(0.0, 1.0 - (t - ini) / (fin - ini))
        return f, k
    return 0.0, 4


# ---------- armado de keyframes ----------
def keyframe(t):
    f, pf = fuego(t)
    pos = []
    rot = []
    # sacudida en la plataforma
    jx = jz = 0.0
    if t < T_CUENTA:
        k = int(round(t / 0.125))
        amp = 0.06 * (t / T_CUENTA)
        jx = amp * (1 if k % 2 == 0 else -1)
        jz = -0.6 * amp * (1 if k % 2 == 0 else -1)
    for i in range(5):
        if i < 4 and t >= SEPARA[i]:
            c, r = CAIDAS[i].estado(t)
        elif i == 4:
            c, r = punta(t)
            if t <= T_ESP0:
                c = (c[0] + jx, c[1], c[2] + jz)
        else:
            c = (jx, CENTROS[i][1] + y_pila(t), jz)
            r = (0.0, 0.0, 0.0)
        pos.append((c[0] - CENTROS[i][0], c[1] - CENTROS[i][1], c[2] - CENTROS[i][2]))
        rot.append(r)
    return f, pf, pos, rot


def tiempos():
    ts = set()
    n = 0
    while n * 0.125 < T_CUENTA - 1e-9:
        ts.add(round(n * 0.125, 3))
        n += 1
    n = 0
    while T_CUENTA + n * 0.25 <= T_FIN + 1e-9:
        ts.add(round(T_CUENTA + n * 0.25, 3))
        n += 1
    for c in CAIDAS:
        ts.add(round(c.t_suelo, 3))
        ts.add(round(c.t_suelo + T_REBOTE, 3))
    return sorted(ts)


def escribe(ruta):
    ts = tiempos()
    idx_loop = ts.index(round(T_LOOP0, 3))
    with open(ruta, "w", newline="\n") as f:
        f.write("# Keyframes del cohete - Ejercicio 1: despegue, separacion por fases y capsula en orbita\n")
        f.write("# Proyecto Final Computacion Grafica, planeta 2 (Pop Star), plataforma de lanzamiento\n")
        f.write("#\n")
        f.write("# Unidades del modelo del cohete (armado mide 4.6; en pantalla se escala x4).\n")
        f.write("# Piezas: 0 base, 1 tanque, 2 cuerpo, 3 anillo, 4 punta (capsula).\n")
        f.write("# Cada linea FRAME es un keyframe:\n")
        f.write("#   FRAME tiempo(s) fuego piezaFuego | por cada pieza: posX posY posZ rotX rotY rotZ\n")
        f.write("#   fuego       = tamano de la llama (0 apagada, 1 completa), se interpola\n")
        f.write("#   piezaFuego  = pieza que lleva la llama en ese keyframe (no se interpola)\n")
        f.write("#   pos         = desplazamiento del centro de la pieza respecto a donde esta armada\n")
        f.write("#   rot         = giro en grados alrededor del centro de la pieza (orden X, Y, Z)\n")
        f.write("# Entre keyframes se interpola linealmente.\n")
        f.write("# LOOP_DESDE n = al llegar al ultimo keyframe se regresa al keyframe n (la capsula sigue orbitando).\n")
        f.write("#\n")
        f.write("# Fases: 0-3 s cuenta y vibracion | 3 s despegue | %.1f s se suelta la base | %.1f s el tanque |\n"
                % (SEPARA[0], SEPARA[1]))
        f.write("#        %.1f s el cuerpo | %.1f s el anillo, la capsula hace espiral y entra en orbita (ciclo desde %.1f s)\n"
                % (SEPARA[2], SEPARA[3], T_LOOP0))
        f.write("LOOP_DESDE %d\n" % idx_loop)
        for n, t in enumerate(ts):
            fu, pf, pos, rot = keyframe(t)
            partes = ["FRAME %.3f %.3f %d" % (t, fu, pf)]
            for i in range(5):
                partes.append("| %.3f %.3f %.3f %.2f %.2f %.2f" % tuple(v + 0.0 if abs(v) > 5e-4 else 0.0 for v in (pos[i] + rot[i])))
            f.write(" ".join(partes) + "\n")
    return ts, idx_loop


if __name__ == "__main__":
    salida = os.path.join(REPO, "keyframes_cohete.txt")
    ts, idx = escribe(salida)
    print("escritos %d keyframes en %s (LOOP_DESDE %d)" % (len(ts), salida, idx))
    for c in CAIDAS:
        print("  %-7s se suelta a %.2f s, toca el suelo a %.2f s en (%.1f, %.1f)"
              % (NOMBRES[c.i], c.ts, c.t_suelo, c.dest[0], c.dest[1]))
