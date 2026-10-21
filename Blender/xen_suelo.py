#textura del piso de xen que se puede repetir (la orilla derecha empata con la izquierda y la de arriba con la de abajo)
#uso: python xen_suelo.py <salida.png> [tamano]
#se usa ruido con una rejilla que se repite: el valor en x = tamano es el mismo que en x = 0
import sys
import numpy as np
import matplotlib.image as mi

N = int(sys.argv[2]) if len(sys.argv) > 2 else 1024
rng = np.random.default_rng(11)


def ruido(celdas):
    #ruido de valores en una rejilla de celdas x celdas que se repite, interpolado suave
    rej = rng.random((celdas, celdas))
    t = np.arange(N) * celdas / N
    i0 = np.floor(t).astype(int)
    f = t - i0
    f = f * f * (3 - 2 * f)
    i1 = (i0 + 1) % celdas
    a = rej[i0][:, i0] * (1 - f)[None, :] + rej[i0][:, i1] * f[None, :]
    b = rej[i1][:, i0] * (1 - f)[None, :] + rej[i1][:, i1] * f[None, :]
    return a * (1 - f)[:, None] + b * f[:, None]


def fbm(base, octavas):
    total, amp, suma = 0, 1.0, 0
    for k in range(octavas):
        total = total + ruido(base * 2 ** k) * amp
        suma += amp
        amp *= 0.5
    return total / suma


def mezcla(a, b, t):
    return a * (1 - t[..., None]) + b * t[..., None]


def suave(x, a, b):
    t = np.clip((x - a) / (b - a), 0, 1)
    return t * t * (3 - 2 * t)


roca = fbm(4, 7)
manchas = fbm(2, 5)
grietas = np.abs(fbm(8, 4) - 0.5)          #cerca de 0.5 del ruido salen lineas
oscuro = np.array([0.16, 0.13, 0.08])
claro = np.array([0.45, 0.40, 0.26])
verde = np.array([0.14, 0.32, 0.25])
col = mezcla(oscuro, claro, suave(roca, 0.3, 0.7))
col = mezcla(col, verde * (0.6 + roca[..., None] * 0.8), suave(manchas, 0.52, 0.65))
col = col * (1 - 0.6 * (1 - suave(grietas, 0.0, 0.025)))[..., None]
grano = fbm(64, 3)                         #detalle fino para que de cerca no se vea borroso
col = col * (0.75 + 0.5 * grano)[..., None]
mi.imsave(sys.argv[1], np.clip(col, 0, 1))
print("textura del piso:", sys.argv[1], N, "px")
