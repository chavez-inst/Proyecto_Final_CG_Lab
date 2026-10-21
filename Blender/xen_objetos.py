#objetos de la superficie de xen: cristal, planta de luz (tallo y foco) e isla flotante
#uso: blender -b --factory-startup -P xen_objetos.py -- <carpeta Models/Xen> [vista.png]
#cada objeto sale en su .obj con una textura horneada en Models/Xen/Textures
#en blender se modela con Z arriba; el exportador lo pasa a Y arriba (opengl)
import bpy, bmesh, math, sys, os, random
from mathutils import Vector, Matrix, noise

args = sys.argv[sys.argv.index("--") + 1:]
OUT = args[0]
VISTA = args[1] if len(args) > 1 else None
os.makedirs(os.path.join(OUT, "Textures"), exist_ok=True)
random.seed(7)

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


def material(nombre, escala, colores, por_altura=None):
    #color con ruido; si por_altura = (zmin, zmax, colores) se mezcla con un degradado en Z
    m = bpy.data.materials.new(nombre)
    m.use_nodes = True
    nt = m.node_tree
    for n in list(nt.nodes):
        nt.nodes.remove(n)
    out = nodo(nt, "ShaderNodeOutputMaterial", 7, 0)
    bsdf = nodo(nt, "ShaderNodeBsdfDiffuse", 6, 0)
    nt.links.new(bsdf.outputs[0], out.inputs[0])
    tc = nodo(nt, "ShaderNodeTexCoord", 0, 0)
    n1 = nodo(nt, "ShaderNodeTexNoise", 1, 0)
    n1.inputs["Scale"].default_value = escala
    n1.inputs["Detail"].default_value = 6.0
    nt.links.new(tc.outputs["Object"], n1.inputs["Vector"])
    r = rampa(nt, 2, 0, colores)
    nt.links.new(n1.outputs["Fac"], r.inputs[0])
    color = r.outputs[0]
    if por_altura:
        sep = nodo(nt, "ShaderNodeSeparateXYZ", 1, -1)
        nt.links.new(tc.outputs["Object"], sep.inputs[0])
        mr = nodo(nt, "ShaderNodeMapRange", 2, -1)
        mr.inputs["From Min"].default_value = por_altura[0]
        mr.inputs["From Max"].default_value = por_altura[1]
        nt.links.new(sep.outputs["Z"], mr.inputs["Value"])
        r2 = rampa(nt, 3, -1, por_altura[2])
        nt.links.new(mr.outputs[0], r2.inputs[0])
        mx = nodo(nt, "ShaderNodeMix", 4, 0)
        mx.data_type = "RGBA"
        mx.blend_type = "MULTIPLY"
        mx.inputs[0].default_value = 1.0
        nt.links.new(color, mx.inputs[6])
        nt.links.new(r2.outputs[0], mx.inputs[7])
        color = mx.outputs[2]
    nt.links.new(color, bsdf.inputs["Color"])
    return m


def objeto(nombre, bm, mats):
    me = bpy.data.meshes.new(nombre)
    for m in mats:
        me.materials.append(m)
    bm.to_mesh(me)
    bm.free()
    ob = bpy.data.objects.new(nombre, me)
    scn.collection.objects.link(ob)
    for p in me.polygons:
        p.use_smooth = True
    return ob


def hornear(objs, nombre, tam):
    #UV de todos los objetos juntos en una textura y se hornea el color
    bpy.ops.object.select_all(action="DESELECT")
    for o in objs:
        o.select_set(True)
    bpy.context.view_layer.objects.active = objs[0]
    bpy.ops.object.mode_set(mode="EDIT")
    bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.smart_project(angle_limit=math.radians(60), island_margin=0.005)
    bpy.ops.uv.pack_islands(margin=0.005)
    bpy.ops.object.mode_set(mode="OBJECT")
    img = bpy.data.images.new(nombre, tam, tam)
    mats = set()
    for o in objs:
        for m in o.data.materials:
            mats.add(m)
    for m in mats:
        t = m.node_tree.nodes.new("ShaderNodeTexImage")
        t.image = img
        m.node_tree.nodes.active = t
    scn.render.engine = "CYCLES"
    scn.cycles.device = "CPU"
    scn.cycles.samples = 4
    scn.render.bake.use_pass_direct = False
    scn.render.bake.use_pass_indirect = False
    scn.render.bake.use_pass_color = True
    scn.render.bake.margin = 6
    bpy.ops.object.bake(type="DIFFUSE", pass_filter={"COLOR"}, use_clear=True)
    img.filepath_raw = os.path.join(OUT, "Textures", nombre + ".png")
    img.file_format = "PNG"
    img.save()
    final = bpy.data.materials.new(nombre)
    final.use_nodes = True
    t = final.node_tree.nodes.new("ShaderNodeTexImage")
    t.image = img
    final.node_tree.links.new(t.outputs[0], final.node_tree.nodes["Principled BSDF"].inputs["Base Color"])
    for o in objs:
        o.data.materials.clear()
        o.data.materials.append(final)


def exporta(o):
    bpy.ops.object.select_all(action="DESELECT")
    o.select_set(True)
    bpy.context.view_layer.objects.active = o
    bpy.ops.wm.obj_export(filepath=os.path.join(OUT, o.name + ".obj"), export_selected_objects=True,
                          export_materials=True, path_mode="STRIP", export_normals=True, export_uv=True)


# ---------------- cristal: varios prismas de 6 lados con punta ----------------

def prisma(bm, base, eje, radio, largo):
    eje = eje.normalized()
    rot = eje.to_track_quat("Z", "Y").to_matrix().to_4x4()
    abajo, arriba = [], []
    for i in range(6):
        a = i * math.pi / 3
        p = Vector((math.cos(a) * radio, math.sin(a) * radio, 0))
        abajo.append(bm.verts.new(base + rot @ p))
        arriba.append(bm.verts.new(base + rot @ (p + Vector((0, 0, largo)))))
    punta = bm.verts.new(base + rot @ Vector((0, 0, largo + radio * 1.8)))
    for i in range(6):
        j = (i + 1) % 6
        bm.faces.new((abajo[i], abajo[j], arriba[j], arriba[i]))
        bm.faces.new((arriba[i], arriba[j], punta))
    bm.faces.new(list(reversed(abajo)))


def cristal():
    bm = bmesh.new()
    prisma(bm, Vector((0, 0, -0.1)), Vector((0, 0, 1)), 0.13, 1.1)
    for i in range(6):
        a = i * math.pi / 3 + random.uniform(-0.3, 0.3)
        inc = random.uniform(0.35, 0.7)
        eje = Vector((math.cos(a) * math.sin(inc), math.sin(a) * math.sin(inc), math.cos(inc)))
        prisma(bm, Vector((math.cos(a) * 0.12, math.sin(a) * 0.12, -0.1)), eje,
               random.uniform(0.06, 0.1), random.uniform(0.35, 0.75))
    bmesh.ops.recalc_face_normals(bm, faces=bm.faces)
    m = material("cristal_col", 6.0, [(0.3, (0.85, 0.45, 0.05)), (0.7, (1.0, 0.75, 0.25))],
                 (0.0, 1.3, [(0.0, (0.55, 0.25, 0.05)), (1.0, (1.0, 1.0, 0.85))]))
    ob = objeto("xen_cristal", bm, [m])
    for p in ob.data.polygons:
        p.use_smooth = False
    return ob


# ---------------- planta de luz: tallo curvo y foco que se puede mover aparte ----------------

ALTO_PLANTA = 1.6


def planta():
    cu = bpy.data.curves.new("tallo_c", "CURVE")
    cu.dimensions = "3D"
    cu.bevel_depth = 1.0
    cu.bevel_resolution = 3
    cu.resolution_u = 8
    cu.use_fill_caps = True
    sp = cu.splines.new("BEZIER")
    puntos = [(0, 0, 0), (0.06, 0.03, 0.5), (-0.04, 0.0, 1.0), (0, 0, ALTO_PLANTA)]
    radios = [0.07, 0.045, 0.035, 0.03]
    sp.bezier_points.add(len(puntos) - 1)
    for bp, p, r in zip(sp.bezier_points, puntos, radios):
        bp.co = Vector(p)
        bp.radius = r
        bp.handle_left_type = "AUTO"
        bp.handle_right_type = "AUTO"
    ob = bpy.data.objects.new("xen_planta_tallo", cu)
    scn.collection.objects.link(ob)
    bpy.ops.object.select_all(action="DESELECT")
    ob.select_set(True)
    bpy.context.view_layer.objects.active = ob
    bpy.ops.object.convert(target="MESH")
    tallo = bpy.context.view_layer.objects.active
    tallo.data.materials.append(material("tallo_col", 25.0, [(0.3, (0.12, 0.10, 0.05)), (0.7, (0.30, 0.28, 0.12))]))
    #raiz: bulto de roca en la base
    bm = bmesh.new()
    bm.from_mesh(tallo.data)
    g = bmesh.ops.create_uvsphere(bm, u_segments=16, v_segments=8, radius=1.0)
    for v in g["verts"]:
        d = 1.0 + 0.25 * noise.noise(v.co * 3.0)
        v.co = Vector((v.co.x * 0.16 * d, v.co.y * 0.16 * d, v.co.z * 0.08 * d))
    bm.to_mesh(tallo.data)
    bm.free()
    for p in tallo.data.polygons:
        p.use_smooth = True
    #foco: bulbo con "petalos" alrededor, centrado en la punta del tallo
    bm = bmesh.new()
    g = bmesh.ops.create_uvsphere(bm, u_segments=24, v_segments=12, radius=1.0)
    for v in g["verts"]:
        v.co = Vector((v.co.x * 0.11, v.co.y * 0.11, v.co.z * 0.14 + ALTO_PLANTA + 0.1))
    n_foco = len(bm.faces)
    for i in range(5):
        a = i * 2 * math.pi / 5
        g = bmesh.ops.create_cone(bm, cap_ends=True, segments=6, radius1=0.05, radius2=0.0, depth=0.22)
        rot = Matrix.Rotation(math.radians(55), 4, "X")
        rot = Matrix.Rotation(a, 4, "Z") @ rot
        for v in g["verts"]:
            v.co = rot @ (v.co + Vector((0, 0, 0.11))) + Vector((0, 0, ALTO_PLANTA + 0.02))
    bm.faces.ensure_lookup_table()
    for i, f in enumerate(bm.faces):
        f.material_index = 0 if i < n_foco else 1
    foco = objeto("xen_planta_foco", bm, [
        material("foco_col", 8.0, [(0.3, (0.95, 0.95, 0.70)), (0.7, (1.0, 1.0, 0.95))]),
        material("petalo_col", 20.0, [(0.3, (0.20, 0.30, 0.12)), (0.7, (0.40, 0.50, 0.20))])])
    return tallo, foco


# ---------------- isla flotante: arriba plana con musgo y abajo una punta de roca ----------------

def isla():
    bm = bmesh.new()
    g = bmesh.ops.create_uvsphere(bm, u_segments=40, v_segments=20, radius=1.0)
    for v in g["verts"]:
        x, y, z = v.co
        d = 1.0 + 0.18 * noise.noise(Vector((x, y, z)) * 2.2) + 0.06 * noise.noise(Vector((x, y, z)) * 7.0)
        if z > 0:
            z = z * 0.18
        else:
            z = -(abs(z) ** 0.8) * 1.6
        v.co = Vector((x * 2.0 * d, y * 1.6 * d, z * d))
    bm.normal_update()
    for f in bm.faces:
        f.material_index = 0 if f.calc_center_median().z > 0.02 else 1
    return objeto("xen_isla", bm, [
        material("musgo_col", 6.0, [(0.3, (0.10, 0.25, 0.18)), (0.55, (0.25, 0.45, 0.28)), (0.75, (0.55, 0.60, 0.30))]),
        material("roca_col", 5.0, [(0.3, (0.12, 0.09, 0.06)), (0.55, (0.30, 0.22, 0.14)), (0.75, (0.45, 0.38, 0.25))],
                 (-1.8, 0.0, [(0.0, (0.35, 0.35, 0.35)), (1.0, (1.0, 1.0, 1.0))]))])


cr = cristal()
hornear([cr], "xen_cristal", 512)
tallo, foco = planta()
hornear([tallo, foco], "xen_planta", 512)
isl = isla()
hornear([isl], "xen_isla", 1024)

for o in (cr, tallo, foco, isl):
    exporta(o)
print("FOCO_PIVOTE opengl: 0.0 %.3f 0.0" % (ALTO_PLANTA + 0.02))

if VISTA:
    cam_d = bpy.data.cameras.new("cam")
    cam = bpy.data.objects.new("cam", cam_d)
    scn.collection.objects.link(cam)
    scn.camera = cam
    sun_d = bpy.data.lights.new("sol", "SUN")
    sun_d.energy = 4.0
    sun = bpy.data.objects.new("sol", sun_d)
    sun.rotation_euler = (math.radians(40), math.radians(20), math.radians(30))
    scn.collection.objects.link(sun)
    w = bpy.data.worlds.new("w")
    scn.world = w
    w.color = (0.3, 0.3, 0.3)
    cr.location = (-1.2, 0, 0)
    tallo.location = foco.location = (0, 0, 0)
    isl.location = (2.6, 1.5, 1.2)
    isl.scale = (0.6, 0.6, 0.6)
    cam.location = (0.6, -6.5, 2.4)
    cam.rotation_euler = (Vector((0.5, 0, 0.9)) - cam.location).to_track_quat("-Z", "Y").to_euler()
    scn.cycles.samples = 32
    scn.render.resolution_x = 900
    scn.render.resolution_y = 600
    scn.render.filepath = VISTA
    bpy.ops.render.render(write_still=True)
