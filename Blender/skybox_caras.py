#saca las 6 caras del skybox de un panorama equirectangular hecho en blender (xen.py)
#uso: python skybox_caras.py <panorama.png> <carpeta de salida> <prefijo> [tamano]
#las caras salen como las lee Skybox.cpp: orden rt, lf, dn, up, bk, ft en GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
#imagenes volteadas por stb y el shader lee el cubemap con -pos (direccion al reves)
import sys, os
import numpy as np
import matplotlib.image as mi

NOMBRES = ["rt", "lf", "dn", "up", "bk", "ft"]   #+X, -X, +Y, -Y, +Z, -Z del cubemap


def lee_pano(pano, d):
    #d: direcciones en coordenadas de opengl (Y arriba). En blender: (x, -z, y)
    bx, by, bz = d[..., 0], -d[..., 2], d[..., 1]
    n = np.sqrt(bx * bx + by * by + bz * bz)
    bx, by, bz = bx / n, by / n, bz / n
    h, w = pano.shape[:2]
    u = 0.5 + np.arctan2(bx, by) / (2 * np.pi)
    v = 0.5 + np.arcsin(np.clip(bz, -1, 1)) / np.pi
    x = u * w - 0.5
    y = (1.0 - v) * h - 0.5
    x0 = np.floor(x).astype(int)
    y0 = np.floor(y).astype(int)
    fx = (x - x0)[..., None]
    fy = (y - y0)[..., None]
    x1 = (x0 + 1) % w
    x0 = x0 % w
    y1 = np.clip(y0 + 1, 0, h - 1)
    y0 = np.clip(y0, 0, h - 1)
    a = pano[y0, x0] * (1 - fx) + pano[y0, x1] * fx
    b = pano[y1, x0] * (1 - fx) + pano[y1, x1] * fx
    return a * (1 - fy) + b * fy


def dir_cubo(cara, sc, tc):
    #tabla de la especificacion de opengl: de (sc, tc) de cada cara a la direccion c del cubemap
    uno = np.ones_like(sc)
    if cara == 0:
        c = (uno, -tc, -sc)
    elif cara == 1:
        c = (-uno, -tc, sc)
    elif cara == 2:
        c = (sc, uno, tc)
    elif cara == 3:
        c = (sc, -uno, -tc)
    elif cara == 4:
        c = (sc, -tc, uno)
    else:
        c = (-sc, -tc, -uno)
    return np.stack(c, axis=-1)


def caras(pano, n):
    res = []
    idx = (np.arange(n) + 0.5) / n
    xs, ys = np.meshgrid(idx, idx)          #ys: fila de la imagen desde arriba
    sc = 2 * xs - 1
    tc = 2 * (1 - ys) - 1                   #stb voltea la imagen: la fila de arriba del archivo es t = 1
    for cara in range(6):
        c = dir_cubo(cara, sc, tc)
        res.append(lee_pano(pano, -c))      #el shader lee el cubemap con -pos
    return res


def prueba(pano, imgs, n):
    #direcciones al azar: se busca la cara y el texel como lo haria opengl y se compara con el panorama
    rng = np.random.default_rng(1)
    g = rng.normal(size=(2000, 3))
    c = -g
    errs = []
    for v in c:
        ax = np.argmax(np.abs(v))
        if ax == 0:
            cara, sc, tc, ma = (0, -v[2], -v[1], v[0]) if v[0] > 0 else (1, v[2], -v[1], v[0])
        elif ax == 1:
            cara, sc, tc, ma = (2, v[0], v[2], v[1]) if v[1] > 0 else (3, v[0], -v[2], v[1])
        else:
            cara, sc, tc, ma = (4, v[0], -v[1], v[2]) if v[2] > 0 else (5, -v[0], -v[1], v[2])
        s = (sc / abs(ma) + 1) / 2
        t = (tc / abs(ma) + 1) / 2
        col = min(int(s * n), n - 1)
        fila = min(int((1 - t) * n), n - 1)
        a = imgs[cara][fila, col]
        b = lee_pano(pano, -v[None, :])[0]
        errs.append(np.abs(a - b).mean())
    #si la tabla estuviera mal el error promedio seria grande (del orden de 0.1); con el redondeo al texel queda muy chico
    return float(np.mean(errs))


if __name__ == "__main__":
    pano = mi.imread(sys.argv[1])[..., :3].astype(np.float64)
    if pano.max() > 1.5:
        pano = pano / 255.0
    out = sys.argv[2]
    pref = sys.argv[3]
    n = int(sys.argv[4]) if len(sys.argv) > 4 else 1024
    os.makedirs(out, exist_ok=True)
    imgs = caras(pano, n)
    print("error promedio de la prueba:", prueba(pano, imgs, n))
    for nombre, im in zip(NOMBRES, imgs):
        ruta = os.path.join(out, pref + "_" + nombre + ".png")
        mi.imsave(ruta, np.clip(im, 0, 1))
        print(ruta)
