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
