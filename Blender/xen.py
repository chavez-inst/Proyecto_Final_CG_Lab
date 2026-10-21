#cielo de xen y textura del planeta, hechos con nodos procedurales de blender
#uso: blender -b --factory-startup -P xen.py -- <carpeta de salida>
#salen dos panoramas equirectangulares (xen_cielo_pano.png y xen_planeta.png) y la esfera del planeta (.obj)
#las 6 caras del skybox se sacan del panorama con skybox_caras.py
import bpy, math, sys, os

args = sys.argv[sys.argv.index("--") + 1:]
OUT = args[0]
os.makedirs(OUT, exist_ok=True)

bpy.ops.wm.read_factory_settings(use_empty=True)
scn = bpy.context.scene

def nodo(nt, tipo, x, y):
    n = nt.nodes.new(tipo)
    n.location = (x * 250, y * 250)
    return n

def rampa(nt, x, y, colores):
    r = nodo(nt, "ShaderNodeValToRGB", x, y)
    els = r.color_ramp.elements
    while len(els) < len(colores):
        els.new(0.5)
    for e, (pos, c) in zip(els, colores):
        e.position = pos
        e.color = (c[0], c[1], c[2], 1.0)
    return r

def noise(nt, vec, x, y, escala, detalle, rug, dist=0.0):
    n = nodo(nt, "ShaderNodeTexNoise", x, y)
    n.inputs["Scale"].default_value = escala
    n.inputs["Detail"].default_value = detalle
    n.inputs["Roughness"].default_value = rug
    n.inputs["Distortion"].default_value = dist
    nt.links.new(vec, n.inputs["Vector"])
    return n

def mezcla(nt, x, y, fac, a, b, modo="MIX"):
    m = nodo(nt, "ShaderNodeMix", x, y)
    m.data_type = "RGBA"
    m.blend_type = modo
    if isinstance(fac, float):
        m.inputs[0].default_value = fac
    else:
        nt.links.new(fac, m.inputs[0])
    nt.links.new(a, m.inputs[6])
    nt.links.new(b, m.inputs[7])
    return m.outputs[2]

def estrellas(nt, vec, x, y, escala, umbral):
    v = nodo(nt, "ShaderNodeTexVoronoi", x, y)
    v.inputs["Scale"].default_value = escala
    v.inputs["Randomness"].default_value = 1.0
    nt.links.new(vec, v.inputs["Vector"])
    r = nodo(nt, "ShaderNodeMapRange", x + 1, y)
    r.inputs["From Min"].default_value = umbral
    r.inputs["From Max"].default_value = 0.0
    nt.links.new(v.outputs["Distance"], r.inputs["Value"])
    pot = nodo(nt, "ShaderNodeMath", x + 2, y)
    pot.operation = "POWER"
    pot.inputs[1].default_value = 3.0
    nt.links.new(r.outputs[0], pot.inputs[0])
    #cada estrella con brillo distinto
    sep = nodo(nt, "ShaderNodeSeparateColor", x + 1, y - 1)
    nt.links.new(v.outputs["Color"], sep.inputs[0])
    mul = nodo(nt, "ShaderNodeMath", x + 3, y)
    mul.operation = "MULTIPLY"
    nt.links.new(pot.outputs[0], mul.inputs[0])
    nt.links.new(sep.outputs[0], mul.inputs[1])
    return mul.outputs[0]

def mundo_cielo():
    #espacio oscuro con nebulosas verdes y moradas como el cielo de xen
    w = bpy.data.worlds.new("cielo")
    w.use_nodes = True
    nt = w.node_tree
    for n in list(nt.nodes):
        nt.nodes.remove(n)
    out = nodo(nt, "ShaderNodeOutputWorld", 12, 0)
    bg = nodo(nt, "ShaderNodeBackground", 11, 0)
    nt.links.new(bg.outputs[0], out.inputs[0])
    tc = nodo(nt, "ShaderNodeTexCoord", -1, 0)
    nor = nodo(nt, "ShaderNodeVectorMath", 0, 0)
    nor.operation = "NORMALIZE"
    nt.links.new(tc.outputs["Generated"], nor.inputs[0])
    vec = nor.outputs[0]

    n1 = noise(nt, vec, 1, 2, 1.2, 5.0, 0.55, 0.35)
    neb1 = rampa(nt, 2, 2, [(0.52, (0.0, 0.0, 0.0)), (0.64, (0.03, 0.22, 0.13)), (0.76, (0.30, 0.80, 0.42))])
    nt.links.new(n1.outputs["Fac"], neb1.inputs[0])
    n2 = noise(nt, vec, 1, 0, 1.8, 5.0, 0.55, 0.5)
    neb2 = rampa(nt, 2, 0, [(0.55, (0.0, 0.0, 0.0)), (0.67, (0.16, 0.03, 0.24)), (0.80, (0.55, 0.25, 0.70))])
    nt.links.new(n2.outputs["Fac"], neb2.inputs[0])
    n3 = noise(nt, vec, 1, -2, 6.0, 10.0, 0.7, 0.5)
    polvo = rampa(nt, 2, -2, [(0.35, (0.0, 0.0, 0.0)), (0.7, (1.0, 1.0, 1.0))])
    nt.links.new(n3.outputs["Fac"], polvo.inputs[0])

    base = nodo(nt, "ShaderNodeRGB", 3, 4)
    base.outputs[0].default_value = (0.0006, 0.0015, 0.002, 1.0)
    c = mezcla(nt, 4, 2, 1.0, base.outputs[0], neb1.outputs[0], "ADD")
    c = mezcla(nt, 5, 1, 1.0, c, neb2.outputs[0], "ADD")
    c = mezcla(nt, 6, 0, 0.45, c, polvo.outputs[0], "MULTIPLY")

    s1 = estrellas(nt, vec, 3, -4, 160.0, 0.09)
    s2 = estrellas(nt, vec, 3, -6, 45.0, 0.05)
    suma = nodo(nt, "ShaderNodeMath", 7, -4)
    suma.operation = "ADD"
    nt.links.new(s1, suma.inputs[0])
    nt.links.new(s2, suma.inputs[1])
    fuerte = nodo(nt, "ShaderNodeMath", 8, -4)
    fuerte.operation = "MULTIPLY"
    fuerte.inputs[1].default_value = 3.0
    nt.links.new(suma.outputs[0], fuerte.inputs[0])
    blanco = nodo(nt, "ShaderNodeRGB", 7, -5)
    blanco.outputs[0].default_value = (0.9, 1.0, 0.95, 1.0)
    c = mezcla(nt, 9, 0, fuerte.outputs[0], c, blanco.outputs[0], "ADD")
    nt.links.new(c, bg.inputs["Color"])
    bg.inputs["Strength"].default_value = 1.0
    return w

def mundo_planeta():
    #superficie de xen vista desde el espacio: roca verde azulada, zonas cafes y grietas que brillan
    w = bpy.data.worlds.new("planeta")
    w.use_nodes = True
    nt = w.node_tree
    for n in list(nt.nodes):
        nt.nodes.remove(n)
    out = nodo(nt, "ShaderNodeOutputWorld", 12, 0)
    bg = nodo(nt, "ShaderNodeBackground", 11, 0)
    nt.links.new(bg.outputs[0], out.inputs[0])
    tc = nodo(nt, "ShaderNodeTexCoord", -1, 0)
    nor = nodo(nt, "ShaderNodeVectorMath", 0, 0)
    nor.operation = "NORMALIZE"
    nt.links.new(tc.outputs["Generated"], nor.inputs[0])
    vec = nor.outputs[0]

    n1 = noise(nt, vec, 1, 2, 2.0, 8.0, 0.55, 0.2)
    roca = rampa(nt, 2, 2, [(0.30, (0.015, 0.035, 0.03)), (0.48, (0.04, 0.11, 0.08)), (0.60, (0.10, 0.20, 0.12)),
                            (0.72, (0.22, 0.20, 0.09))])
    nt.links.new(n1.outputs["Fac"], roca.inputs[0])
    n2 = noise(nt, vec, 1, 0, 2.5, 5.0, 0.5, 0.3)
    zonas = rampa(nt, 2, 0, [(0.50, (0.0, 0.0, 0.0)), (0.62, (1.0, 1.0, 1.0))])
    nt.links.new(n2.outputs["Fac"], zonas.inputs[0])
    cafe = nodo(nt, "ShaderNodeRGB", 2, 1)
    cafe.outputs[0].default_value = (0.20, 0.11, 0.04, 1.0)
    c = mezcla(nt, 4, 1, zonas.outputs[0], roca.outputs[0], cafe.outputs[0])

    vor = nodo(nt, "ShaderNodeTexVoronoi", 1, -2)
    vor.feature = "DISTANCE_TO_EDGE"
    vor.inputs["Scale"].default_value = 3.5
    dist = noise(nt, vec, 0, -3, 5.0, 4.0, 0.5)
    sumav = nodo(nt, "ShaderNodeVectorMath", 0.5, -2.5)
    sumav.operation = "MULTIPLY_ADD"
    sumav.inputs[1].default_value = (0.12, 0.12, 0.12)
    nt.links.new(dist.outputs["Color"], sumav.inputs[0])
    nt.links.new(vec, sumav.inputs[2])
    nt.links.new(sumav.outputs[0], vor.inputs["Vector"])
    grietas = rampa(nt, 2, -2, [(0.0, (1.0, 1.0, 1.0)), (0.012, (0.0, 0.0, 0.0))])
    nt.links.new(vor.outputs["Distance"], grietas.inputs[0])
    #solo brillan las grietas donde la mascara de ruido es alta
    n5 = noise(nt, vec, 1, -4, 3.0, 3.0, 0.5)
    mascara = rampa(nt, 2, -4, [(0.45, (0.0, 0.0, 0.0)), (0.6, (1.0, 1.0, 1.0))])
    nt.links.new(n5.outputs["Fac"], mascara.inputs[0])
    grieta_fac = nodo(nt, "ShaderNodeMath", 3, -3)
    grieta_fac.operation = "MULTIPLY"
    nt.links.new(grietas.outputs[0], grieta_fac.inputs[0])
    nt.links.new(mascara.outputs[0], grieta_fac.inputs[1])
    brillo = nodo(nt, "ShaderNodeRGB", 2, -3)
    brillo.outputs[0].default_value = (0.20, 0.85, 0.55, 1.0)
    c = mezcla(nt, 5, 0, grieta_fac.outputs[0], c, brillo.outputs[0])

    n4 = noise(nt, vec, 1, 4, 2.0, 5.0, 0.5, 0.4)
    nubes = rampa(nt, 2, 4, [(0.58, (0.0, 0.0, 0.0)), (0.78, (0.35, 0.35, 0.35))])
    nt.links.new(n4.outputs["Fac"], nubes.inputs[0])
    nube_col = nodo(nt, "ShaderNodeRGB", 3, 5)
    nube_col.outputs[0].default_value = (0.55, 0.70, 0.60, 1.0)
    c = mezcla(nt, 6, 2, nubes.outputs[0], c, nube_col.outputs[0])
    nt.links.new(c, bg.inputs["Color"])
    return w

def render_pano(mundo, ancho, archivo):
    scn.world = mundo
    scn.render.engine = "CYCLES"
    scn.cycles.device = "CPU"
    scn.cycles.samples = 16
    scn.render.resolution_x = ancho
    scn.render.resolution_y = ancho // 2
    scn.render.resolution_percentage = 100
    scn.render.image_settings.file_format = "PNG"
    scn.render.image_settings.color_mode = "RGB"
    scn.view_settings.view_transform = "Standard"
    scn.render.filepath = os.path.join(OUT, archivo)
    bpy.ops.render.render(write_still=True)

cam_d = bpy.data.cameras.new("cam")
cam_d.type = "PANO"
cam_d.panorama_type = "EQUIRECTANGULAR"
cam = bpy.data.objects.new("cam", cam_d)
cam.rotation_euler = (math.radians(90), 0, 0)
scn.collection.objects.link(cam)
scn.camera = cam

render_pano(mundo_cielo(), 4096, "xen_cielo_pano.png")
render_pano(mundo_planeta(), 2048, "xen_planeta.png")

#esfera del planeta con UV equirectangulares (las de la UV sphere de blender)
bpy.ops.mesh.primitive_uv_sphere_add(segments=64, ring_count=32, radius=1.0)
esf = bpy.context.active_object
esf.name = "xen_planeta"
bpy.ops.object.shade_smooth()
m = bpy.data.materials.new("xen_planeta")
m.use_nodes = True
t = m.node_tree.nodes.new("ShaderNodeTexImage")
t.image = bpy.data.images.load(os.path.join(OUT, "xen_planeta.png"))
m.node_tree.links.new(t.outputs[0], m.node_tree.nodes["Principled BSDF"].inputs["Base Color"])
esf.data.materials.append(m)
bpy.ops.wm.obj_export(filepath=os.path.join(OUT, "xen_planeta.obj"), export_selected_objects=True,
                      export_materials=True, path_mode="STRIP", export_normals=True, export_uv=True)
