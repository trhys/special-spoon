# CHANGELOG
## 1/13/26: 

#### Editor
- Changed the raycast in the editor viewport to toggle based on click and release. This prevents the rapid cycling behavior

#### Misc
- Moved scripts to folder

note: Previous changes will not be in this log. This record will keep all changes going forward from 1-13-2026.

---

## 9/11/26:

- Refactored action and state handling into typed registries with typed action/state IDs and updated serialization.
- Added a new movement component and updated input/movement flow to support held movement actions.
- Introduced physics component improvements and collision resolution handling.
- Added a new state system and updated system execution order for movement, physics, and animation.
- Updated editor/entity management to use display names and improved default component setup.
- Removed older player/patrol-specific ECS code in favor of the new generalized action/state pipeline.

---

## 9/24/26:

### Tile Maps
- Added a layered tile map ECS component with per-layer tile data, atlas metadata, and validation utilities for map bounds and tile values.
- Added editor tooling for creating/removing/reordering map layers, painting tiles, resizing layer data, and selecting atlas tiles.
- Added atlas-backed tile rendering for layered map data.
- Added runtime dirty-state rebuilds for tile map rendering and collision generation.
- Added scene serialization support for tile map data, atlas settings, layer metadata, and tile layouts.

### Physics and Collision
- Added a robust collision system for ECS-driven physics using continuous AABB sweep resolution and broadphase pruning.
- Added runtime body construction and contact processing for static, dynamic, and kinematic bodies.
- Added discrete correction passes, persistent contact handling, restitution, friction, and motion syncing between physics and movement state.
- Added quadtree broadphase support, TOI-based AABB collision resolution, and discrete circle collision handling.
- Integrated collision response with movement and transform updates so collider interactions resolve cleanly against entity motion.
- Added support for layer-driven tile-map collision generation by baking solid tile regions into static collider bodies.

### Editor
- Improved tile map tool behavior for painting, erasing, layer management, and atlas-based placement.

### Serialization
- Added support for marking entities as runtime-only, which prevents them from being serialized.

### Misc
- Hardened entity generation and destruction to prevent invalid states and ensure proper recycling of entity IDs.
- Implemented buffered component reaping to safely handle orphaned components.

---

## 9/30/26:

### Blueprints
- Added an editor for creating, editing, and deleting entity blueprints with searchable component selection and unique-name validation.
- Added project-scoped blueprint storage and JSON serialization, including default blueprints for new projects and projects without saved blueprint data.
- Updated entity creation to use the current project's blueprints and automatically add their selected components.

---

## 10/7/26:

### Isometric projection / coordinate spaces
- Added `WorldProjection` (`Core/Renderer/WorldProjection.h`) documenting the coordinate-space contract: transforms, colliders, physics and tile collision stay in logical space; only presentation is projected.
- Projection is gated by the project config's Projection Style (`SortPolicy`). Added a `Top-Down` style that draws logical positions as-is.
- Tilemap render cache, editor tile picking/overlays and the collider debug overlay share the projection helpers. Tile collision is still built from the logical tile grid.
- Sprites keep logical transforms and are projected at render time (feet placed on the projected center of their logical footprint).
- Dragging an entity in the editor viewport inverse-projects the mouse before storing the transform.

### Audi System/Component
- Added AudioComp to ECS with it's editor methods attached
- Added Audio System to iterate on AudioComp and pulls actions from the ActionQueue, checks them against the component's mappings, and plays and matches
- Added a large sampling of UI sound effects in the Sandbox/Assets folder

---

## 10/9/26:

### Particle System

- Added a new particle emitter component and particle system for spawning, updating, and rendering configurable particle effects.
- Added editor controls for particle emitters, including emission rate, spread, velocity, lifetime, size, colors, texture selection, and active/looping state.
- Added particle preset save/load support, with project-scoped preset storage under the assets presets directory.
- Registered the particle emitter component and particle system in the ECS/system registries so they are available in-game and in the editor.
- Added support for loading and editing particle emitter texture regions directly from the editor.

### Project / Editor

- Added project preset path handling to project creation and serialization.
- Updated texture rect editing to work through the editor’s texture rect tool API.
- Adjusted tile map editor access to work directly from the selected tile map component.

### Misc

- Updated sprite rendering so inactive sprites no longer render.
- Refactored editor and component interfaces to support the new particle and texture editing workflows.

---

## 10/12/26:

### Movement / Animation
- Reworked movement-driven animation state selection to support 8-direction facing and idle transitions.
- Consolidated action-to-state mappings into `StatusComp` and removed the legacy `StateAction` component.
- Updated animation mapping UI and lookup logic to better handle missing or changing mappings.
- Refactored movement projection and facing logic so animation state matches logical movement direction.

### Editor / Serialization
- Improved animation frame selection tooling with better drag behavior and selection handling.
- Updated component loading and serialization to support the new `StatusComp` structure.

### Misc
- Adjusted resource and entity loading to match the new component layout.
- Updated system registration and build configuration for the movement pipeline changes.

---

## 10/16/26:

### UI / Input
- Added a new UI component and UI system for screen-space and world-space hit testing, focus tracking, navigation, and action emission.
- Reworked input handling into a more flexible binding system with keyboard and mouse sources, pressed/released/held/repeat triggers, and viewport-aware routing.
- Added UI actions and updated action registry support for select, cancel, and navigate flow.

### Rendering
- Split rendering into world-space and screen-space buckets and added projection-aware rendering hooks for sprites, text, and particles.
- Updated UI, text, and particle rendering to preserve logical positions while applying presentation-time projection.

### Editor / Serialization
- Added editor tools for editing UI components and input bindings.
- Updated component and system loading/registration to support the new UI and reorganized core component/system layout.

### Refactor
- Reorganized core, physics, render, and component files into clearer subdirectories.
- Removed legacy input binding code and old asset/license files.

---

## 10/19/26:

### Editor / Refactor
- Moved viewport rendering and entity interaction logic from `Application` into the editor.
- Added a shared `Services` structure to pass renderer, manager, and input dependencies into the editor.
- Split the editor dashboard into separate menu bar, inspector, and performance metrics widgets.
- Reorganized project, entity, blueprint, scene, resource, and system menus into dedicated directories and editor methods.
- Moved menu visibility, selection, and confirmation state into the editor.
- Added a shared window-centering helper and updated editor build configuration.

### Project Management
- Separated project creation, loading, and saving operations from editor dialogs and prompts.
- Updated project operations to return project pointers, with editor-side handling for failures.
- Renamed the project identifier field and JSON key from `ID` to `name`.
- Centralized editor project activation, including asset scanning and scene manifest loading.

### Application / Shutdown
- Routed window-close events and Quit actions through `Application::HandleQuit()`.
- Moved the save-and-exit confirmation dialog into the editor.
- Allowed Save and Exit to close the application when no scene is active.

---
