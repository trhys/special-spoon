# Exports animations from blend file into a packed sprite sheet
# with a companion JSON file that we'll use to import into
# animation data on the engine side

# run from inside blender - assumes the current selection is the object to be animated
# iterate actions, assign each to the rig, and export each animation onto the same sheet

# generates a companion JSON file with animation data we can deserialize
# as sprite cords and animation data (see Special-Spoon/src/.../AnimationData.h)

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
from PIL import Image

def export_animations(output_path, mode): # mode: isometric, top-down, etc
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

    # config render settings
    scene.render.image_settings.file_format = 'PNG'
    scene.render.image_settings.color_mode = 'RGBA'
    scene.render.resolution_x = 1024 # todo: make this configurable
    scene.render.resolution_y = 1024 # ""
    scene.render.resolution_percentage = 100
    scene.render.film_transparent = True

    # sprite settings
    width = 128 # todo: make this configurable
    height = 128

    # initialize sprite sheet image
    sprite_sheet = Image.new('RGBA', (scene.render.resolution_x, scene.render.resolution_y))

    # initialize json
    animation_data = {}

    # iterate through actions and render each frame
    tile_index = 0
    columns = scene.render.resolution_x // width
    for action in actions:
        rig.animation_data.action = action

        # Sanitize names for safe file exporting
        action_name = action.name.replace(" ", "_").replace("/", "-")
        
        for frame in range(int(action.frame_range[0]), int(action.frame_range[1]) + 1):
            sprite_cords = (
                (tile_index % columns) * width,
                (tile_index // columns) * height,
                width,
                height
            )
            tile_index += 1
            scene.frame_set(frame)
            output_file = os.path.join(output_path, f"{action_name}_{frame:04d}.png")
            scene.render.filepath = output_file
            bpy.ops.render.render(write_still=True)

            # Add rendered frame to sprite sheet
            frame_image = Image.open(output_file)
            sprite_sheet.paste(frame_image, (sprite_cords[0], sprite_cords[1]))
            frame_image.close()

            # clean up the temporary frame file
            os.remove(output_file)

        # store animation data for this action - see AnimationData struct in the C++ reference above
        animation_data[action_name] =  {
            "ID": action_name,
            "textureID": sprite_sheet,
            "spriteCords": [
                {
                    "x": (i % columns) * width,
                    "y": (i // columns) * height,
                    "width": width,
                    "height": height
                } for i in range(int(action.frame_range[0]), int(action.frame_range[1]) + 1)
            ]
        }

    # save the final sprite sheet
    sprite_sheet.save(os.path.join(output_path, "sprite_sheet.png"))
    # save the animation data as JSON
    with open(os.path.join(output_path, "animation_data.json"), "w") as f:
        json.dump(animation_data, f, indent=4)

