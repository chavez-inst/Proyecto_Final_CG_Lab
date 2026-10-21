#pasa un modelo descargado (.gltf) a .obj para el proyecto: una sola malla con la pose aplicada,
#escalado a una altura dada, centrado en X y Z, con los pies en y = 0 y mirando hacia +Z en opengl
#uso: blender -b --factory-startup -P npc_convertir.py -- <scene.gltf> <salida.obj> <altura> <giro en grados> [vista.png]
#el giro es alrededor del eje vertical, para que el frente del modelo quede hacia +Z
import bpy, sys, os, math
from mathutils import Vector, Matrix

args = sys.argv[sys.argv.index("--") + 1:]
ENTRADA, SALIDA, ALTURA, GIRO = args[0], args[1], float(args[2]), float(args[3])
VISTA = args[4] if len(args) > 4 else None

bpy.ops.wm.read_factory_settings(use_empty=True)
scn = bpy.context.scene
bpy.ops.import_scene.gltf(filepath=ENTRADA)

#cada malla con su pose (armature) y transformaciones aplicadas, en una sola malla nueva
dg = bpy.context.evaluated_depsgraph_get()
mallas = [o for o in scn.objects if o.type == "MESH" and o.visible_get()]
nuevas = []
for o in mallas:
    ev = o.evaluated_get(dg)
    me = bpy.data.meshes.new_from_object(ev, preserve_all_data_layers=True, depsgraph=dg)
    me.transform(o.matrix_world)
    nuevas.append(me)
for o in list(scn.objects):
    bpy.data.objects.remove(o, do_unlink=True)
objs = []
for me in nuevas:
    ob = bpy.data.objects.new("pieza", me)
    scn.collection.objects.link(ob)
    objs.append(ob)
bpy.ops.object.select_all(action="DESELECT")
for o in objs:
    o.select_set(True)
bpy.context.view_layer.objects.active = objs[0]
if len(objs) > 1:
    bpy.ops.object.join()
ob = bpy.context.view_layer.objects.active
ob.name = os.path.splitext(os.path.basename(SALIDA))[0]

#giro, escala y posicion
me = ob.data
me.transform(Matrix.Rotation(math.radians(GIRO), 4, "Z"))
xs = [v.co.x for v in me.vertices]
ys = [v.co.y for v in me.vertices]
zs = [v.co.z for v in me.vertices]
esc = ALTURA / (max(zs) - min(zs))
cx, cy = (max(xs) + min(xs)) / 2, (max(ys) + min(ys)) / 2
me.transform(Matrix.Translation(Vector((-cx, -cy, -min(zs)))))
me.transform(Matrix.Scale(esc, 4))
me.update()
print("MEDIDAS", ob.name, "ancho %.3f largo %.3f alto %.3f" % ((max(xs) - min(xs)) * esc, (max(ys) - min(ys)) * esc, ALTURA))

#texturas: se copian junto al obj en Textures/ (el .mtl solo guarda el nombre)
carpeta = os.path.dirname(SALIDA)
os.makedirs(os.path.join(carpeta, "Textures"), exist_ok=True)
for img in bpy.data.images:
    if img.source == "FILE" and img.filepath:
        ruta = bpy.path.abspath(img.filepath)
        nombre = os.path.basename(ruta)
        if os.path.exists(ruta):
            with open(ruta, "rb") as f:
                datos = f.read()
            with open(os.path.join(carpeta, "Textures", nombre), "wb") as f:
                f.write(datos)
for m in me.materials:
    tiene = m and m.use_nodes and any(n.type == "TEX_IMAGE" and n.image for n in m.node_tree.nodes)
    print("MATERIAL", m.name if m else None, "con textura" if tiene else "SIN TEXTURA")

bpy.ops.object.select_all(action="DESELECT")
ob.select_set(True)
bpy.ops.wm.obj_export(filepath=SALIDA, export_selected_objects=True, export_materials=True,
                      path_mode="STRIP", export_normals=True, export_uv=True)

#vista de frente (desde -Y de blender, que es +Z en opengl) y de lado
if VISTA:
    scn.render.engine = "BLENDER_WORKBENCH"
    scn.display.shading.color_type = "TEXTURE"
    scn.display.shading.light = "STUDIO"
    scn.render.resolution_x = 900
    scn.render.resolution_y = 600
    cam_d = bpy.data.cameras.new("cam")
    cam_d.type = "ORTHO"
    cam_d.ortho_scale = ALTURA * 2.2
    cam = bpy.data.objects.new("cam", cam_d)
    scn.collection.objects.link(cam)
    scn.camera = cam
    base, ext = os.path.splitext(VISTA)
    for nombre, pos in (("frente", (0, -10, ALTURA / 2)), ("lado", (10, 0, ALTURA / 2))):
        cam.location = pos
        d = Vector((0, 0, ALTURA / 2)) - Vector(pos)
        cam.rotation_euler = d.to_track_quat("-Z", "Y").to_euler()
        scn.render.filepath = base + "_" + nombre + ext
        bpy.ops.render.render(write_still=True)
