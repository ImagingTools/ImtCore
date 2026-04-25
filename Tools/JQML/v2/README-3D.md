# JQML2 — 3D Visualization Support

This document describes the 3D component family added to JQML2, which mirrors
**Qt Quick 3D** so a single QML source tree can target both the Qt desktop
build and the JQML web runtime.

## Web 3D Framework

**Three.js (MIT)** is used as the Web 3D backend. Reasoning:

| Reason | Detail |
|---|---|
| 1:1 API mapping to Qt Quick 3D | `THREE.Scene/Camera/Light/Mesh/Material` → `View3D/PerspectiveCamera/…/Model/PrincipledMaterial` |
| Lightweight, tree-shakable | ~150 kB min+gz core, plus loaders on demand |
| Standard glTF (.glb/.gltf) asset workflow | identical to Qt Quick 3D's recommended Balsam pipeline |
| MIT licensed | Compatible with the rest of the stack |
| Mature, well-documented | Accelerates adoption by team with Qt background |
| WebGPU-ready | Future-proof via `WebGPURenderer` |

Three.js (and the `GLTFLoader`) are loaded **lazily** via dynamic `import()` —
nothing is added to the 2D bundle until the first `View3D` is instantiated.
Three.js is declared as an **optional `peerDependency`** in `package.json`.

## Component Mapping

| QML / JQML type            | Qt Quick 3D type            | Three.js type                       |
|----------------------------|------------------------------|---------------------------------------|
| `View3D`                   | `View3D`                    | `WebGLRenderer` + `Scene`            |
| `Node3D`                   | `Node`                      | `Object3D` / `Group`                 |
| `Model`                    | `Model`                     | `Mesh` (with `BoxGeometry`/`SphereGeometry`/… or glTF) |
| `PerspectiveCamera`        | `PerspectiveCamera`         | `PerspectiveCamera`                  |
| `OrthographicCamera`       | `OrthographicCamera`        | `OrthographicCamera`                 |
| `DirectionalLight`         | `DirectionalLight`          | `DirectionalLight`                   |
| `PointLight`               | `PointLight`                | `PointLight`                         |
| `SpotLight`                | `SpotLight`                 | `SpotLight`                          |
| `AmbientLight`             | (n/a — use IBL)             | `AmbientLight`                       |
| `PrincipledMaterial`       | `PrincipledMaterial`        | `MeshStandardMaterial`               |
| `DefaultMaterial`          | `DefaultMaterial`           | `MeshPhongMaterial`                  |
| `Texture`                  | `Texture`                   | `Texture` via `TextureLoader`        |
| `CubeMapTexture`           | `CubeMapTexture`            | `CubeTexture` via `CubeTextureLoader` |
| `SceneEnvironment`         | `SceneEnvironment`          | `Scene.background` / `Scene.environment` + tone-mapping |

### Mesh primitives

`Model.source` accepts the same special tokens as Qt Quick 3D:

| Token        | Geometry                              |
|--------------|----------------------------------------|
| `"#Cube"`    | `BoxGeometry(100,100,100)`             |
| `"#Sphere"`  | `SphereGeometry(50,32,16)`             |
| `"#Cylinder"`| `CylinderGeometry(50,50,100,32)`       |
| `"#Cone"`    | `ConeGeometry(50,100,32)`              |
| `"#Plane"`   | `PlaneGeometry(100,100)`               |
| `"#Rectangle"`| `PlaneGeometry(100,100)`              |

URLs ending in `.glb` / `.gltf` are loaded asynchronously via `GLTFLoader`
(also lazy-imported).

### Value types

Four new `QProperty` subclasses are added to `utils/properties.js`:

- `QVector3D`  — `{ x, y, z }`
- `QVector4D`  — `{ x, y, z, w }`
- `QQuaternion` — `{ scalar, x, y, z }` (also accepts `{ x, y, z, w }`)
- `QMatrix4x4` — 16-element row-major array

Corresponding helpers are added to `utils/Qt.js`:

- `Qt.vector3d(x, y, z)`
- `Qt.vector4d(x, y, z, w)`
- `Qt.quaternion(scalar, x, y, z)`
- `Qt.matrix4x4(...16 values)` / `Qt.matrix4x4(arr)` / `Qt.matrix4x4()` (identity)

### Rotation conventions

Qt Quick 3D's `eulerRotation` is in **degrees, ZYX order**. The JQML web
runtime sets `THREE.Object3D.rotation.order = 'ZYX'` and converts degrees ↔
radians inside `Node3D.$applyRotation()`. If both `eulerRotation` and a
non-identity `rotation` (quaternion) are set, the quaternion wins.

## Compiler integration

The JQML compiler discovers components purely through
`Tools/JQML/v2/components/list.js`. Adding a 3D type only requires:

1. dropping a `.js` file in `Tools/JQML/v2/components/`, and
2. registering its name in `list.js`.

No special-cased compiler logic is needed: `View3D` declares itself as a
3D host via the `$isView3D = true` marker, and `Node3D.$findHost3D()` walks
up the QML parent chain to register each 3D node with the enclosing
`View3D`. `View3D.addDomChild()` takes care of moving any `Node3D` that was
inadvertently inserted as a DOM child back into the 3D scene graph.

## Render strategy

`View3D` uses an **on-demand render loop**: every property change that
affects the visual result calls `View3D.$requestRender()`, which schedules a
single `requestAnimationFrame`. Idle scenes don't burn CPU/GPU. A
`ResizeObserver` keeps the renderer canvas in sync with the Item geometry.

## Picking and 3D ↔ 2D mapping

`View3D` exposes:

- `pick(x, y)` — returns the closest `Node3D` under a 2D screen coordinate
  (uses `THREE.Raycaster` against scene children); `null` if nothing was hit.
- `mapFrom3DScene({x, y, z})` — projects a world-space point to View3D-local
  screen coordinates.
- `mapTo3DScene({x, y, z})` — inverse projection (z is normalised depth in
  `[-1, 1]`).

## Desktop module

`Qml/imt3dgui/` provides pure-QML wrapper components (property-bag
`QtObject`/`Item` types) that expose the same property API that the JQML web
runtime understands.  Apps written against `import imt3dgui 1.0` therefore
compile to both worlds without any source-level changes.

On the desktop the actual 3D rendering is handled by the C++ backend
(`CRhiWidget` / `COpenGLWidget`).  The QML types serve as a declarative
scene-description layer and do **not** depend on the GPL-licensed QtQuick3D
module.

A small demo lives under `Qml/imt3dgui-demo/View3DDemo.qml`.

## Limitations / Future work

- **Materials per submesh.** A `Model` currently applies only `materials[0]`
  to all submeshes; per-submesh assignment is a future iteration.
- **Tone mapping / IBL fidelity.** Look matches the JQML web backend *approximately*;
  exact pixel parity with any particular 3D engine is out of scope.
- **WebGPU.** Three.js' `WebGPURenderer` is API-compatible and can replace
  `WebGLRenderer` later.
- **`pick()` on lights/cameras.** Only nodes with a hit-testable `THREE.Mesh`
  underneath them are picked.
