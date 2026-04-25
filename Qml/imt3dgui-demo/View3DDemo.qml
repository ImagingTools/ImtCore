/*
 * View3DDemo.qml - small demo of the imt3dgui module.
 *
 * Renders a red cube under a directional light, viewed by a perspective
 * camera tilted slightly downward. The same QML compiles to:
 *   - the desktop (via the imt3dgui pure-QML module)
 *   - three.js on the web   (via the JQML compiler & web runtime)
 */
import QtQuick 2.15
import imt3dgui 1.0

Item {
    id: root
    width: 800
    height: 600

    View3D {
        id: view3d
        anchors.fill: parent
        clearColor: "#202020"

        environment: SceneEnvironment {
            backgroundMode: 0      // Color
            clearColor: "#202830"
            antialiasingMode: 2    // MSAA
            antialiasingQuality: 1 // High
        }

        camera: PerspectiveCamera {
            id: cam
            position: Qt.vector3d(0, 200, 400)
            eulerRotation: Qt.vector3d(-30, 0, 0)
            fieldOfView: 45
            clipNear: 1
            clipFar: 5000
        }

        DirectionalLight {
            id: keyLight
            eulerRotation: Qt.vector3d(-45, 30, 0)
            brightness: 1.0
            color: "white"
        }

        AmbientLight {
            brightness: 0.15
            color: "white"
        }

        Model {
            id: cube
            source: "#Cube"
            position: Qt.vector3d(0, 0, 0)
            scale: Qt.vector3d(1, 1, 1)
            eulerRotation: Qt.vector3d(0, _spin, 0)

            // Drive the rotation through a full vector3d binding so the same
            // QML works on the desktop and on the web (JQML + three.js);
            // animating sub-properties (e.g. `eulerRotation.y`) is a Qt-only
            // QML feature not supported by the JQML compiler.
            property real _spin: 0
            NumberAnimation on _spin {
                from: 0; to: 360
                duration: 6000
                loops: Animation.Infinite
                running: true
            }

            materials: PrincipledMaterial {
                baseColor: "red"
                metalness: 0.2
                roughness: 0.5
            }
        }

        Model {
            source: "#Sphere"
            position: Qt.vector3d(150, 50, 0)
            materials: PrincipledMaterial {
                baseColor: "#3399ff"
                metalness: 0.8
                roughness: 0.2
            }
        }

        Model {
            source: "#Plane"
            position: Qt.vector3d(0, -75, 0)
            eulerRotation: Qt.vector3d(-90, 0, 0)
            scale: Qt.vector3d(8, 8, 1)
            materials: PrincipledMaterial {
                baseColor: "#888888"
                roughness: 1.0
            }
        }
    }
}
