#headcrab de half-life por piezas para el modelo jerarquico
#uso: blender -b --factory-startup -P headcrab.py -- <carpeta de salida> <png de vista previa>
#en blender: Z arriba, el headcrab mira hacia +Y, la derecha del headcrab esta en X negativa
import bpy, bmesh, math, sys, os
from mathutils import Vector

args = sys.argv[sys.argv.index("--") + 1:]
OUT = args[0]
PREVIEW = args[1] if len(args) > 1 else None
TEX = 1024

bpy.ops.wm.read_factory_settings(use_empty=True)
scn = bpy.context.scene

# ---------------- materiales de color (se hornean a una sola textura) ----------------

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

def mat_base(nombre):
    m = bpy.data.materials.new(nombre)
    m.use_nodes = True
    nt = m.node_tree
    for n in list(nt.nodes):
        nt.nodes.remove(n)
    out = nodo(nt, "ShaderNodeOutputMaterial", 6, 0)
    bsdf = nodo(nt, "ShaderNodeBsdfDiffuse", 5, 0)
    nt.links.new(bsdf.outputs[0], out.inputs[0])
    coord = nodo(nt, "ShaderNodeTexCoord", 0, 0)
    return m, nt, bsdf, coord

def noise(nt, coord, x, y, escala, detalle=6.0, rug=0.6):
    n = nodo(nt, "ShaderNodeTexNoise", x, y)
    n.inputs["Scale"].default_value = escala
    n.inputs["Detail"].default_value = detalle
    n.inputs["Roughness"].default_value = rug
    nt.links.new(coord.outputs["Object"], n.inputs["Vector"])
    return n

def mezcla(nt, x, y, a, b, fac):
    m = nodo(nt, "ShaderNodeMix", x, y)
    m.data_type = "RGBA"
    nt.links.new(fac, m.inputs[0])
    nt.links.new(a, m.inputs[6])
    nt.links.new(b, m.inputs[7])
    return m.outputs[2]

def mat_caparazon():
    #parte de arriba: crema con manchas y venas rojizas
    m, nt, bsdf, coord = mat_base("caparazon")
    n1 = noise(nt, coord, 1, 1, 18.0)
    base0 = rampa(nt, 2, 1, [(0.30, (0.58, 0.50, 0.28)), (0.55, (0.80, 0.72, 0.46)), (0.75, (0.88, 0.82, 0.58))])
    nt.links.new(n1.outputs["Fac"], base0.inputs[0])
    n2 = noise(nt, coord, 1, 2, 7.0, 4.0, 0.55)
    manchas = rampa(nt, 2, 2, [(0.50, (0, 0, 0)), (0.62, (1, 1, 1))])
    nt.links.new(n2.outputs["Fac"], manchas.inputs[0])
    col_mancha = nodo(nt, "ShaderNodeRGB", 2, 3)
    col_mancha.outputs[0].default_value = (0.50, 0.38, 0.20, 1.0)
    base = nodo(nt, "ShaderNodeMix", 3, 1)
    base.data_type = "RGBA"
    nt.links.new(manchas.outputs[0], base.inputs[0])
    nt.links.new(base0.outputs[0], base.inputs[6])
    nt.links.new(col_mancha.outputs[0], base.inputs[7])
    base.outputs[0].name
    vor = nodo(nt, "ShaderNodeTexVoronoi", 1, -1)
    vor.feature = "DISTANCE_TO_EDGE"
    vor.inputs["Scale"].default_value = 14.0
    dist = noise(nt, coord, 0.5, -2, 9.0, 2.0, 0.5)
    sumav = nodo(nt, "ShaderNodeVectorMath", 0.8, -1.5)
    sumav.operation = "MULTIPLY_ADD"
    sumav.inputs[1].default_value = (0.09, 0.09, 0.09)
    nt.links.new(dist.outputs["Color"], sumav.inputs[0])
    nt.links.new(coord.outputs["Object"], sumav.inputs[2])
    nt.links.new(sumav.outputs[0], vor.inputs["Vector"])
    venas = rampa(nt, 2, -1, [(0.0, (1, 1, 1)), (0.035, (0, 0, 0))])
    nt.links.new(vor.outputs["Distance"], venas.inputs[0])
    col_vena = nodo(nt, "ShaderNodeRGB", 2, -2)
    col_vena.outputs[0].default_value = (0.45, 0.22, 0.14, 1.0)
    fac = nodo(nt, "ShaderNodeMath", 3, -1)
    fac.operation = "MULTIPLY"
    fac.inputs[1].default_value = 0.45
    nt.links.new(venas.outputs[0], fac.inputs[0])
    c = mezcla(nt, 4, 0, base.outputs[2], col_vena.outputs[0], fac.outputs[0])
    nt.links.new(c, bsdf.inputs["Color"])
    return m

def mat_ruido(nombre, escala, colores):
    m, nt, bsdf, coord = mat_base(nombre)
    n1 = noise(nt, coord, 1, 0, escala)
    r = rampa(nt, 2, 0, colores)
    nt.links.new(n1.outputs["Fac"], r.inputs[0])
    nt.links.new(r.outputs[0], bsdf.inputs["Color"])
    return m

def mat_pata():
    #rojo carne, mas oscuro hacia la punta (z baja)
    m, nt, bsdf, coord = mat_base("pata")
    n1 = noise(nt, coord, 1, 1, 25.0)
    r = rampa(nt, 2, 1, [(0.35, (0.28, 0.04, 0.03)), (0.55, (0.48, 0.08, 0.05)), (0.75, (0.58, 0.16, 0.10))])
    nt.links.new(n1.outputs["Fac"], r.inputs[0])
    sep = nodo(nt, "ShaderNodeSeparateXYZ", 1, -1)
    nt.links.new(coord.outputs["Object"], sep.inputs[0])
    alto = nodo(nt, "ShaderNodeMapRange", 2, -1)
    alto.inputs["From Min"].default_value = 0.0
    alto.inputs["From Max"].default_value = 0.12
    nt.links.new(sep.outputs["Z"], alto.inputs["Value"])
    oscuro = nodo(nt, "ShaderNodeRGB", 2, -2)
    oscuro.outputs[0].default_value = (0.20, 0.05, 0.04, 1.0)
    inv = nodo(nt, "ShaderNodeMath", 3, -1)
    inv.operation = "SUBTRACT"
    inv.inputs[0].default_value = 1.0
    nt.links.new(alto.outputs[0], inv.inputs[1])
    c = mezcla(nt, 4, 0, r.outputs[0], oscuro.outputs[0], inv.outputs[0])
    nt.links.new(c, bsdf.inputs["Color"])
    return m

M_CAPA = mat_caparazon()
M_PIEL = mat_ruido("piel", 20.0, [(0.3, (0.16, 0.09, 0.05)), (0.55, (0.32, 0.20, 0.10)), (0.75, (0.45, 0.31, 0.17))])
M_MUSLO = mat_ruido("muslo", 22.0, [(0.3, (0.20, 0.10, 0.05)), (0.55, (0.36, 0.19, 0.09)), (0.75, (0.46, 0.27, 0.14))])
M_PATA = mat_pata()
M_UNA = mat_ruido("una", 40.0, [(0.3, (0.04, 0.035, 0.03)), (0.7, (0.14, 0.12, 0.10))])
M_BOCA = mat_ruido("boca", 30.0, [(0.3, (0.35, 0.08, 0.08)), (0.7, (0.62, 0.22, 0.20))])
M_DIENTE = mat_ruido("diente", 30.0, [(0.3, (0.70, 0.66, 0.52)), (0.7, (0.90, 0.88, 0.76))])

# ---------------- geometria ----------------

def nuevo_obj(nombre, bm, mats):
    me = bpy.data.meshes.new(nombre)
    bm.to_mesh(me)
    bm.free()
    ob = bpy.data.objects.new(nombre, me)
    scn.collection.objects.link(ob)
    for m in mats:
        me.materials.append(m)
    for p in me.polygons:
        p.use_smooth = True
    return ob

def esfera(bm, centro, radios, seg=24, anillos=12):
    geom = bmesh.ops.create_uvsphere(bm, u_segments=seg, v_segments=anillos, radius=1.0)
    vs = geom["verts"]
    for v in vs:
        v.co = Vector((v.co.x * radios[0], v.co.y * radios[1], v.co.z * radios[2])) + Vector(centro)
    return vs

def cuerpo():
    bm = bmesh.new()
    c = Vector((0.0, 0.0, 0.15))
    vs = esfera(bm, (0, 0, 0), (1, 1, 1), 48, 24)
    for v in vs:
        x, y, z = v.co
        if z < 0:
            z *= 0.55                      #panza aplanada
        else:
            z *= 1.05
            y -= 0.18 * z                  #el caparazon se va hacia atras
        r = 1.0 + 0.06 * math.exp(-((z - 0.27) / 0.07) ** 2)   #borde entre caparazon y piel
        r -= 0.05 * max(0.0, y) ** 3 * (1.0 if z < 0.2 else 0.0)  #frente un poco plano para la boca
        v.co = Vector((x * 0.15 * r, y * 0.19 * r, z * 0.13 * r)) + c
    bm.normal_update()
    for f in bm.faces:
        f.material_index = 0 if f.calc_center_median().z > c.z + 0.035 else 1
    return nuevo_obj("cuerpo", bm, [M_CAPA, M_PIEL])

PIVOTE_BOCA = Vector((0.0, 0.10, 0.10))

def boca():
    bm = bmesh.new()
    #labio inferior: media esfera aplanada debajo del frente
    vs = esfera(bm, (0.0, 0.135, 0.095), (0.075, 0.07, 0.032), 32, 12)
    for v in vs:
        if v.co.z > 0.095:
            v.co.z = 0.095 + (v.co.z - 0.095) * 0.35
    n_labio = len(bm.faces)
    #dientes: conos chicos alrededor de la orilla, apuntando hacia arriba
    for i in range(11):
        a = math.radians(-80 + i * 16)
        px = math.sin(a) * 0.062
        py = 0.135 + math.cos(a) * 0.058
        g = bmesh.ops.create_cone(bm, cap_ends=True, segments=8, radius1=0.0075, radius2=0.0, depth=0.024)
        for v in g["verts"]:
            v.co = Vector((v.co.x + px, v.co.y + py, v.co.z + 0.106))
    bm.faces.ensure_lookup_table()
    for i, f in enumerate(bm.faces):
        f.material_index = 0 if i < n_labio else 1
    return nuevo_obj("boca", bm, [M_BOCA, M_DIENTE])

def tubo(nombre, puntos, radios, mats, z_una=None, bola=None):
    #extremidad como curva con grosor variable, se convierte a malla
    cu = bpy.data.curves.new(nombre + "_c", "CURVE")
    cu.dimensions = "3D"
    cu.bevel_depth = 1.0
    cu.bevel_resolution = 4
    cu.resolution_u = 10
    cu.use_fill_caps = True
    sp = cu.splines.new("BEZIER")
    sp.bezier_points.add(len(puntos) - 1)
    for bp, p, r in zip(sp.bezier_points, puntos, radios):
        bp.co = Vector(p)
        bp.radius = r
        bp.handle_left_type = "AUTO"
        bp.handle_right_type = "AUTO"
    ob = bpy.data.objects.new(nombre, cu)
    scn.collection.objects.link(ob)
    bpy.ops.object.select_all(action="DESELECT")
    ob.select_set(True)
    bpy.context.view_layer.objects.active = ob
    bpy.ops.object.convert(target="MESH")
    ob = bpy.context.view_layer.objects.active
    #los materiales se ponen antes de los indices, si no clear() los regresa a 0
    ob.data.materials.clear()
    for m in mats:
        ob.data.materials.append(m)
    bm = bmesh.new()
    bm.from_mesh(ob.data)
    if bola is not None:
        esfera(bm, bola[0], (bola[1], bola[1], bola[1]), 16, 8)
    for f in bm.faces:
        f.material_index = 0
        if z_una is not None and f.calc_center_median().z < z_una:
            f.material_index = 1
    bm.to_mesh(ob.data)
    bm.free()
    for p in ob.data.polygons:
        p.use_smooth = True
    return ob

def lado(p, s):
    #s = -1 derecha (X negativa), +1 izquierda
    return (p[0] * s, p[1], p[2])

PIEZAS = []      #(nombre, pivote)

def patas():
    for s, sufijo in ((1, "der"), (-1, "izq")):
        #delanteras: largas, suben un poco y bajan hacia el frente con una garra
        cad = lado((0.095, 0.085, 0.135), s)
        rod = lado((0.175, 0.205, 0.205), s)
        tubo("muslo_del_" + sufijo, [cad, lado((0.145, 0.15, 0.19), s), rod],
             [0.036, 0.031, 0.026], [M_MUSLO], bola=(cad, 0.036))
        tubo("garra_del_" + sufijo, [rod, lado((0.20, 0.30, 0.15), s), lado((0.195, 0.37, 0.06), s), lado((0.185, 0.395, 0.0), s)],
             [0.026, 0.020, 0.011, 0.0015], [M_PATA, M_UNA], z_una=0.055, bola=(rod, 0.027))
        PIEZAS.append(("muslo_del_" + sufijo, cad))
        PIEZAS.append(("garra_del_" + sufijo, rod))
    for s, sufijo in ((1, "der"), (-1, "izq")):
        #traseras: mas cortas, hacia los lados y atras
        cad = lado((0.10, -0.06, 0.12), s)
        rod = lado((0.20, -0.12, 0.165), s)
        tubo("muslo_tras_" + sufijo, [cad, lado((0.155, -0.09, 0.155), s), rod],
             [0.032, 0.027, 0.022], [M_MUSLO], bola=(cad, 0.032))
        tubo("garra_tras_" + sufijo, [rod, lado((0.245, -0.165, 0.11), s), lado((0.255, -0.20, 0.0), s)],
             [0.022, 0.012, 0.0015], [M_PATA, M_UNA], z_una=0.045, bola=(rod, 0.023))
        PIEZAS.append(("muslo_tras_" + sufijo, cad))
        PIEZAS.append(("garra_tras_" + sufijo, rod))

cuerpo()
boca()
patas()
ORDEN = ["cuerpo", "boca",
         "muslo_del_der", "garra_del_der", "muslo_del_izq", "garra_del_izq",
         "muslo_tras_der", "garra_tras_der", "muslo_tras_izq", "garra_tras_izq"]
PIVOTES = {"cuerpo": Vector((0.0, 0.0, 0.15)), "boca": PIVOTE_BOCA}
for n, p in PIEZAS:
    PIVOTES[n] = Vector(p)
objs = [bpy.data.objects[n] for n in ORDEN]

# ---------------- UV juntos y horneado ----------------

bpy.ops.object.select_all(action="DESELECT")
for o in objs:
    o.select_set(True)
bpy.context.view_layer.objects.active = objs[0]
bpy.ops.object.mode_set(mode="EDIT")
bpy.ops.mesh.select_all(action="SELECT")
bpy.ops.uv.smart_project(angle_limit=math.radians(60), island_margin=0.004, scale_to_bounds=False)
bpy.ops.uv.pack_islands(margin=0.004)
bpy.ops.object.mode_set(mode="OBJECT")

img = bpy.data.images.new("headcrab", TEX, TEX)
for m in [M_CAPA, M_PIEL, M_MUSLO, M_PATA, M_UNA, M_BOCA, M_DIENTE]:
    nt = m.node_tree
    t = nt.nodes.new("ShaderNodeTexImage")
    t.image = img
    t.location = (1500, -500)
    nt.nodes.active = t

scn.render.engine = "CYCLES"
scn.cycles.device = "CPU"
scn.cycles.samples = 4
scn.render.bake.use_pass_direct = False
scn.render.bake.use_pass_indirect = False
scn.render.bake.use_pass_color = True
scn.render.bake.margin = 6
bpy.ops.object.bake(type="DIFFUSE", pass_filter={"COLOR"}, use_clear=True)

os.makedirs(os.path.join(OUT, "Textures"), exist_ok=True)
img.filepath_raw = os.path.join(OUT, "Textures", "headcrab.png")
img.file_format = "PNG"
img.save()

#material final: solo la textura horneada
M_FINAL = bpy.data.materials.new("headcrab")
M_FINAL.use_nodes = True
nt = M_FINAL.node_tree
bsdf = nt.nodes.get("Principled BSDF")
bsdf.inputs["Roughness"].default_value = 0.45
t = nt.nodes.new("ShaderNodeTexImage")
t.image = img
nt.links.new(t.outputs[0], bsdf.inputs["Base Color"])
for o in objs:
    o.data.materials.clear()
    o.data.materials.append(M_FINAL)

# ---------------- exportar cada pieza ----------------
#giro de 180 en Z: en opengl queda mirando a +Z, Y arriba y la derecha del headcrab en X negativa
for o in objs:
    for v in o.data.vertices:
        v.co.x = -v.co.x
        v.co.y = -v.co.y
    o.data.update()
for n in ORDEN:
    PIVOTES[n] = Vector((-PIVOTES[n].x, -PIVOTES[n].y, PIVOTES[n].z))

for o in objs:
    bpy.ops.object.select_all(action="DESELECT")
    o.select_set(True)
    bpy.context.view_layer.objects.active = o
    bpy.ops.wm.obj_export(filepath=os.path.join(OUT, "headcrab_" + o.name + ".obj"),
                          export_selected_objects=True, export_materials=True, path_mode="STRIP",
                          export_normals=True, export_uv=True, apply_modifiers=True)

#pivotes en coordenadas de opengl (el exportador pasa Z arriba de blender a Y arriba)
print("PIVOTES_OPENGL")
for n in ORDEN:
    p = PIVOTES[n]
    print("%s %.4f %.4f %.4f" % (n, p.x, p.z, -p.y))

bpy.ops.wm.save_as_mainfile(filepath=os.path.join(OUT, "headcrab.blend"))

# ---------------- vista previa ----------------
if PREVIEW:
    cam_d = bpy.data.cameras.new("cam")
    cam = bpy.data.objects.new("cam", cam_d)
    scn.collection.objects.link(cam)
    cam.location = (-0.75, -0.85, 0.55)
    d = Vector((0, -0.08, 0.12)) - cam.location
    cam.rotation_euler = d.to_track_quat("-Z", "Y").to_euler()
    cam_d.lens = 50
    scn.camera = cam
    sun_d = bpy.data.lights.new("sol", "SUN")
    sun_d.energy = 4.0
    sun = bpy.data.objects.new("sol", sun_d)
    sun.rotation_euler = (math.radians(40), math.radians(20), math.radians(30))
    scn.collection.objects.link(sun)
    w = bpy.data.worlds.new("w")
    w.use_nodes = True
    w.node_tree.nodes["Background"].inputs[0].default_value = (0.8, 0.8, 0.8, 1)
    w.node_tree.nodes["Background"].inputs[1].default_value = 0.5
    scn.world = w
    scn.cycles.samples = 48
    scn.render.resolution_x = 900
    scn.render.resolution_y = 700
    scn.render.filepath = PREVIEW
    bpy.ops.render.render(write_still=True)
