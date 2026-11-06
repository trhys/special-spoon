# Project Blueprints

A **project blueprint** is a reusable starter-project template, distinct from
[entity blueprints](../Special-Spoon/src/Editor/Blueprints/Blueprint.h). It
declares a project profile, a starter scene, and optional project-config
overrides. The built-in templates are embedded into the engine binary at build
time (see `ProjectTemplates/` under `src/Editor/Blueprints/`) and offered in
the editor's *Create New Project* prompt.

## Project blueprint schema

```json
{
    "schemaVersion": 1,
    "name": "Platformer Starter",
    "profile": "platformer",
    "scene": "Scenes/Platformer-Main.scene.json",
    "configOverrides": {
        "PhysicsEnabled": true
    }
}
```

- `name` — display name shown in the template list.
- `profile` — `isometric`, `platformer`, `topdown`, or `custom`. Selects
  profile-derived defaults for the project config (see precedence below).
- `scene` — path to a starter scene template, relative to the template set.
- `configOverrides` — optional partial `ProjectConfig` JSON; keys present here
  replace the profile defaults.

## Scene template schema

The starter scene describes entity-blueprint instances, their names, and
positions:

```json
{
    "schemaVersion": 1,
    "name": "Main",
    "entities": [
        {
            "blueprint": "Blueprints/Player.json",
            "name": "Player",
            "position": { "x": 100, "y": 100 }
        }
    ]
}
```

- `blueprint` — entity blueprint path, resolved against the project's
  `assets/blueprints/` directory (an optional leading `Blueprints/` prefix is
  accepted). New projects are seeded with the built-in entity blueprints, so
  templates can rely on them being present.
- `position` — optional; overrides the blueprint's `Transform` position (one
  is injected if the blueprint has no transform).

Scaffolding expands each instance into the regular serialized scene format, so
the resulting scene loads through the standard scene/component loaders with no
special-casing.

## Config precedence

Project config values are composed at project creation
(`ComposeProjectConfig`), in this order:

1. **Explicit overrides** — `configOverrides` keys in the project blueprint.
2. **Profile defaults** — the profile-to-defaults mapping in
   `ProjectBlueprint.cpp` (e.g. `platformer` enables physics and top-down
   sorting; `isometric` selects isometric sorting).
3. **Engine fallback** — the `ProjectConfig` defaults.

The profile only informs this composition layer. Core systems keep consuming
`ProjectConfig` (e.g. the renderer reads `SortPolicy`) and never branch on the
profile itself.
