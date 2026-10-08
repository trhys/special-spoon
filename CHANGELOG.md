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
