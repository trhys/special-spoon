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

# yaw (degrees around world Z) applied to the camera pivot for each named direction.
# the pivot orbits the rig while keeping the camera's original pitch/distance intact,
# so only "top-down"/"isometric" camera rigs with a fixed downward tilt are supported.
DIRECTION_YAWS = {
    "right": 0,
    "up-right": 45,
    "up": 90,
    "up-left": 135,
    "left": 180,
    "down-left": 225,
    "down": 270,
    "down-right": 315,
}

DEFAULT_OUTPUT_PATH = "//export"
DEFAULT_MODE = "isometric"
DEFAULT_SPRITE_WIDTH = 128
DEFAULT_SPRITE_HEIGHT = 128
DEFAULT_COLUMNS = 16
DEFAULT_ORTHO_PADDING = 1.1  # 10% extra headroom to avoid clipping on angles that expose more silhouette


def _create_camera_pivot(rig, camera):
    # create an empty at the rig's location and parent the camera to it,
    # preserving the camera's current world transform (keep transform parenting)
    pivot = bpy.data.objects.new(f"{rig.name}_CameraPivot", None)
    bpy.context.collection.objects.link(pivot)
    pivot.location = rig.matrix_world.translation.copy()

    original_parent = camera.parent
    original_matrix_parent_inverse = camera.matrix_parent_inverse.copy()
    original_matrix_world = camera.matrix_world.copy()

    camera.parent = pivot
    camera.matrix_parent_inverse = pivot.matrix_world.inverted()
    camera.matrix_world = original_matrix_world

    return pivot, original_parent, original_matrix_parent_inverse, original_matrix_world


def _destroy_camera_pivot(camera, pivot, original_parent, original_matrix_parent_inverse, original_matrix_world):
    # restore the camera's original parenting/transform and remove the pivot
    camera.parent = original_parent
    camera.matrix_parent_inverse = original_matrix_parent_inverse
    camera.matrix_world = original_matrix_world
    bpy.data.objects.remove(pivot, do_unlink=True)


def _set_pivot_angle(pivot, angle_name):
    yaw_degrees = DIRECTION_YAWS[angle_name]
    pivot.rotation_euler = (0.0, 0.0, math.radians(yaw_degrees))
    bpy.context.view_layer.update()


def _ensure_orthographic_camera(camera, ortho_padding):
    # Perspective cameras foreshorten geometry based on distance, so an
    # asymmetric character (e.g. forward-leaning head, hair, backpack) will
    # clip differently depending on which side faces the camera as it orbits.
    # Orthographic projection keeps apparent scale consistent across all angles.
    original_type = camera.data.type
    original_ortho_scale = camera.data.ortho_scale

    if camera.data.type != 'ORTHO':
        print(f"[blenderExport] Camera '{camera.name}' was '{camera.data.type}', "
              f"switching to 'ORTHO' to avoid per-angle clipping.")
        camera.data.type = 'ORTHO'
        # perspective cameras don't have a meaningful ortho_scale set up, so
        # derive a reasonable starting scale from the camera's current lens/distance
        if original_ortho_scale <= 0:
            camera.data.ortho_scale = 5.0

    # add padding regardless, since even an already-orthographic camera can be
    # framed too tightly for angles that expose more of the character's silhouette
    camera.data.ortho_scale *= ortho_padding

    return original_type, original_ortho_scale


def _restore_camera_projection(camera, original_type, original_ortho_scale):
    camera.data.type = original_type
    camera.data.ortho_scale = original_ortho_scale


def export_animations(output_path, mode, sprite_width=DEFAULT_SPRITE_WIDTH, sprite_height=DEFAULT_SPRITE_HEIGHT,
                       columns=DEFAULT_COLUMNS, ortho_padding=DEFAULT_ORTHO_PADDING): # mode: isometric, top-down, etc
    # get all actions for the current blend file
    actions = bpy.data.actions
    if not actions:
        raise ValueError("No actions found in the blend file.")

    # get active rig
    rig = bpy.context.object
    if rig is None or rig.type != 'ARMATURE':
        raise ValueError("No active rig selected.")

    # get camera
    scene = bpy.context.scene
    camera = scene.camera
    if camera is None:
        raise ValueError("No camera found in the scene.")

    # set up camera angles based on the mode
    if mode == "isometric":
        angles = ["up", "down", "left", "right", "up-left", "up-right", "down-left", "down-right"]
    elif mode == "top-down":
        angles = ["up", "down", "left", "right"]
    else:
        raise ValueError(f"Unsupported mode: {mode}")

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

    # force an orthographic camera (with headroom padding) so apparent character
    # scale/framing stays consistent across every orbit angle, rather than
    # clipping only on angles that expose more of the silhouette to the camera
    original_camera_type, original_ortho_scale = _ensure_orthographic_camera(camera, ortho_padding)

    # set up a pivot so the camera can orbit the rig per-direction without
    # disturbing its original pitch/distance
    pivot, original_parent, original_matrix_parent_inverse, original_matrix_world = _create_camera_pivot(rig, camera)

    try:
        tile_index = 0
        for angle in angles:
            _set_pivot_angle(pivot, angle)

            # iterate through actions and render each frame for this angle
            for action in actions:
                rig.animation_data.action = action

                # Sanitize names for safe file exporting
                action_name = action.name.replace(" ", "_").replace("/", "-")
                animation_id = f"{action_name}_{angle.replace('-', '_')}"
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
                    "ID": animation_id,
                    "textureID": sheet_filename,
                    "spriteCords": sprite_cords_list,
                    "frameRate": 0.1,
                    "isLooping": True
                }
    finally:
        # always restore the camera, even if rendering raised partway through
        _destroy_camera_pivot(camera, pivot, original_parent, original_matrix_parent_inverse, original_matrix_world)
        _restore_camera_projection(camera, original_camera_type, original_ortho_scale)

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
    )

