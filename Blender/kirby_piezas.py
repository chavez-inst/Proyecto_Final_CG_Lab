#separa Models/Kirby/kirby.obj en las 5 piezas que usa main.cpp (cuerpo, manos y pies)
#el obj original viene en centimetros y acostado (Z hacia abajo, Y hacia el frente); aqui se gira 90 grados en X
#para que quede con Y arriba, mirando hacia +Z, los pies en y = 0 y una altura de ~0.73
#las manos y los pies vienen juntos en un grupo; se separan por el lado (X negativa = derecha de Kirby)
#uso: python kirby_piezas.py <carpeta de Kirby>
import sys, os

carpeta = sys.argv[1]
ESCALA = 0.004

vs, vts, vns = [], [], []
grupos = {}
grupo = None
mtl = None
for linea in open(os.path.join(carpeta, "kirby.obj")):
    p = linea.split()
    if not p:
        continue
    if p[0] == "v":
        vs.append([float(t) for t in p[1:4]])
    elif p[0] == "vt":
        vts.append(p[1:3])
    elif p[0] == "vn":
        vns.append([float(t) for t in p[1:4]])
    elif p[0] == "g":
        grupo = p[1].split("_")[0].lower()
    elif p[0] == "usemtl":
        mtl = p[1]
    elif p[0] == "f":
        grupos.setdefault(grupo, []).append(p[1:])

zmax = max(v[2] for v in vs)
cuerpo_y = [vs[int(c.split("/")[0]) - 1][1] for f in grupos["body"] for c in f]
ycentro = (min(cuerpo_y) + max(cuerpo_y)) / 2


def gira(v):
    #(x, y, z) -> (x, zmax - z, y - ycentro): giro de 90 grados en X, pies en 0 y cuerpo centrado
    return [v[0] * ESCALA, (zmax - v[2]) * ESCALA, (v[1] - ycentro) * ESCALA]


def gira_normal(n):
    return [n[0], -n[2], n[1]]


def lado(f):
    return sum(vs[int(c.split("/")[0]) - 1][0] for c in f) / len(f)


piezas = {"cuerpo": grupos["body"],
          "mano_der": [f for f in grupos["hands"] if lado(f) < 0],
          "mano_izq": [f for f in grupos["hands"] if lado(f) >= 0],
          "pie_der": [f for f in grupos["feets"] if lado(f) < 0],
          "pie_izq": [f for f in grupos["feets"] if lado(f) >= 0]}

for nombre, caras in piezas.items():
    nv, nt, nn = {}, {}, {}
    sal_v, sal_t, sal_n, sal_f = [], [], [], []
    for f in caras:
        nueva = []
        for c in f:
            partes = c.split("/")
            iv = int(partes[0])
            if iv not in nv:
                nv[iv] = len(nv) + 1
                sal_v.append(gira(vs[iv - 1]))
            txt = str(nv[iv])
            it = int(partes[1]) if len(partes) > 1 and partes[1] else None
            inn = int(partes[2]) if len(partes) > 2 and partes[2] else None
            if it is not None and it not in nt:
                nt[it] = len(nt) + 1
                sal_t.append(vts[it - 1])
            if inn is not None and inn not in nn:
                nn[inn] = len(nn) + 1
                sal_n.append(gira_normal(vns[inn - 1]))
            txt += "/" + (str(nt[it]) if it is not None else "")
            if inn is not None:
                txt += "/" + str(nn[inn])
            nueva.append(txt)
        sal_f.append(nueva)
    with open(os.path.join(carpeta, "kirby_" + nombre + ".obj"), "w", newline="\n") as s:
        s.write("mtllib kirby.mtl\n")
        s.write("o kirby_" + nombre + "\n")
        for v in sal_v:
            s.write("v %.6f %.6f %.6f\n" % tuple(v))
        for t in sal_t:
            s.write("vt %s %s\n" % (t[0], t[1]))
        for n in sal_n:
            s.write("vn %.6f %.6f %.6f\n" % tuple(n))
        s.write("usemtl " + mtl + "\n")
        for f in sal_f:
            s.write("f " + " ".join(f) + "\n")
    ys = [v[1] for v in sal_v]
    xs = [v[0] for v in sal_v]
    zs = [v[2] for v in sal_v]
    print("%-9s caras %5d  x %.3f..%.3f  y %.3f..%.3f  z %.3f..%.3f" % (nombre, len(sal_f), min(xs), max(xs), min(ys), max(ys), min(zs), max(zs)))
