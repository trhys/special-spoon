# Exports animations from blend file into a packed sprite sheet
# with a companion JSON file that we'll use to import into
# animation data on the engine side

# run from inside blender - assumes the current selection is the object to be animated
# iterate actions, assign each to the rig, and export each animation onto the same sheet

# generates a companion JSON file with animation data we can deserialize
# as sprite cords and animation data (see Special-Spoon/src/.../AnimationData.h)

# Usage (from a terminal, since Blender forwards args after "--" to this script):
#   blender --background your_scene.blend --python scripts/blenderExport.py -- \
#       --output "C:/dev/special-spoon/exports" --mode isometric --sprite-width 128 --sprite-height 128
# DEPENDANCY NOTE: This script requires the Pillow library for image manipulation. Install it via pip if you haven't already:
#
# Can also be run interactively from Blender's Scripting tab / Text Editor;
# in that case no CLI args are available, so defaults below are used instead.

'''
Exported animation data structure for reference in C++ engine code

struct SpriteCords
    {
        SpriteCords() = default;
        SpriteCords(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {}
        int x;
        int y;
        int width;
        int height;
    };
    
    struct AnimationData
    {
        std::string ID;
        std::string textureID;
        std::vector<SpriteCords> spriteCords;
        float frameRate = 0.1f;
        bool isLooping = true;
    };
'''

import bpy
import os
import sys
import json
import math
import argparse
from PIL import Image
from mathutils import Vector

# yaw (degrees around world Z) applied to the camera pivot for each named direction.
# the pivot orbits the rig while keeping the camera's pitch/distance fixed, so only
# "top-down"/"isometric" camera rigs with a fixed downward tilt are supported.
#
# NOTE: these angles are deliberately mirrored left<->right relative to a naive
# "camera orbit yaw" mapping. Viewing a character face-on inherently mirrors
# left/right (the subject's own right hand appears on the *viewer's* left),
# but these labels are meant to describe on-screen/movement direction as seen
# by the player, not the character's own anatomical side. "up" and "down" are
# unaffected since mirroring is only across the vertical (screen-horizontal) axis.
DIRECTION_YAWS = {
    "right": 180,
    "up-right": 135,
    "up": 90,
    "up-left": 45,
    "left": 0,
    "down-left": 315,
    "down": 270,
    "down-right": 225,
}

DEFAULT_OUTPUT_PATH = "//export"
DEFAULT_MODE = "isometric"
DEFAULT_SPRITE_WIDTH = 128
DEFAULT_SPRITE_HEIGHT = 128
DEFAULT_COLUMNS = 16
DEFAULT_ORTHO_PADDING = 1.1  # 10% extra headroom to avoid clipping on angles that expose more silhouette

# Camera pitch (degrees above the horizontal plane) used per mode when the
# user doesn't explicitly override it with --camera-pitch. 35.264 is the
# classic "true isometric" angle (arctan(1/sqrt(2))); top-down looks straight
# down the rig's local Z axis.
DEFAULT_MODE_PITCH_DEGREES = {
    "isometric": 35.264,
    "top-down": 90.0,
}

# How far back to place the camera, expressed as a multiple of the rig's
# bounding radius. Since we render orthographically, this mostly just needs
# to be far enough to avoid near-clipping and comfortably inside far-clipping.
DEFAULT_CAMERA_DISTANCE_MULTIPLIER = 4.0


def _find_deformed_meshes(rig):
    # Character meshes are frequently NOT parented to the armature - they're
    # linked purely via an Armature modifier pointing back at the rig. Using
    # rig.children_recursive alone misses these entirely, silently falling
    # back to default bounds and producing incorrect framing with no
    # indication anything went wrong.
    meshes = []
    for obj in bpy.data.objects:
        if obj.type != 'MESH':
            continue
        if obj in rig.children_recursive:
            meshes.append(obj)
            continue
        for modifier in obj.modifiers:
            if modifier.type == 'ARMATURE' and modifier.object == rig:
                meshes.append(obj)
                break
    return meshes


def _compute_rig_bounds(rig):
    # Compute separate horizontal (XY) and vertical (Z) half-extents around
    # the rig's location, in world space, covering every mesh deformed by
    # this rig (whether parented or linked via an Armature modifier). Uses
    # the depsgraph-evaluated mesh so the bounds reflect the character's
    # current pose/deformation rather than its rest-pose local bounding box.
    depsgraph = bpy.context.evaluated_depsgraph_get()
    center = rig.matrix_world.translation
    horizontal_radius = 0.0
    vertical_half_height = 0.0

    meshes = _find_deformed_meshes(rig)
    for obj in meshes:
        evaluated_obj = obj.evaluated_get(depsgraph)
        evaluated_mesh = evaluated_obj.to_mesh()
        try:
            for vertex in evaluated_mesh.vertices:
                world_vertex = evaluated_obj.matrix_world @ vertex.co
                offset = world_vertex - center
                horizontal_radius = max(horizontal_radius, math.hypot(offset.x, offset.y))
                vertical_half_height = max(vertical_half_height, abs(offset.z))
        finally:
            evaluated_obj.to_mesh_clear()

    if not meshes:
        print(f"[blenderExport] WARNING: no mesh found parented to or deformed by "
              f"'{rig.name}' - falling back to default camera framing bounds. "
              f"Framing will likely be incorrect.")

    # fall back to sane defaults only if we genuinely found no geometry
    if horizontal_radius <= 0.0:
        horizontal_radius = 1.0
    if vertical_half_height <= 0.0:
        vertical_half_height = 1.0

    return horizontal_radius, vertical_half_height


def _compute_ortho_scale(horizontal_radius, vertical_half_height, pitch_degrees, sprite_width, sprite_height):
    # Derive the tightest orthographic scale that keeps the rig fully in
    # frame for every yaw angle at the given pitch, accounting for
    # non-square sprites via Blender's sensor_fit='AUTO' behavior (which
    # fits ortho_scale to the larger pixel dimension and derives the other
    # axis from the resolution aspect ratio).
    pitch_radians = math.radians(pitch_degrees)

    # the image's horizontal axis always sees the full horizontal sweep,
    # since the pivot orbits through every yaw angle
    frame_half_width = horizontal_radius

    # the image's vertical axis sees a blend of the rig's height and its
    # horizontal extent, proportional to how steeply we're looking down
    frame_half_height = (vertical_half_height * math.cos(pitch_radians)
                         + horizontal_radius * math.sin(pitch_radians))

    required_width = frame_half_width * 2.0
    required_height = frame_half_height * 2.0

    if sprite_width >= sprite_height:
        return max(required_width, required_height * (sprite_width / sprite_height))
    else:
        return max(required_height, required_width * (sprite_height / sprite_width))


def _create_export_camera(rig, pitch_degrees, ortho_padding, distance_multiplier, sprite_width, sprite_height):
    # Build a fresh pivot + orthographic camera from scratch rather than
    # trusting whatever camera/transform happens to already be in the scene.
    # The pivot sits at the rig's location with no rotation, and the camera
    # is placed at a known offset (yaw 0 = world +X, tilted up by pitch_degrees)
    # so that DIRECTION_YAWS is always correct by construction.
    horizontal_radius, vertical_half_height = _compute_rig_bounds(rig)
    distance = max(horizontal_radius, vertical_half_height) * distance_multiplier

    pivot = bpy.data.objects.new(f"{rig.name}_ExportCameraPivot", None)
    bpy.context.collection.objects.link(pivot)
    pivot.location = rig.matrix_world.translation.copy()
    pivot.rotation_euler = (0.0, 0.0, 0.0)

    camera_data = bpy.data.cameras.new(f"{rig.name}_ExportCamera")
    camera_data.type = 'ORTHO'
    camera_data.sensor_fit = 'AUTO'
    camera_data.ortho_scale = _compute_ortho_scale(
        horizontal_radius, vertical_half_height, pitch_degrees, sprite_width, sprite_height
    ) * ortho_padding
    camera_data.clip_start = max(distance - (horizontal_radius + vertical_half_height) * 2.0, 0.01)
    camera_data.clip_end = distance + (horizontal_radius + vertical_half_height) * 2.0

    camera = bpy.data.objects.new(f"{rig.name}_ExportCamera", camera_data)
    bpy.context.collection.objects.link(camera)
    camera.parent = pivot

    pitch_radians = math.radians(pitch_degrees)
    local_offset = Vector((
        distance * math.cos(pitch_radians),
        0.0,
        distance * math.sin(pitch_radians),
    ))
    camera.location = local_offset

    # orient the camera to look back at the pivot's origin, keeping world +Z as "up"
    camera.rotation_mode = 'QUATERNION'
    look_direction = (-local_offset).normalized()
    camera.rotation_quaternion = look_direction.to_track_quat('-Z', 'Y')

    bpy.context.view_layer.update()
    return pivot, camera


def _destroy_export_camera(pivot, camera):
    bpy.data.objects.remove(camera, do_unlink=True)
    bpy.data.objects.remove(pivot, do_unlink=True)


def _set_pivot_angle(pivot, angle_name):
    yaw_degrees = DIRECTION_YAWS[angle_name]
    pivot.rotation_euler = (0.0, 0.0, math.radians(yaw_degrees))
    bpy.context.view_layer.update()


def export_animations(output_path, mode, sprite_width=DEFAULT_SPRITE_WIDTH, sprite_height=DEFAULT_SPRITE_HEIGHT,
                       columns=DEFAULT_COLUMNS, ortho_padding=DEFAULT_ORTHO_PADDING,
                       camera_pitch_degrees=None,
                       camera_distance_multiplier=DEFAULT_CAMERA_DISTANCE_MULTIPLIER): # mode: isometric, top-down, etc
    # get all actions for the current blend file
    actions = bpy.data.actions
    if not actions:
        raise ValueError("No actions found in the blend file.")

    # get active rig
    rig = bpy.context.object
    if rig is None or rig.type != 'ARMATURE':
        raise ValueError("No active rig selected.")

    scene = bpy.context.scene

    # set up camera angles based on the mode
    if mode == "isometric":
        angles = ["up", "down", "left", "right", "up-left", "up-right", "down-left", "down-right"]
    elif mode == "top-down":
        angles = ["up", "down", "left", "right"]
    else:
        raise ValueError(f"Unsupported mode: {mode}")

    if camera_pitch_degrees is None:
        camera_pitch_degrees = DEFAULT_MODE_PITCH_DEGREES[mode]

    # resolve blender-relative paths (e.g. "//export") against the current blend file
    output_path = bpy.path.abspath(output_path)
    if not os.path.isdir(output_path):
        os.makedirs(output_path, exist_ok=True)

    # config render settings
    scene.render.image_settings.file_format = 'PNG'
    scene.render.image_settings.color_mode = 'RGBA'
    scene.render.resolution_x = sprite_width
    scene.render.resolution_y = sprite_height
    scene.render.resolution_percentage = 100
    scene.render.film_transparent = True

    # sprite settings
    width = sprite_width
    height = sprite_height

    # figure out how many tiles we need (action frames * directions) so the
    # sheet is sized to fit everything instead of relying on render resolution
    action_frame_counts = {
        action.name: int(action.frame_range[1]) - int(action.frame_range[0]) + 1
        for action in actions
    }
    total_tiles = sum(action_frame_counts.values()) * len(angles)

    rows = max(1, math.ceil(total_tiles / columns))
    sheet_width = columns * width
    sheet_height = rows * height

    # initialize sprite sheet image
    sprite_sheet = Image.new('RGBA', (sheet_width, sheet_height))

    # initialize json
    animation_data = {}
    sheet_filename = "sprite_sheet.png"

    # build a dedicated export camera/pivot from scratch rather than trusting
    # whatever camera is already in the scene, so framing and direction yaws
    # are always correct regardless of how the artist set up their scene
    pivot, camera = _create_export_camera(
        rig, camera_pitch_degrees, ortho_padding, camera_distance_multiplier, sprite_width, sprite_height
    )
    original_scene_camera = scene.camera
    scene.camera = camera

    try:
        tile_index = 0
        for angle in angles:
            _set_pivot_angle(pivot, angle)

            # iterate through actions and render each frame for this angle
            for action in actions:
                rig.animation_data.action = action

                # Sanitize names for safe file exporting
                action_name = "".join(
                     "_" if char in ' <>:"/\\|?*' or ord(char) < 32 else char
                     for char in action.name
                 )
                animation_id = f"{action_name}_{angle.replace("-", "_")}"
                sprite_cords_list = []

                for frame in range(int(action.frame_range[0]), int(action.frame_range[1]) + 1):
                    x = (tile_index % columns) * width
                    y = (tile_index // columns) * height
                    tile_index += 1

                    scene.frame_set(frame)
                    output_file = os.path.join(output_path, f"{animation_id}_{frame:04d}.png")
                    scene.render.filepath = output_file
                    bpy.ops.render.render(write_still=True)

                    # Add rendered frame to sprite sheet
                    frame_image = Image.open(output_file)
                    sprite_sheet.paste(frame_image, (x, y))
                    frame_image.close()

                    # clean up the temporary frame file
                    os.remove(output_file)

                    sprite_cords_list.append({"x": x, "y": y, "width": width, "height": height})

                # store animation data for this action/angle - see AnimationData struct in the C++ reference above
                animation_data[animation_id] = {
                    "spriteCords": sprite_cords_list,
                    "frameRate": 0.1,
                    "isLooping": True
                }
    finally:
        # always restore the scene's original camera and remove our temporary
        # export camera/pivot, even if rendering raised partway through
        scene.camera = original_scene_camera
        _destroy_export_camera(pivot, camera)

    # save the final sprite sheet
    sprite_sheet.save(os.path.join(output_path, sheet_filename))
    # save the animation data as JSON
    with open(os.path.join(output_path, "animation_data.json"), "w") as f:
        json.dump(animation_data, f, indent=4)


def _parse_args():
    # Blender passes its own args first, then "--", then args meant for this script.
    # When run from the Scripting tab/Text Editor there is no "--", so argv_after_dashdash
    # stays empty and defaults are used below.
    argv = sys.argv
    argv_after_dashdash = argv[argv.index("--") + 1:] if "--" in argv else []

    parser = argparse.ArgumentParser(description="Export Blender actions into a packed sprite sheet + animation JSON.")
    parser.add_argument("--output", dest="output_path", default=DEFAULT_OUTPUT_PATH,
                         help="Directory to write sprite_sheet.png and animation_data.json to.")
    parser.add_argument("--mode", dest="mode", default=DEFAULT_MODE, choices=["isometric", "top-down"],
                         help="Camera angle set to export.")
    parser.add_argument("--sprite-width", dest="sprite_width", type=int, default=DEFAULT_SPRITE_WIDTH,
                         help="Width in pixels of each sprite tile.")
    parser.add_argument("--sprite-height", dest="sprite_height", type=int, default=DEFAULT_SPRITE_HEIGHT,
                         help="Height in pixels of each sprite tile.")
    parser.add_argument("--columns", dest="columns", type=int, default=DEFAULT_COLUMNS,
                         help="Number of tile columns in the packed sprite sheet.")
    parser.add_argument("--ortho-padding", dest="ortho_padding", type=float, default=DEFAULT_ORTHO_PADDING,
                         help="Multiplier applied to the camera's orthographic scale to add headroom margin "
                              "and avoid clipping on angles that expose more of the character's silhouette.")
    parser.add_argument("--camera-pitch", dest="camera_pitch_degrees", type=float, default=None,
                         help="Degrees above the horizontal plane to tilt the generated export camera. "
                              "Defaults to 35.264 (true isometric) for --mode isometric, or 90 (straight down) "
                              "for --mode top-down.")
    parser.add_argument("--camera-distance-multiplier", dest="camera_distance_multiplier", type=float,
                         default=DEFAULT_CAMERA_DISTANCE_MULTIPLIER,
                         help="How far back to place the generated export camera, as a multiple of the rig's "
                              "computed bounding radius.")

    return parser.parse_args(argv_after_dashdash)


if __name__ == "__main__":
    args = _parse_args()
    export_animations(
        output_path=args.output_path,
        mode=args.mode,
        sprite_width=args.sprite_width,
        sprite_height=args.sprite_height,
        columns=args.columns,
        ortho_padding=args.ortho_padding,
        camera_pitch_degrees=args.camera_pitch_degrees,
        camera_distance_multiplier=args.camera_distance_multiplier,
    )

